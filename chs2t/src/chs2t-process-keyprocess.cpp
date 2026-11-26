#include    "chs2t.h"

#include    <automatic-train-stop.h>
#include    <brake-crane.h>
#include    <loco-crane.h>

#include    <cmath>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::keyProcess(const simulator_time_t& t, const double& dt)
{
    // Не допускаем двух ключей в электропневматических клапанах автостопа
    epk[CAB2]->allowKey(!(epk[CAB1]->isKey()));
    epk[CAB1]->allowKey(!(epk[CAB2]->isKey()));

    // Не допускаем двух ключей в панелях переключателей
    sw_panel[CAB2].allowKey(!(sw_panel[CAB1].isKey()));
    sw_panel[CAB1].allowKey(!(sw_panel[CAB2].isKey()));

    // Не допускаем двух реверсивных рукояток в контроллерах машиниста
    km21KR2[CAB2].allowReversHandle(!(km21KR2[CAB1].isReversHandle()));
    km21KR2[CAB1].allowReversHandle(!(km21KR2[CAB2].isReversHandle()));

    // Автозапуск и автовыключение
    if (autoStartTimer->isStarted() || autoStopTimer->isStarted())
    {
        return;
    }

    // Активная кабина (там, где вставлена реверсивная рукоятка)
    int active_cab = (km21KR2[CAB2].isReversHandle()) ? CAB2 : CAB1;

    // Автозапуск
    if (vc_inputs.getBool(active_cab, IO_AUTOSTART) && initAutostartProgram(active_cab))
    {
        autoStartTimer->start();
        return;
    }

    // Автовыключение
    if (vc_inputs.getBool(active_cab, IO_AUTOSTOP) && initAutostopProgram(active_cab))
    {
        autoStopTimer->start();
        return;
    }

    for (size_t cab_idx : {CAB1, CAB2})
    {
        // Управляем краном, учитывая возможное наличие внешнего пульта
        if (vc_inputs.contains(cab_idx, IO_CRANE_395))
        {
            int brake_crane_pos = static_cast<int>(std::lround(vc_inputs.getValue(cab_idx, IO_CRANE_395)));
            brake_crane[cab_idx]->setHandlePosition(brake_crane_pos);
        }
        else if (control_signals.analogSignal[CS_BRAKE_CRANE].is_active)
        {
            int brake_crane_pos = static_cast<int>(control_signals.analogSignal[CS_BRAKE_CRANE].cur_value);
            brake_crane[cab_idx]->setHandlePosition(brake_crane_pos);
        }
        else
        {
            brake_crane[cab_idx]->setControl(&pressed_keys_by_cabine[cab_idx]);
        }

        // Управляем краном, учитывая возможное наличие внешнего пульта
        if (vc_inputs.contains(cab_idx, IO_CRANE_254))
        {
            double pos = vc_inputs.getValue(cab_idx, IO_CRANE_254);
            loco_crane[cab_idx]->release(false);
            loco_crane[cab_idx]->setHandlePosition(pos);
        }
        else if (control_signals.analogSignal[CS_LOCO_CRANE].is_active)
        {
            double pos = 0.0;

            if (static_cast<bool>(control_signals.analogSignal[CS_RELEASE_VALVE].cur_value))
            {
                loco_crane[cab_idx]->release(true);
                pos = -1.0;
            }
            else
            {
                loco_crane[cab_idx]->release(false);
                pos = control_signals.analogSignal[CS_LOCO_CRANE].cur_value;
            }

            loco_crane[cab_idx]->setHandlePosition(pos);
        }
        else
        {
            loco_crane[cab_idx]->setControl(&pressed_keys_by_cabine[cab_idx]);
        }

        sw_panel[cab_idx].step(t.simulation_seconds, dt);

        button_svistok[cab_idx].step();
        button_tifon[cab_idx].step();
        button_sand[cab_idx].step();
        button_loco_release[cab_idx].step();
        button_sbros_cpc[cab_idx].step();

        // Нажатие РБС
        // Если активна РБС на внешнем пульте
        // TODO // перенести freejoy во вьювер, его команды передавать по сети,
        // TODO // и также указывая индекс кабины
        if (control_signals.analogSignal[CS_RBS].is_active)
        {
            // реагируем на состояние РБС на внешнем пульте
            if (static_cast<bool>(control_signals.analogSignal[CS_RBS].cur_value))
                rb[cab_idx][RBS].set();
            else
                rb[cab_idx][RBS].reset();
        }
        else // иначе
        {
            // обрабатываем клавиши
            rb[cab_idx][RBS].step();
        }
        rb[cab_idx][RB1].step();
        rb[cab_idx][RBP].step();

        autopilot_switcher[cab_idx].step();

        shunting_mode_switcher[cab_idx].step();
    }
}
