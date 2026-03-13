#include    <chs2t-autopilot.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CHS2TAutopilot::CHS2TAutopilot() : Autopilot(nullptr)
{

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

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2TAutopilot::ode_system(const state_vector_t &Y,
                                state_vector_t &dYdt,
                                double t)
{

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

GET_AUTOPILOT(CHS2TAutopilot)
