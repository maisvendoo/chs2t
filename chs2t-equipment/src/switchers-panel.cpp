#include    "switchers-panel.h"
#include    "hardware-signals.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CHS2tSwitchers::CHS2tSwitchers(QObject *parent) : Device(parent)
{
    initControl();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CHS2tSwitchers::~CHS2tSwitchers()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2tSwitchers::setControl(std::set<std::uint16_t> *keys, control_signals_t *control_signals)
{
    Device::setControl(keys, control_signals);
    for (auto& switcher : switchers)
    {
        switcher.setControl(pressed_keys);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2tSwitchers::allowKey(bool allow)
{
    is_key_allowed = allow;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool CHS2tSwitchers::isKeyAllowed() const
{
    return is_key_allowed;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2tSwitchers::insertKey(bool insert)
{
    insert = insert && is_key_allowed;

    if (insert)
    {
        is_key.set();
    }
    else
    {
        if (!isKeyOn())
        {
            is_key.reset();
        }
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool CHS2tSwitchers::isKey() const
{
    return is_key.getState();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2tSwitchers::setKeyOn(bool state)
{
    if (state)
    {
        if (isKey())
        {
            key_state.set();
        }
    }
    else
    {
        if (isSwitchersOff())
        {
            key_state.reset();
        }
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool CHS2tSwitchers::isKeyOn() const
{
    return key_state.getState();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool CHS2tSwitchers::setSwitcherInitPosition(size_t switcher_idx, std::uint16_t position)
{
    return switchers[switcher_idx].setInitPosition(position);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool CHS2tSwitchers::setSwitcherPosition(size_t switcher_idx, std::uint16_t position)
{
    return switchers[switcher_idx].setPosition(position);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
std::uint16_t CHS2tSwitchers::getSwitcherNumPositions(size_t switcher_idx) const
{
    return switchers[switcher_idx].getNumPositions();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
std::uint16_t CHS2tSwitchers::getSwitcherPosition(size_t switcher_idx) const
{
    return switchers[switcher_idx].getPosition();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float CHS2tSwitchers::getSwitcherHandlePosition(size_t switcher_idx) const
{
    return switchers[switcher_idx].getHandlePosition();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool CHS2tSwitchers::isSwitched(size_t switcher_idx, std::uint16_t pos) const
{
    return switchers[switcher_idx].isSwitched(pos);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float CHS2tSwitchers::getKeyInsertSoundSignal(size_t idx) const
{
    return is_key.getSoundSignal(idx);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float CHS2tSwitchers::getKeyTurnSoundSignal(size_t idx) const
{
    return key_state.getSoundSignal(idx);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float CHS2tSwitchers::getSoundSignal(size_t switcher_idx) const
{
    return switchers[switcher_idx].getSoundSignal();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2tSwitchers::ode_system(const state_vector_t &Y,
                                   state_vector_t &dYdt,
                                   double t)
{
    (void) Y;
    (void) dYdt;
    (void) t;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2tSwitchers::step(double t, double dt)
{
    if (pressed_keys && getKeyState(*pressed_keys, key_symbol))
    {
        // Управляем новым нажатием на клавишу
        if (!prev_key)
        {
            prev_key = true; // Запоминаем, что клавиша нажата

            // Alt - вставляем/извлекаем ключ
            if (isModifier(*pressed_keys, MODIFIER_Alt))
            {
                insertKey(!isKey());
                return;
            }

            // Ctrl - отключаем ключ
            if (isModifier(*pressed_keys, MODIFIER_Control))
            {
                setKeyOn(false);
                return;
            }

            // Shift - включаем ключ
            if (isModifier(*pressed_keys, MODIFIER_Shift))
            {
                setKeyOn(true);
                return;
            }
        }
    }
    else
    {
        prev_key = false; // Запоминаем, что клавиша отпущена
    }

    for (const auto& idx : locked_idx)
    {
        if (isKeyOn())
        {
            // При разблокированной панели управляем переключателями
            switchers[idx].step(t, dt);
        }
        else
        {
            // При заблокированной панели переключатели в нулевой позиции
            switchers[idx].setPosition(0);
        }
    }

    for (const auto& idx : no_locked_idx)
    {
        // Управляем переключателями
        switchers[idx].step(t, dt);
    }

    if (control_signals)
    {
        auto setExternalSwitcher = [](Switcher& sw, signal_t& signal)
        {
            if (signal.is_active)
            {
                sw.setInitPosition(std::uint16_t(signal.cur_value - 1));
            }
        };
        // Верхняя половина приборной панели
        setExternalSwitcher(switchers[EPB], control_signals->analogSignal[SWP1_EPT]);
        setExternalSwitcher(switchers[COMPR_1], control_signals->analogSignal[SWP1_MK1]);
        setExternalSwitcher(switchers[PANT_FWD], control_signals->analogSignal[SWP1_TP1]);
        setExternalSwitcher(switchers[FAST_SW], control_signals->analogSignal[SWP1_BV]);
        // Нижняя половина панели
        setExternalSwitcher(switchers[FANS], control_signals->analogSignal[SWP2_MV]);
        setExternalSwitcher(switchers[COMPR_2], control_signals->analogSignal[SWP2_MK2]);
        setExternalSwitcher(switchers[PANT_BWD], control_signals->analogSignal[SWP1_TP2]);
        setExternalSwitcher(switchers[BLINDS], control_signals->analogSignal[SWP1_VK]);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2tSwitchers::initControl()
{
    key_symbol = KEY_Y;

    switchers[EPB].setNumPositions(EPB_NUM);
    switchers[EPB].setKeySymbolIncrease(KEY_V);
    switchers[EPB].setKeyModifierIncrease(MODIFIER_OnlyShift);
    switchers[EPB].setKeySymbolDecrease(KEY_V);
    switchers[EPB].setKeyModifierDecrease(MODIFIER_OnlyControl);
    switchers[EPB].setInitPosition(EPB_OFF);

    switchers[COMPR_1].setNumPositions(COMPR_NUM);
    switchers[COMPR_1].setKeySymbolIncrease(KEY_2);
    switchers[COMPR_1].setKeyModifierIncrease(MODIFIER_OnlyShift);
    switchers[COMPR_1].setKeySymbolDecrease(KEY_2);
    switchers[COMPR_1].setKeyModifierDecrease(MODIFIER_OnlyControl);
    switchers[COMPR_1].setInitPosition(COMPR_HEAT);

    switchers[PANT_FWD].setNumPositions(PANT_NUM);
    switchers[PANT_FWD].setKeySymbolIncrease(KEY_I);
    switchers[PANT_FWD].setKeyModifierIncrease(MODIFIER_OnlyShift);
    switchers[PANT_FWD].setKeySymbolDecrease(KEY_I);
    switchers[PANT_FWD].setKeyModifierDecrease(MODIFIER_OnlyControl);
    switchers[PANT_FWD].setInitPosition(PANT_OFF);

    switchers[FAST_SW].setNumPositions(FAST_SW_NUM);
    switchers[FAST_SW].setKeySymbolIncrease(KEY_P);
    switchers[FAST_SW].setKeyModifierIncrease(MODIFIER_OnlyShift);
    switchers[FAST_SW].setKeySymbolDecrease(KEY_P);
    switchers[FAST_SW].setKeyModifierDecrease(MODIFIER_OnlyControl);
    switchers[FAST_SW].setInitPosition(FAST_SW_NO_CTRL);

    switchers[FANS].setNumPositions(FANS_NUM);
    switchers[FANS].setKeySymbolIncrease(KEY_1);
    switchers[FANS].setKeyModifierIncrease(MODIFIER_OnlyShift);
    switchers[FANS].setKeySymbolDecrease(KEY_1);
    switchers[FANS].setKeyModifierDecrease(MODIFIER_OnlyControl);
    switchers[FANS].setInitPosition(FANS_NO_CTRL);

    switchers[COMPR_2].setNumPositions(COMPR_NUM);
    switchers[COMPR_2].setKeySymbolIncrease(KEY_3);
    switchers[COMPR_2].setKeyModifierIncrease(MODIFIER_OnlyShift);
    switchers[COMPR_2].setKeySymbolDecrease(KEY_3);
    switchers[COMPR_2].setKeyModifierDecrease(MODIFIER_OnlyControl);
    switchers[COMPR_2].setInitPosition(COMPR_HEAT);

    switchers[PANT_BWD].setNumPositions(PANT_NUM);
    switchers[PANT_BWD].setKeySymbolIncrease(KEY_O);
    switchers[PANT_BWD].setKeyModifierIncrease(MODIFIER_OnlyShift);
    switchers[PANT_BWD].setKeySymbolDecrease(KEY_O);
    switchers[PANT_BWD].setKeyModifierDecrease(MODIFIER_OnlyControl);
    switchers[PANT_BWD].setInitPosition(PANT_OFF);

    switchers[BLINDS].setNumPositions(BLINDS_NUM);
    switchers[BLINDS].setKeySymbolIncrease(KEY_4);
    switchers[BLINDS].setKeyModifierIncrease(MODIFIER_OnlyShift);
    switchers[BLINDS].setKeySymbolDecrease(KEY_4);
    switchers[BLINDS].setKeyModifierDecrease(MODIFIER_OnlyControl);
    switchers[BLINDS].setInitPosition(AUXCOMPR);

    switchers[CAB_LIGHT].setNumPositions(CAB_LIGHT_NUM);
    switchers[CAB_LIGHT].setKeySymbolIncrease(KEY_K);
    switchers[CAB_LIGHT].setKeyModifierIncrease(MODIFIER_OnlyShift);
    switchers[CAB_LIGHT].setKeySymbolDecrease(KEY_K);
    switchers[CAB_LIGHT].setKeyModifierDecrease(MODIFIER_OnlyControl);
    switchers[CAB_LIGHT].setInitPosition(CAB_LIGHT_OFF);

    switchers[BUFLIGHT_L].setNumPositions(BUFLIGHT_NUM);
    switchers[BUFLIGHT_L].setKeySymbolIncrease(KEY_G);
    switchers[BUFLIGHT_L].setKeyModifierIncrease(MODIFIER_OnlyShift);
    switchers[BUFLIGHT_L].setKeySymbolDecrease(KEY_G);
    switchers[BUFLIGHT_L].setKeyModifierDecrease(MODIFIER_OnlyControl);
    switchers[BUFLIGHT_L].setInitPosition(BUFLIGHT_OFF);

    switchers[BUFLIGHT_R].setNumPositions(BUFLIGHT_NUM);
    switchers[BUFLIGHT_R].setKeySymbolIncrease(KEY_J);
    switchers[BUFLIGHT_R].setKeyModifierIncrease(MODIFIER_OnlyShift);
    switchers[BUFLIGHT_R].setKeySymbolDecrease(KEY_J);
    switchers[BUFLIGHT_R].setKeyModifierDecrease(MODIFIER_OnlyControl);
    switchers[BUFLIGHT_R].setInitPosition(BUFLIGHT_OFF);

    switchers[SPOTLIGHT].setNumPositions(SPOTLIGHT_NUM);
    switchers[SPOTLIGHT].setKeySymbolIncrease(KEY_H);
    switchers[SPOTLIGHT].setKeyModifierIncrease(MODIFIER_OnlyShift);
    switchers[SPOTLIGHT].setKeySymbolDecrease(KEY_H);
    switchers[SPOTLIGHT].setKeyModifierDecrease(MODIFIER_OnlyControl);
    switchers[SPOTLIGHT].setInitPosition(SPOTLIGHT_OFF);

    switchers[EDT].setNumPositions(EDT_NUM);
    switchers[EDT].setKeySymbolIncrease(KEY_F);
    switchers[EDT].setKeyModifierIncrease(MODIFIER_OnlyShift);
    switchers[EDT].setKeySymbolDecrease(KEY_F);
    switchers[EDT].setKeyModifierDecrease(MODIFIER_OnlyControl);
    switchers[EDT].setInitPosition(EDT_ON);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool CHS2tSwitchers::isSwitchersOff() const
{
    for (const auto& idx : locked_idx)
    {
        if (switchers[idx].getPosition() != 0)
        {
            return false;
        }
    }

    return true;
}
