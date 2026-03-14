#include    <chs2t-autopilot.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CHS2TAutopilot::CHS2TAutopilot() : Autopilot(nullptr)
{
    connect(km_pos_timer, &Timer::process, this, &CHS2TAutopilot::slotPosDelay);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CHS2TAutopilot::~CHS2TAutopilot()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
auto_control_t *CHS2TAutopilot::getControl()
{
    return auto_control;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2TAutopilot::step(double t, double dt)
{
    brake_control->step(t, dt);
    km_pos_timer->step(t, dt);
    Autopilot::step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2TAutopilot::initAutoBrakeControl(const QString &config_name,
                                          const QString &custom_cfg_dir)
{
    brake_control->read_config(config_name, custom_cfg_dir);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2TAutopilot::preStep(state_vector_t &Y, double t)
{
    auto_feedback = dynamic_cast<chs2t_feedback_t *>(feedback);

    if (auto_feedback == nullptr)
    {
        return;
    }

    // Режем сигнал интегратора
    Y[0] = cut(Y[0], -1.0, 1.0);

    // Ошибка по скорости
    dv = v_ref - auto_feedback->v_cur;

    // Вычисляем задание по току ТЭД
    double kp = Kp * train_mass / ref_mass;

    double dv_s = pf(feedback->v_tau - feedback->v_cur);

    double I_ref = Imax * (kp * dv - Ks * dv_s + getY(0));

    I_ref = cut(I_ref, 0.0, Imax);

    // Блокирование тяги по давлению в ТЦ
    if (auto_feedback->pBC > 0.04)
    {
        lock_traction = true;
    }
    else
    {
        // Если тяга заблокирована но скорость не упала сильно
        if (lock_traction && dv < 5.0)
            lock_traction = true; // продолжаем блокировать тягу
        else
            lock_traction = false;
    }

    // Если ток упал ниже уставки
    if (auto_feedback->I_motor < I_ref - delta_I)
    {
        if (!lock_traction)
        {
            // + позиция
            plusPos();
        }
    }

    // Если ток сильно выше уставки
    if (auto_feedback->I_motor > I_ref + delta_I)
    {
        // - позиция
        minusPos();
    }

    // Если превышаем скорость - мотаем вниз до упора
    if (dv < -dV_traction_off)
    {
        if (auto_feedback->pos != 0)
            setPosKM(auto_control->km_pos_ref, chs2t_control_t::KM_POS_AUTO_MINUS);
        else
            setPosKM(auto_control->km_pos_ref, chs2t_control_t::KM_POS_ZERO);
    }

    brake_control->setBrakePressures(auto_feedback->pEQ,
                                     auto_feedback->pBC,
                                     auto_feedback->p_charge);

    brake_control->setFeedback(auto_feedback->v_cur, dist_target, a_brake, accel_meter->value());

    brake_control->step_control(auto_feedback->is_EPB_on,
                                dv,
                                is_motion_allowed,
                                lock_traction,
                                is_disable_release);

    autopilot_brake_control_state_t bc_state = brake_control->getControlState();

    auto_control->krm_pos = bc_state.brake_crane_pos_ref;
    auto_control->kvt_pos = bc_state.loco_crane_pos_ref;

    // Управляем прожектором - включаем когда разрешено движение
    auto_control->spotlight_ON = is_motion_allowed;

    auto_control->press_RB = auto_feedback->is_vigilance_control;

    if (auto_feedback->v_cur > 10.0)
    {
        auto_control->up_front_pant = false;
    }

    if (auto_feedback->v_cur <= 5.0)
    {
        auto_control->up_front_pant = true;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2TAutopilot::ode_system(const state_vector_t &Y,
                                state_vector_t &dYdt,
                                double t)
{
    dYdt[0] = Ki * dv;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2TAutopilot::load_config(CfgReader &cfg)
{
    Autopilot::load_config(cfg);

    QString secName = "Device";

    cfg.getDouble(secName, "Imax", Imax);
    cfg.getDouble(secName, "DeltaI", delta_I);
    cfg.getDouble(secName, "Kp", Kp);
    cfg.getDouble(secName, "Ki", Ki);
    cfg.getDouble(secName, "Ks", Ks);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2TAutopilot::plusPos()
{
    if (lock_traction)
    {
        setPosKM(auto_control->km_pos_ref, chs2t_control_t::KM_POS_ZERO);
        return;
    }

    if (auto_feedback->pos == 42)
    {
        setPosKM(auto_control->km_pos_ref, chs2t_control_t::KM_POS_ZERO);
        return;
    }

    if (!km_pos_timer->isStarted())
    {
        if (auto_control->km_pos_ref == 0)
        {
            setPosKM(auto_control->km_pos_ref, chs2t_control_t::KM_POS_PLUS);
            km_pos_timer->start();
        }
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2TAutopilot::minusPos()
{
    if (!km_pos_timer->isStarted())
    {
        if (auto_control->km_pos_ref == 0)
        {
            setPosKM(auto_control->km_pos_ref, chs2t_control_t::KM_POS_MINUS);
            km_pos_timer->start();
        }
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2TAutopilot::setPosKM(int &km_pos, int km_pos_ref)
{
    if (km_pos != km_pos_ref)
    {
        km_pos = km_pos_ref;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2TAutopilot::slotPosDelay()
{
    if (auto_control->km_pos_ref == 0)
    {
        km_pos_timer->stop();
    }

    auto_control->km_pos_ref = chs2t_control_t::KM_POS_ZERO;
}

GET_AUTOPILOT(CHS2TAutopilot)
