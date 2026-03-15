#ifndef     CHS2T_AUTOPILOT_H
#define     CHS2T_AUTOPILOT_H

#include    <autopilot.h>
#include    <chs2t-autopilot-types.h>
#include    <timer.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class CHS2TAutopilot : public Autopilot
{
public:

    CHS2TAutopilot();

    ~CHS2TAutopilot();

    auto_control_t *getControl() override;

    void step(double t, double dt) override;

    void initAutoBrakeControl(const QString& config_name,
                              const QString& custom_cfg_dir) override;

private:

    double Imax = 0.0;

    double delta_I = 0.0;

    double Kp = 1.0;

    double Ki = 0.0;

    double Ks = 0.0;

    double dv = 0.0;

    /// Тормозной контроллер
    AutopilotBrakeController *brake_control = new AutopilotBrakeController;

    /// Блокирование тяги тормозами
    bool lock_traction = false;

    /// Структура управляющих воздействий ВСЕГДА специфична
    chs2t_control_t *auto_control = new chs2t_control_t();

    /// Структура обратной связи, приводимая к нашей от общей
    chs2t_feedback_t *auto_feedback = nullptr;

    void preStep(state_vector_t &Y, double t) override;

    void ode_system(const state_vector_t &Y,
                    state_vector_t &dYdt,
                    double t) override;

    void load_config(CfgReader &cfg) override;

    const double KM_POS_DELAY = 0.5;

    Timer *km_pos_timer = new Timer(KM_POS_DELAY, false);

    void plusPos();

    void minusPos();

    void setPosKM(int &km_pos, int km_pos_ref);

private slots:

    void slotPosDelay();
};

#endif
