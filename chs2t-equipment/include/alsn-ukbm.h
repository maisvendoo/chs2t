#ifndef     ALSN_UKBM_H
#define     ALSN_UKBM_H

#include    <device.h>
#include    <solver-types.h>
#include    <trigger.h>

#include    <QObject>

#include    <array>
#include    <cstddef>

class       CfgReader;
class       Timer;

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class SafetyDevice : public Device
{
public:

    explicit SafetyDevice(QObject* parent = nullptr);

    virtual ~SafetyDevice() override;

    virtual void step(double t, double dt) override;

    /// Приём кода АЛСН
    void setAlsnCode(int code_alsn);

    /// Приём состояния РБ
    void setRBstate(bool state);

    /// Приём состояния РБС
    void setRBSstate(bool state);

    /// Приём скорости от скоростемера
    void setVelocity(double v);

    void setKeyEPK(bool key_epk);

    /// Выдача состояния цепи удерживающей катушки ЭПК
    bool getEPKstate() const;

    float getRedLamp() const;
    float getRedYellowLamp() const;
    float getYellowLamp() const;
    float getGreenLamp() const;
    float getWhiteLamp() const;

    /// Приём сигнала от переключателя маневрового режима
    void setShuntingModeState(bool is_shunting_mode);

private:

    virtual void preStep(state_vector_t& Y, double t) override;

    virtual void ode_system(const state_vector_t& Y, state_vector_t& dYdt, double t) override;

    virtual void load_config(CfgReader& cfg) override;

    void alsn_process(int code_alsn);

    void off_all_lamps();

    void lamp_on(std::size_t lamp_idx);

private:

    int code_alsn;

    int old_code_alsn;

    bool state_RB;

    bool state_RBS;

    bool state_EPK;

    double v_kmh;

    bool key_epk;

    Trigger is_red;

    std::array<float, 5> lamps;

    Trigger epk_state;

    Timer* safety_timer;

    bool is_shunting_mode;
};

#endif // ALSN_UKBM_H
