#include    <chs2t.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool CHS2T::initAutostartProgram(int cab_autostart_request)
{
    if (autoStartTimer->isStarted())
    {
        return false;
    }

    if ((cab_autostart_request != CAB1) && (cab_autostart_request != CAB2))
    {
        return false;
    }

    if (km21KR2[(cab_autostart_request == CAB1) ? CAB2 : CAB1].isReversHandle())
    {
        return false;
    }

    if (sw_panel[(cab_autostart_request == CAB1) ? CAB2 : CAB1].isKey())
    {
        return false;
    }

    if (!epk[cab_autostart_request]->isKeyAllowed())
    {
        return false;
    }

    autostart_cab = cab_autostart_request;
    km21KR2[autostart_cab].insertReversHandle(true);
    sw_panel[autostart_cab].insertKey(true);
    epk[autostart_cab]->insertKey(true);

    km21KR2[CAB1].setControl();
    km21KR2[CAB2].setControl();
    epk[CAB1]->setControl();
    epk[CAB2]->setControl();

    return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::switcherController(SwitcherControl *sw, uint16_t ref_pos)
{
    uint16_t cur_pos = sw->getPosition();

    if (cur_pos < ref_pos)
    {
        sw->incPos();
    }

    if (cur_pos > ref_pos)
    {
        sw->decPos();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::slotAutostart()
{
   // Разблокируем панель кулачковых переключателей
    if (!sw_panel[autostart_cab].isKeyOn())
    {
        sw_panel[autostart_cab].setKeyOn(true);
        return;
    }

    // Поднимаем задние рога
    if (UpPantograph(PANT2, autostart_cab))
    {
        return;
    }

    // Поднимаем передние рога
    if (UpPantograph(PANT1, autostart_cab))
    {
        return;
    }

    // Включаем БВ
    if (!bv->getState())
    {
        if (sw_panel[autostart_cab].getSwitcherPtr(CHS2tSwitchers::FAST_SW)->getPosition() != CHS2tSwitchers::FAST_SW_ON)
        {
            switcherController(sw_panel[autostart_cab].getSwitcherPtr(CHS2tSwitchers::FAST_SW), CHS2tSwitchers::FAST_SW_ON);
        }

        return;
    }
    else
    {
        switcherController(sw_panel[autostart_cab].getSwitcherPtr(CHS2tSwitchers::FAST_SW), CHS2tSwitchers::FAST_SW_WORK);
    }

    // Включаем компрессор 1
    if (sw_panel[autostart_cab].getSwitcherPtr(CHS2tSwitchers::COMPR_1)->getPosition() != CHS2tSwitchers::COMPR_AUTO)
    {
        switcherController(sw_panel[autostart_cab].getSwitcherPtr(CHS2tSwitchers::COMPR_1), CHS2tSwitchers::COMPR_AUTO);

        return;
    }

    // Включаем компрессор 2
    if (sw_panel[autostart_cab].getSwitcherPtr(CHS2tSwitchers::COMPR_2)->getPosition() != CHS2tSwitchers::COMPR_AUTO)
    {
        switcherController(sw_panel[autostart_cab].getSwitcherPtr(CHS2tSwitchers::COMPR_2), CHS2tSwitchers::COMPR_AUTO);

        return;
    }

    // Включаем мотор-вентиляторы
    if (sw_panel[autostart_cab].getSwitcherPtr(CHS2tSwitchers::FANS)->getPosition() != CHS2tSwitchers::FANS_AUTO)
    {
        switcherController(sw_panel[autostart_cab].getSwitcherPtr(CHS2tSwitchers::FANS), CHS2tSwitchers::FANS_AUTO);

        return;
    }

    // Жалюзи - автомат
    if (sw_panel[autostart_cab].getSwitcherPtr(CHS2tSwitchers::BLINDS)->getPosition() != CHS2tSwitchers::AUTO_BLINDS)
    {
        switcherController(sw_panel[autostart_cab].getSwitcherPtr(CHS2tSwitchers::BLINDS), CHS2tSwitchers::AUTO_BLINDS);

        return;
    }

    // Включаем ЭПК
    if (!epk[autostart_cab]->isKeyOn())
    {
        epk[autostart_cab]->setKeyOn(true);
        return;
    }

    // Проверка бдительности
    if (!rb[autostart_cab][RBS].getState())
    {
        rb[autostart_cab][RBS].set();
        return;
    }

    rb[autostart_cab][RBS].reset();

    // Переводим реверс вперед
    km21KR2[autostart_cab].setReversHandlePos(1);

    sw_panel[CAB1].setControl(&pressed_keys_by_cabine[CAB1]);
    sw_panel[CAB2].setControl(&pressed_keys_by_cabine[CAB2]);
    km21KR2[CAB1].setControl(&pressed_keys_by_cabine[CAB1]);
    km21KR2[CAB2].setControl(&pressed_keys_by_cabine[CAB2]);
    epk[CAB1]->setControl(&pressed_keys_by_cabine[CAB1]);
    epk[CAB2]->setControl(&pressed_keys_by_cabine[CAB2]);

    autoStartTimer->stop();

    if (auto_start_autopilot)
    {
        autopilot_switcher[autostart_cab].set();        
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool CHS2T::UpPantograph(int pant_idx, int cab_idx)
{
    int sw_idx = CHS2tSwitchers::PANT_BWD;

    pant_idx == PANT1 ? sw_idx = CHS2tSwitchers::PANT_FWD : sw_idx = CHS2tSwitchers::PANT_BWD;

    if (!pantographs[pant_idx]->isUp())
    {
        if (sw_panel[cab_idx].getSwitcherPtr(sw_idx)->getPosition() != CHS2tSwitchers::PANT_ON)
        {
            if (!lock_pant_sw[pant_idx])
                switcherController(sw_panel[cab_idx].getSwitcherPtr(sw_idx), CHS2tSwitchers::PANT_ON);
        }
        else
        {
            switcherController(sw_panel[cab_idx].getSwitcherPtr(sw_idx), CHS2tSwitchers::PANT_UP);
            lock_pant_sw[pant_idx] = true;
        }

        return true;
    }

    return false;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool CHS2T::DownPantograph(int pant_idx, int cab_idx)
{
    int sw_idx = CHS2tSwitchers::PANT_BWD;

    pant_idx == PANT1 ? sw_idx = CHS2tSwitchers::PANT_FWD : sw_idx = CHS2tSwitchers::PANT_BWD;

    if (!pantographs[pant_idx]->isDown())
    {
        switcherController(sw_panel[cab_idx].getSwitcherPtr(sw_idx), CHS2tSwitchers::PANT_DOWN);
        return true;
    }
    else
    {
        lock_pant_sw[pant_idx] = false;
    }

    return false;
}
