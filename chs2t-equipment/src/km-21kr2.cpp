#include "km-21kr2.h"



//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
Km21KR2::Km21KR2(QObject* parent) : Device(parent)
{
    y.resize(1);
}

//------------------------------------------------------------------------------
// Деструктор
//------------------------------------------------------------------------------
Km21KR2::~Km21KR2()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::allowReversHandle(bool allow)
{
    is_reverse_handle_allowed = allow;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool Km21KR2::isReversHandleAllowed() const
{
    return is_reverse_handle_allowed;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::insertReversHandle(bool insert)
{
    insert = insert && is_reverse_handle_allowed;

    if (insert)
    {
        // Вставляем реверсивную рукоятку
        is_revers_handle.set();
        return;
    }

    // Извлечение реверсивной рукоятки только в нулевом положении
    if (reverseState == 0)
    {
        is_revers_handle.reset();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool Km21KR2::isReversHandle() const
{
    return is_revers_handle.getState();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::allowChangeReversPos(bool allow)
{
    is_reverse_change_allowed = allow;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool Km21KR2::isChangeReversAllowed() const
{
    return is_reverse_change_allowed;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::setReversHandlePos(int pos)
{
    if (isReversHandle())
    {
        if (is_reverse_change_allowed && (getMainPos() == 0))
        {
            pos = std::clamp(pos, -1, 1);
        }
        else
        {
            // Пока контроллер не в нулевой позиции
            // и не подаёт напряжение на электромагнитную защёлку
            // не меняем положение реверсивки
            pos = reverseState;
        }
    }
    else
    {
        // При снятой реверсивке всегда в нуле
        pos = 0;
    }

    if (reverseState != pos)
    {
        sounds[REVERS_CHANGE_POS_SOUND].play();

        reverseState = pos;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int Km21KR2::getReversHandlePos() const
{
    return reverseState;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::setControlPos(int pos)
{
    if ((reverseState != 0) && (getMainHeight() < 0.01))
    {
        pos = std::clamp(pos, -2, 2);
    }
    else
    {
        // При реверсивке в нуле или вдавленном штурвале блокируем командный вал
        pos = 0;
    }

    if (mainShaftPos != pos)
    {
        switch (pos)
        {
        case -2:
        {
            sounds[MAIN_FIXED_RESET_ON_SOUND].play();
            break;
        }
        case -1:
        case 1:
        case 2:
        {
            sounds[MAIN_NONFIXED_ON_SOUND].play();
            break;
        }
        case 0:
        {
            if (mainShaftPos == -2)
                sounds[MAIN_FIXED_RESET_OFF_SOUND].play();
            else
                sounds[MAIN_NONFIXED_OFF_SOUND].play();
            break;
        }
        default:
        {
            break;
        }
        }

        mainShaftPos = pos;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::setFieldWeakPos(int pos)
{
    if ((reverseState != 0) && (mainShaftPos == 0))
    {
        // Управляем вдавливанием штурвала
        ref_height_for_field_weak = (pos > 0);

        if (getMainHeight() > 0.99)
        {
            pos = std::clamp(pos, 0, 5);
        }
        else
        {
            // Пока не вдавлен штурвал, блокируем вал ослабления поля
            pos = 0;
        }
    }
    else
    {
        // При реверсивке в нуле или командном валу не в нуле блокируем вал ослабления поля
        ref_height_for_field_weak = false;
        pos = 0;
    }

    if (fieldWeakShaft != pos)
    {
        sounds[MAIN_CHANGE_FIELDWEAK_SOUND].play();

        fieldWeakShaft = pos;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int Km21KR2::getMainPos() const
{
    if (getMainHeight() < 0.01)
    {
        return mainShaftPos;
    }

    return fieldWeakShaft;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double Km21KR2::getMainHeight() const
{
    return getY(0);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
ControllerState Km21KR2::getCtrlState()
{
    return controlState;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
sound_state_t Km21KR2::getSoundState(size_t idx) const
{
    if (idx < NUM_SOUNDS)
    {
        return sounds[idx];
    }

    return is_revers_handle.getSoundState(idx - NUM_SOUNDS);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float Km21KR2::getSoundSignal(size_t idx) const
{
    if (idx < NUM_SOUNDS)
    {
        return sounds[idx].createSoundSignal();
    }

    return is_revers_handle.getSoundSignal(idx - NUM_SOUNDS);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::ode_system(const state_vector_t& Y, state_vector_t& dYdt, double t)
{
    (void) t;

    dYdt[0] = (static_cast<double>(ref_height_for_field_weak) - Y[0]) / 0.1;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::stepKeysControl(double t, double dt)
{
    (void) t;
    (void) dt;

    controlState.k01 = (reverseState == 1);
    controlState.k02 = (reverseState == -1);

    controlState.k21 = (mainShaftPos == -2 || mainShaftPos == 0  || mainShaftPos == 2);
    controlState.k22 = (mainShaftPos == 0   || mainShaftPos == 1  || mainShaftPos == 2);
    controlState.k23 = (mainShaftPos == 1   || mainShaftPos == 2);
    controlState.k25 = (mainShaftPos == -2 || mainShaftPos == -1 || mainShaftPos == 0);

    controlState.k31 = (fieldWeakShaft == 1 || fieldWeakShaft == 4 || fieldWeakShaft == 5 );
    controlState.k32 = (fieldWeakShaft == 2 || fieldWeakShaft == 4);
    controlState.k33 = (fieldWeakShaft == 3 || fieldWeakShaft == 5);

    if (!pressed_keys)
    {
        old_key_state_fwd_or_bwd = false;
        old_key_state_inc_or_dec = false;

        if ((getMainHeight() <= 0.99) && (mainShaftPos != -2))
        {
            setControlPos(0);
        }
        return;
    }

    bool key_fwd = getKeyState(*pressed_keys, KEY_W);
    bool key_bwd = getKeyState(*pressed_keys, KEY_S);
    bool key_traction = getKeyState(*pressed_keys, KEY_A);
    bool key_reset = getKeyState(*pressed_keys, KEY_D);
    bool key_traction_auto = getKeyState(*pressed_keys, KEY_Q);
    bool key_reset_auto = getKeyState(*pressed_keys, KEY_E);
    bool isShift = isModifier(*pressed_keys, MODIFIER_OnlyShift);
    bool isControl = isModifier(*pressed_keys, MODIFIER_OnlyControl);

    if (key_fwd)
    {
        if (isShift)
        {
            old_key_state_fwd_or_bwd = true;

            // Shift - вставляем реверсивку
            insertReversHandle(true);
            return;
        }

        if (isControl)
        {
            old_key_state_fwd_or_bwd = true;

            // Ctrl - извлекаем реверсивку
            insertReversHandle(false);
            return;
        }

        if (!old_key_state_fwd_or_bwd)
        {
            // Управление реверсивной рукояткой
            setReversHandlePos(reverseState + 1);
        }

        old_key_state_fwd_or_bwd = true;
        return;
    }

    if (key_bwd)
    {
        if (isControl)
        {
            old_key_state_fwd_or_bwd = true;

            // Ctrl - быстрый возврат в нулевую позицию
            setReversHandlePos(0);
            return;
        }

        if (!old_key_state_fwd_or_bwd)
        {
            // Управление реверсивной рукояткой
            setReversHandlePos(reverseState - 1);
        }

        old_key_state_fwd_or_bwd = true;
        return;
    }

    old_key_state_fwd_or_bwd = false;

    if (key_reset_auto)
    {
        old_key_state_inc_or_dec = true;

        // Управление командным валом
        setControlPos(-2);
        return;
    }

    if (key_reset)
    {
        if (isControl)
        {
            old_key_state_inc_or_dec = true;

            // Ctrl - Быстрый сброс штурвала в ноль
            setControlPos(0);
            setFieldWeakPos(0);
            return;
        }

        if (mainShaftPos == -2)
        {
            old_key_state_inc_or_dec = true;
            return;
        }

        if (!old_key_state_inc_or_dec)
        {
            if (isShift)
            {
                old_key_state_inc_or_dec = true;

                // Управление валом ослабления поля
                setFieldWeakPos(fieldWeakShaft - 1);
                return;
            }

            // Управление командным валом
            setControlPos(-1);
        }

        old_key_state_inc_or_dec = true;
        return;
    }

    if (key_traction)
    {
        if ((mainShaftPos == -2) && (!key_reset_auto))
        {
            old_key_state_inc_or_dec = true;

            // Возврат из автосброса
            setControlPos(0);
            return;
        }

        if (!old_key_state_inc_or_dec)
        {
            if (isShift)
            {
                // Обрабатываем удержание клавиши до вдавливания штуравла
                old_key_state_inc_or_dec = (getMainHeight() > 0.99);

                // Управление валом ослабления поля
                setFieldWeakPos(fieldWeakShaft + 1);
                return;
            }

            // Управление командным валом
            setControlPos(1);
        }

        old_key_state_inc_or_dec = true;
        return;
    }

    if (key_traction_auto)
    {
        if ((mainShaftPos == -2) && (!key_reset_auto))
        {
            // Возврат из автосброса
            old_key_state_inc_or_dec = true;
            setControlPos(0);
            return;
        }

        if (!old_key_state_inc_or_dec)
        {
            // Управление командным валом
            setControlPos(2);
        }

        old_key_state_inc_or_dec = true;
        return;
    }

    old_key_state_inc_or_dec = false;

    // Автовозврат из нефиксированных положений
    if (mainShaftPos != -2)
    {
        setControlPos(0);
    }
    if (fieldWeakShaft == 0)
    {
        ref_height_for_field_weak = false;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::stepExternalControl(double t, double dt)
{
    (void) t;
    (void) dt;

    if (!control_signals)
        return;

    /*connectSignals(KM_K01, k01);
    connectSignals(KM_K02, k02);
    connectSignals(KM_K21, k21);
    connectSignals(KM_K22, k22);
    connectSignals(KM_K23, k23);
    connectSignals(KM_K25, k25);
    connectSignals(KM_K31, k31);
    connectSignals(KM_K32, k32);
    connectSignals(KM_K33, k33);*/
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::connectSignals(ControllerSignals cs, bool &k)
{
    if (control_signals->analogSignal[cs].is_active)
        k = static_cast<bool>(control_signals->analogSignal[cs].cur_value);
}
