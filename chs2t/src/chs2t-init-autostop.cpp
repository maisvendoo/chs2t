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

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::slotAutostop()
{
    // Возвращаем реверс в нейтраль
    if (km21KR2[autostop_cab].isReversHandle() && km21KR2[autostop_cab].getReversHandlePos() != 0)
    {
        km21KR2[autostop_cab].setReversHandlePos(0);
        return;
    }

    // Выключаем ЭПК
    if (epk[autostop_cab]->isKeyOn())
    {
        epk[autostop_cab]->setKeyOn(false);
        return;
    }

    // Жалюзи - открыто
    if (sw_panel[autostop_cab].getSwitcherPtr(CHS2tSwitchers::BLINDS)->getPosition() != CHS2tSwitchers::BLINDS_OPEN)
    {
        switcherController(sw_panel[autostop_cab].getSwitcherPtr(CHS2tSwitchers::BLINDS), CHS2tSwitchers::BLINDS_OPEN);
        return;
    }

    // Выключаем мотор-вентиляторы
    if (sw_panel[autostop_cab].getSwitcherPtr(CHS2tSwitchers::FANS)->getPosition() != CHS2tSwitchers::FANS_OFF)
    {
        switcherController(sw_panel[autostop_cab].getSwitcherPtr(CHS2tSwitchers::FANS), CHS2tSwitchers::FANS_OFF);
        return;
    }

    // Выключаем компрессор 2
    if (sw_panel[autostop_cab].getSwitcherPtr(CHS2tSwitchers::COMPR_2)->getPosition() != CHS2tSwitchers::COMPR_OFF)
    {
        switcherController(sw_panel[autostop_cab].getSwitcherPtr(CHS2tSwitchers::COMPR_2), CHS2tSwitchers::COMPR_OFF);
        return;
    }

    // Выключаем компрессор 1
    if (sw_panel[autostop_cab].getSwitcherPtr(CHS2tSwitchers::COMPR_1)->getPosition() != CHS2tSwitchers::COMPR_OFF)
    {
        switcherController(sw_panel[autostop_cab].getSwitcherPtr(CHS2tSwitchers::COMPR_1), CHS2tSwitchers::COMPR_OFF);
        return;
    }

    // Выключаем БВ: переключатели ГВ обеих кабин в положение OFF
    for (auto cab_idx : {CAB1, CAB2})
    {
        if (sw_panel[cab_idx].getSwitcherPtr(CHS2tSwitchers::FAST_SW)->getPosition() != CHS2tSwitchers::FAST_SW_OFF)
        {
            switcherController(sw_panel[cab_idx].getSwitcherPtr(CHS2tSwitchers::FAST_SW), CHS2tSwitchers::FAST_SW_OFF);
            return;
        }
    }

    // Опускаем передний токоприемник (переключатель "вперед")
    if (sw_panel[autostop_cab].getSwitcherPtr(CHS2tSwitchers::PANT_FWD)->getPosition() != CHS2tSwitchers::PANT_DOWN)
    {
        switcherController(sw_panel[autostop_cab].getSwitcherPtr(CHS2tSwitchers::PANT_FWD), CHS2tSwitchers::PANT_DOWN);
        return;
    }

    lock_pant_sw[(autostop_cab == CAB1) ? PANT1 : PANT2] = false;

    // Опускаем задний токоприемник (переключатель "назад")
    if (sw_panel[autostop_cab].getSwitcherPtr(CHS2tSwitchers::PANT_BWD)->getPosition() != CHS2tSwitchers::PANT_DOWN)
    {
        switcherController(sw_panel[autostop_cab].getSwitcherPtr(CHS2tSwitchers::PANT_BWD), CHS2tSwitchers::PANT_DOWN);
        return;
    }

    lock_pant_sw[(autostop_cab == CAB1) ? PANT2 : PANT1] = false;

    // Выключаем ключ панели
    if (sw_panel[autostop_cab].isKeyOn())
    {
        sw_panel[autostop_cab].setKeyOn(false);
        return;
    }

    // Возвращаем управление клавиатуре
    sw_panel[CAB1].setControl(&pressed_keys_by_cabine[CAB1]);
    sw_panel[CAB2].setControl(&pressed_keys_by_cabine[CAB2]);
    km21KR2[CAB1].setControl(&pressed_keys_by_cabine[CAB1]);
    km21KR2[CAB2].setControl(&pressed_keys_by_cabine[CAB2]);
    epk[CAB1]->setControl(&pressed_keys_by_cabine[CAB1]);
    epk[CAB2]->setControl(&pressed_keys_by_cabine[CAB2]);

    autoStopTimer->stop();
}