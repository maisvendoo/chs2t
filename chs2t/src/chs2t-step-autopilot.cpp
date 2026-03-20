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
    //int cab_idx = 0;
    // Индекс переднего по ходу движения токоприемника
    //int front_pant_idx = 0;

    if (km21KR2[CAB1].isReversHandle())
    {
        v_lim = speedmap_fwd->getCurrentLimit();
        v_lim_next = speedmap_fwd->getNextLimit();
        limit_dist = speedmap_fwd->getNextLimitDistance();
        alsn_code = coil_ALSN_fwd->getCode();
        signal_dist = coil_ALSN_fwd->getNextSignalDistance();

        cab_idx = CAB1;
        front_pant_idx = PANT1;
    }

    if (km21KR2[CAB2].isReversHandle())
    {
        v_lim = speedmap_bwd->getCurrentLimit();
        v_lim_next = speedmap_bwd->getNextLimit();
        limit_dist = speedmap_bwd->getNextLimitDistance();
        alsn_code = coil_ALSN_bwd->getCode();
        signal_dist = coil_ALSN_bwd->getNextSignalDistance();

        cab_idx = CAB2;
        front_pant_idx = PANT2;
    }

    if (autopilot[cab_idx] == nullptr)
    {
        return;
    }

    // Включение и выключение автоведения
    if (autopilot_switcher[cab_idx].getState())
    {
        autopilot[cab_idx]->on();
        pantCtrlTimer->start();
    }
    else
    {
        autopilot[cab_idx]->off();
        pantCtrlTimer->stop();
    }

    // Сигнал контроля бдительности от цепей прибора безопасности
    auto_feedback[cab_idx]->is_vigilance_control = safety_device[cab_idx]->getEPKstate();
    auto_feedback[cab_idx]->I_motor = motor->getI56();
    auto_feedback[cab_idx]->km_pos = km21KR2[cab_idx].getMainPos();
    auto_feedback[cab_idx]->pos = stepSwitch->getPoz();
    auto_feedback[cab_idx]->v_cur = qAbs(velocity * Physics::kmh);
    auto_feedback[cab_idx]->v_tau = qAbs(wheel_omega[0] * wheel_diameter[0] / 2.0 * Physics::kmh);
    auto_feedback[cab_idx]->v_lim = v_lim;
    auto_feedback[cab_idx]->v_lim_next = v_lim_next;
    auto_feedback[cab_idx]->limit_dist = limit_dist;
    auto_feedback[cab_idx]->alsn_code = alsn_code;
    auto_feedback[cab_idx]->signal_dist = signal_dist;
    auto_feedback[cab_idx]->pBC = brake_mech[TROLLEY_FWD]->getBCpressure();
    auto_feedback[cab_idx]->pEQ = brake_crane[cab_idx]->getERpressure();
    auto_feedback[cab_idx]->p_charge = charge_press;
    auto_feedback[cab_idx]->is_EPB_on = epb_control->stateReleaseLamp();
    auto_feedback[cab_idx]->is_front_pant_up = pantographs[front_pant_idx]->isUp();

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
        km21KR2[cab_idx].lockManualControl(true);
        km21KR2[cab_idx].setControlPos(auto_control[cab_idx]->km_pos_ref);

        // Управление КрМ
        brake_crane[cab_idx]->setHandlePosition(auto_control[cab_idx]->krm_pos);

        // Управление КВТ
        loco_crane[cab_idx]->setHandlePosition(auto_control[cab_idx]->kvt_pos);

        // Управление прожектором
        auto_control[cab_idx]->spotlight_ON ? sw_panel[cab_idx].getSwitcherPtr(CHS2tSwitchers::SPOTLIGHT)->setPosition(CHS2tSwitchers::SPOTLIGHT_HIGH) :
            sw_panel[cab_idx].getSwitcherPtr(CHS2tSwitchers::SPOTLIGHT)->setPosition(CHS2tSwitchers::SPOTLIGHT_OFF);

        sand_system->setSandDeliveryOn(auto_control[cab_idx]->sand_ON);

        if (auto_control[cab_idx]->is_EDB_ON)
        {
            sw_panel[cab_idx].getSwitcherPtr(CHS2tSwitchers::EDT)->setPosition(CHS2tSwitchers::EDT_ON);
        }
        else
        {
            sw_panel[cab_idx].getSwitcherPtr(CHS2tSwitchers::EDT)->setPosition(CHS2tSwitchers::EDT_OFF);
        }

        horn[cab_idx]->setSvistokOn(auto_control[cab_idx]->whistle);
        horn[cab_idx]->setTifonOn(auto_control[cab_idx]->typhoid);
    }
    else
    {
        km21KR2[cab_idx].lockManualControl(false);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::slotPantCtrl()
{
    if (autopilot[cab_idx] == nullptr)
    {
        return;
    }

    if (!autopilot[cab_idx]->isActive())
    {
        return;
    }

    if (auto_control[cab_idx]->up_front_pant)
    {
        UpPantograph(front_pant_idx, cab_idx);
    }
    else
    {
        DownPantograph(front_pant_idx, cab_idx);
    }
}
