#include "chs2t.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::keyProcess()
{
    if (getKeyState(KEY_9))
    {
        if (isShift())
            EDTSwitch.set();
        else
            EDTSwitch.reset();
    }

    if (getKeyState(KEY_V))
    {
        if (isShift())
            epb_switch.set();
        else
            epb_switch.reset();
    }

    // ЭПК
    if (getKeyState(KEY_N))
    {
        if (isShift())
            key_epk.set();
        else
            key_epk.reset();
    }

    state_RB = getKeyState(KEY_M);

    if (control_signals.analogSignal[CS_RBS].is_active)
    {
        state_RBS = static_cast<bool>(control_signals.analogSignal[CS_RBS].cur_value);
    }
    else
    {
        state_RBS = getKeyState(KEY_Z);
    }
}
