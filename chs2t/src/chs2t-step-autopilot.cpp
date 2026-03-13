#include    <chs2t.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepAutopilot(double t, double dt)
{
    double v_lim = 0;
    double v_lim_next = 0;
    double limit_dist = 0;
    double signal_dist = 0;
    ALSN alsn_code = ALSN::NO_CODE;

    int cab_idx = 0;

    if (km21KR2[CAB1].isReversHandle())
    {
        v_lim = speedmap_fwd->getCurrentLimit();
        v_lim_next = speedmap_fwd->getNextLimit();
        limit_dist = speedmap_fwd->getNextLimitDistance();
        alsn_code = coil_ALSN_fwd->getCode();
        signal_dist = coil_ALSN_fwd->getNextSignalDistance();

        cab_idx = CAB1;
    }

    if (km21KR2[CAB2].isReversHandle())
    {
        v_lim = speedmap_bwd->getCurrentLimit();
        v_lim_next = speedmap_bwd->getNextLimit();
        limit_dist = speedmap_bwd->getNextLimitDistance();
        alsn_code = coil_ALSN_bwd->getCode();
        signal_dist = coil_ALSN_bwd->getNextSignalDistance();

        cab_idx = CAB2;
    }

    if (autopilot[cab_idx] == nullptr)
    {
        return;
    }

    // Включение и выключение автоведения
    autopilot_switcher[cab_idx].getState() ? autopilot[cab_idx]->on() : autopilot[cab_idx]->off();

    // Сигнал контроля бдительности от цепей прибора безопасности
    auto_feedback[cab_idx]->is_vigilance_control = safety_device[cab_idx]->getEPKstate();
    // TODO: специфичные для данного локомотива сигналы обратной связи


    // Принимаем сигналы обратной связи от оборудования
    autopilot[cab_idx]->setFeedback(auto_feedback[cab_idx]);

    // Выполняем шаг управления
    autopilot[cab_idx]->step(t, dt);

    // Получаем управляющие воздействия
    auto_control[cab_idx] = dynamic_cast<chs2t_control_t *>(autopilot[cab_idx]->getControl());

    // Действия по управлению, только если автоведение активно
    if (autopilot[cab_idx]->isActive())
    {
        // Проверка бдительности
        auto_control[cab_idx]->press_RB ? rb[cab_idx][RBS].set() : rb[cab_idx][RBS].reset();

        // Управление КМ
        //controller[cab_idx]->setMainHandlePos(auto_control[cab_idx]->km_pos_ref);

        // Управление КрМ
        brake_crane[cab_idx]->setHandlePosition(auto_control[cab_idx]->krm_pos);

        // Управление КВТ
        loco_crane[cab_idx]->setHandlePosition(auto_control[cab_idx]->kvt_pos);

        // Управление прожектором
        //auto_control[cab_idx]->spotlight_ON ? spotlight_low_tumbler[cab_idx].set() :
        //    spotlight_low_tumbler[cab_idx].reset();

        sand_system->setSandDeliveryOn(auto_control[cab_idx]->sand_ON);
    }
}
