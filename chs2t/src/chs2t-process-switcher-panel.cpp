#include "chs2t.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::setSwitcherState(Switcher& sw, signal_t signal)
{
    if (signal.is_active)
    {
        int pos = signal.cur_value - 1;

        if (pos >= 0)
            sw.setInitPosition(pos);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::setTriggerState(Trigger& trig, signal_t signal)
{
    if (signal.is_active)
    {
        int pos = signal.cur_value - 1;

        if (pos >= 0)
        {
            if (static_cast<bool>(pos))
                trig.set();
            else
                trig.reset();
        }
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepSwitcherPanel(const simulator_time_t& t, const double& dt)
{
    // Верхняя половина приборной панели
    setTriggerState(epb_switch[CAB1], control_signals.analogSignal[SWP1_EPT]);
    setSwitcherState(mk_switcher[CAB1][0], control_signals.analogSignal[SWP1_MK1]);
    setSwitcherState(fastswitch_switcher[CAB1], control_signals.analogSignal[SWP1_BV]);
    setSwitcherState(pant_switcher[CAB1][0], control_signals.analogSignal[SWP1_TP1]);

    // Нижняя половина панели
    setSwitcherState(motor_fan_switcher[CAB1], control_signals.analogSignal[SWP2_MV]);
    setSwitcherState(pant_switcher[CAB1][1], control_signals.analogSignal[SWP1_TP2]);
    setSwitcherState(mk_switcher[CAB1][1], control_signals.analogSignal[SWP2_MK2]);
    setSwitcherState(blinds_switcher[CAB1], control_signals.analogSignal[SWP1_VK]);
}
