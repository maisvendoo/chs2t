#ifndef KM21KR2STATE_H
#define KM21KR2STATE_H

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
struct ControllerState
{
    bool k01 = false;
    bool k02 = false;

    bool k21 = false;
    bool k22 = false;
    bool k23 = false;
    bool k25 = false;

    bool k31 = false;
    bool k32 = false;
    bool k33 = false;

    ControllerState() = default;
};

#endif // KM21KR2STATE_H
