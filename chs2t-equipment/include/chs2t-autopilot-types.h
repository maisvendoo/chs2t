#ifndef     CHS2T_AUTOPILOT_TYPES_H
#define     CHS2T_AUTOPILOT_TYPES_H

#include    <autopilot-types.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class chs2t_control_t : public auto_control_t
{
public:

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

    chs2t_feedback_t() : auto_feedback_t()
    {

    }
};

#endif
