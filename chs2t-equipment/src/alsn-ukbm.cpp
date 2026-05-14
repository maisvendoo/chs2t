#include    "alsn-ukbm.h"

#include    <device.h>
#include    <physics.h>
#include    <solver-types.h>
#include    <timer.h>

#include    <QObject>

#include    <algorithm>
#include    <cstddef>

enum
{
    RED_LAMP,
    RED_YELLOW_LAMP,
    YELLOW_LAMP,
    GREEN_LAMP,
    WHITE_LAMP
};

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
SafetyDevice::SafetyDevice(QObject* parent)
    : Device{parent}
    , code_alsn{1}
    , old_code_alsn{1}
    , state_RB{false}
    , state_RBS{false}
    , state_EPK{false}
    , v_kmh{0.0}
    , key_epk{false}
    , is_shunting_mode{false}
{
    epk_state.reset();

    safety_timer = new Timer{45.0, false, this};
    connect(safety_timer, &Timer::process, [this]() -> void {
        epk_state.reset();
    });
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
SafetyDevice::~SafetyDevice() = default;

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void SafetyDevice::step(double t, double dt)
{
    safety_timer->step(t, dt);
    Device::step(t, dt);
}

//------------------------------------------------------------------------------
// Приём кода АЛСН
//------------------------------------------------------------------------------
void SafetyDevice::setAlsnCode(int code_alsn)
{
    old_code_alsn = this->code_alsn;
    this->code_alsn = code_alsn;
}

//------------------------------------------------------------------------------
// Приём состояния РБ
//------------------------------------------------------------------------------
void SafetyDevice::setRBstate(bool state)
{
    state_RB = state;
}

//------------------------------------------------------------------------------
// Приём состояния РБС
//------------------------------------------------------------------------------
void SafetyDevice::setRBSstate(bool state)
{
    state_RBS = state;
}

//------------------------------------------------------------------------------
// Приём скорости от скоростемера
//------------------------------------------------------------------------------
void SafetyDevice::setVelocity(double v)
{
    v_kmh = v * Physics::kmh;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void SafetyDevice::setKeyEPK(bool key_epk)
{
    this->key_epk = key_epk;
}

//------------------------------------------------------------------------------
// Выдача состояния цепи удерживающей катушки ЭПК
//------------------------------------------------------------------------------
bool SafetyDevice::getEPKstate() const
{
    return epk_state.getState();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float SafetyDevice::getRedLamp() const
{
    return lamps[RED_LAMP];
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float SafetyDevice::getRedYellowLamp() const
{
    return lamps[RED_YELLOW_LAMP];
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float SafetyDevice::getYellowLamp() const
{
    return lamps[YELLOW_LAMP];
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float SafetyDevice::getGreenLamp() const
{
    return lamps[GREEN_LAMP];
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float SafetyDevice::getWhiteLamp() const
{
    return lamps[WHITE_LAMP];
}

//------------------------------------------------------------------------------
// Приём сигнала от переключателя маневрового режима
//------------------------------------------------------------------------------
void SafetyDevice::setShuntingModeState(bool is_shunting_mode)
{
    this->is_shunting_mode = is_shunting_mode;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void SafetyDevice::preStep(state_vector_t& Y, double t)
{
    (void)Y;
    (void)t;

    // Ничего не делаем при выключенном ЭПК
    if (!key_epk)
    {
        off_all_lamps();
        is_red.reset();
        return;
    }

    if (is_red.getState() && v_kmh > 20.0)
    {
        return;
    }

    if (code_alsn < old_code_alsn)
    {
        epk_state.reset();
    }

    // Отрезаем сигнал с дешифратора АЛСН при маневровом режиме
    if (is_shunting_mode)
    {
        code_alsn = 0;
    }

    if (code_alsn == 0)
    {
        if (old_code_alsn == 1)
        {
            is_red.set();
            epk_state.reset();
            lamp_on(RED_LAMP);

            if (v_kmh > 20.0)
            {
                return;
            }
        }
        else
        {
            if (!is_red.getState())
            {
                lamp_on(WHITE_LAMP);
            }
        }
    }

    alsn_process(code_alsn);

    if (code_alsn == 0)
    {
        // Отключено, до выяснения реальной логики работы
        // if (v_kmh > 40.0)
        // {
        //     epk_state.reset();
        //     return;
        // }

        if ((!safety_timer->isStarted()) && (v_kmh > 5))
        {
            safety_timer->start();
        }
        else
        {
            safety_timer->stop();
        }
    }
    else if (code_alsn == 1)
    {
        if (v_kmh > 60.0)
        {
            epk_state.reset();
            return;
        }

        if ((!safety_timer->isStarted()) && (v_kmh > 5))
        {
            safety_timer->start();
        }
        else
        {
            safety_timer->stop();
        }
    }
    else if (code_alsn == 2)
    {
        if (v_kmh > 60.0)
        {
            if (!safety_timer->isStarted())
            {
                safety_timer->start();
            }
        }
        else
        {
            safety_timer->stop();
        }
    }

    if (state_RB || state_RBS)
    {
        epk_state.set();
        safety_timer->stop();
    }

    if (state_RBS)
    {
        // Если отбиваем красный, то включаем белый
        if (is_red.getState())
        {
            is_red.reset();
        }

        epk_state.set();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void SafetyDevice::ode_system(
    const state_vector_t& Y,
    state_vector_t& dYdt,
    double t
)
{
    (void)Y;
    (void)dYdt;
    (void)t;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void SafetyDevice::load_config(CfgReader& cfg)
{
    (void)cfg;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void SafetyDevice::alsn_process(int code_alsn)
{
    switch (code_alsn)
    {
        case 1:
        {
            lamp_on(RED_YELLOW_LAMP);
            return;
        }
        case 2:
        {
            lamp_on(YELLOW_LAMP);
            return;
        }
        case 3:
        {
            lamp_on(GREEN_LAMP);
            return;
        }
        default:
        {
            return;
        }
    }

    // TODO: Replace on something like this?
    // if (code_alsn >= RED_YELLOW_LAMP && code_alsn <= GREEN_LAMP)
    // {
    //     lamp_on(static_cast<std::size_t>(code_alsn));
    // }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void SafetyDevice::off_all_lamps()
{
    // Очищаем состояние ламп
    std::fill(lamps.begin(), lamps.end(), 0.0f);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void SafetyDevice::lamp_on(std::size_t lamp_idx)
{
    off_all_lamps();
    lamps[lamp_idx] = 1.0f;
}
