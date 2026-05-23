#ifndef     CHS2T_AUTOPILOT_TYPES_H
#define     CHS2T_AUTOPILOT_TYPES_H

#include    <autopilot-types.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class chs2t_control_t : public auto_control_t
{
public:

    int krm_pos = 1;

    double kvt_pos = 0.0;

    bool up_front_pant = false;

    bool is_EDB_ON = false;

    enum
    {
        KM_POS_AUTO_MINUS = -2,
        KM_POS_MINUS = -1,
        KM_POS_ZERO = 0,
        KM_POS_PLUS = 1,
        KM_POS_AUTO_PLUS = 2
    };

    int km_pos_ref = KM_POS_ZERO;

    chs2t_control_t() : auto_control_t()
    {

    }
};

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class chs2t_feedback_t : public auto_feedback_t
{
public:

    double I_motor = 0.0;

    int km_pos = 0;

    int pos = 0;

    bool is_allow_traction = false;

    bool is_EPB_on = false;

    bool is_front_pant_up = false;

    chs2t_feedback_t() : auto_feedback_t()
    {

    }
};

#endif
