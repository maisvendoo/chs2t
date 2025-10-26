#include "chs2t.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::keyProcess(const simulator_time_t& t, const double& dt)
{
    // Не допускаем двух ключей в электропневматических клапанах автостопа
    epk[CAB2]->allowKey(!(epk[CAB1]->isKey()));
    epk[CAB1]->allowKey(!(epk[CAB2]->isKey()));

    // Не допускаем двух реверсивных рукояток в контроллерах машиниста
    km21KR2[CAB2]->allowReversHandle(!(km21KR2[CAB1]->isReversHandle()));
    km21KR2[CAB1]->allowReversHandle(!(km21KR2[CAB2]->isReversHandle()));

    for (size_t cab_idx : {CAB1, CAB2})
    {
        // Управляем краном, учитывая возможное наличие внешнего пульта
        // TODO // перенести freejoy во вьювер, его команды передавать по сети,
        // TODO // и также указывая индекс кабины
        if (control_signals.analogSignal[CS_BRAKE_CRANE].is_active)
        {
            int brake_crane_pos = static_cast<int>(control_signals.analogSignal[CS_BRAKE_CRANE].cur_value);
            brake_crane[cab_idx]->setHandlePosition(brake_crane_pos);
        }
        else
        {
            brake_crane[cab_idx]->setControl(&pressed_keys_by_cabine[cab_idx]);
        }

        // Управляем краном, учитывая возможное наличие внешнего пульта
        // TODO // перенести freejoy во вьювер, его команды передавать по сети,
        // TODO // и также указывая индекс кабины
        if (control_signals.analogSignal[CS_LOCO_CRANE].is_active)
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

        handleEDT[cab_idx]->setControl(&pressed_keys_by_cabine[cab_idx], &control_signals);

        button_svistok[cab_idx].step();
        button_tifon[cab_idx].step();
        button_sand[cab_idx].step();
        button_loco_release[cab_idx].step();
        epb_switch[cab_idx].step();
        motor_fan_switcher[cab_idx].step();
        mk_switcher[cab_idx][MK1].step();
        mk_switcher[cab_idx][MK2].step();
        pant_switcher[cab_idx][PANT1].step();
        pant_switcher[cab_idx][PANT2].step();
        fastswitch_switcher[cab_idx].step();
        blinds_switcher[cab_idx].step();
        cab_light_switcher[cab_idx].step();
        bufferlight_L_switcher[cab_idx].step();
        bufferlight_R_switcher[cab_idx].step();
        spotlight_switcher[cab_idx].step();

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
        EDT_switch[cab_idx].step();
        button_sbros_cpc[cab_idx].step();
    }

}
