#include    <chs2t.h>

#include    <automatic-train-stop.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool CHS2T::initAutostopProgram(int cab_autostop_request)
{
    if (autoStartTimer->isStarted())
    {
        return false;
    }

    if (autoStopTimer->isStarted())
    {
        return false;
    }

    if ((cab_autostop_request != CAB1) && (cab_autostop_request != CAB2))
    {
        return false;
    }

    // Целевая кабина - запрошенная, но если в ней нет реверсивной рукоятки,
    // а в другой она есть, выключаем из кабины, где установлена рукоятка
    int cab = cab_autostop_request;

    if (!km21KR2[cab].isReversHandle() && km21KR2[(cab == CAB1) ? CAB2 : CAB1].isReversHandle())
    {
        cab = (cab == CAB1) ? CAB2 : CAB1;
    }

    // Останавливать нечего: в целевой кабине нет ни рукоятки, ни ключа панели
    if (!km21KR2[cab].isReversHandle() && !sw_panel[cab].isKey())
    {
        return false;
    }

    autostop_cab = cab;

    // Если было включено автоведение - выключаем его
    if (autopilot_switcher[autostop_cab].getState())
    {
        autopilot_switcher[autostop_cab].reset();
    }

    return true;
}