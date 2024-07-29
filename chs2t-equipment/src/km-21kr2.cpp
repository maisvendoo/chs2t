#include "km-21kr2.h"



//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
Km21KR2::Km21KR2(QObject* parent) : Device(parent)
  , k01(false)
  , k02(false)

  , k21(false)
  , k22(false)
  , k23(false)
  , k25(false)

  , k31(false)
  , k32(false)
  , k33(false)


  , autoSet(false)
  , autoReset(false)
  , reverseIsPressedOneTime(false)
  , hod(false)
  , reverseState(0)
  , mainShaftPos(0.0)
  , fieldWeakShaft(0.0)
  , mainShaftHeight(0.0)
  , is_inc(true)
  , is_dec(true)
  , no_from_weak(true)
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
sound_state_t Km21KR2::getSoundState(size_t idx) const
{
    if (idx < sounds.size())
        return sounds[idx];
    return Device::getSoundState();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float Km21KR2::getSoundSignal(size_t idx) const
{
    if (idx < sounds.size())
        return sounds[idx].createSoundSignal();
    return Device::getSoundSignal();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::ode_system(const state_vector_t& Y, state_vector_t& dYdt, double t)
{
    Q_UNUSED(t)
    Q_UNUSED(Y)
    Q_UNUSED(dYdt)

    dYdt[0] = (mainShaftHeight - Y[0]) / 0.1;
}

//------------------------------------------------------------------------------
// Загрузка данных из конфигурационного файла
//------------------------------------------------------------------------------
void Km21KR2::load_config(CfgReader& cfg)
{
    Q_UNUSED(cfg)

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::preStep(state_vector_t& Y, double t)
{
    Q_UNUSED(t)
    Q_UNUSED(Y)

    addSignalsInControllerState();

    k01 = (reverseState == 1);

    k02 = (reverseState == -1);

    k25 = (mainShaftPos == -10 || mainShaftPos == -5 || mainShaftPos == 0);
    k21 = (mainShaftPos == -10 || mainShaftPos == 0  || mainShaftPos == 4);
    k22 = (mainShaftPos == 0   || mainShaftPos == 2  || mainShaftPos == 4);
    k23 = (mainShaftPos == 2   || mainShaftPos == 4);

    k31 = (fieldWeakShaft == 2 || fieldWeakShaft == 8 || fieldWeakShaft == 10 );
    k32 = (fieldWeakShaft == 4 || fieldWeakShaft == 8);
    k33 = (fieldWeakShaft == 6 || fieldWeakShaft == 10);

    if ((k01 != controlState.k01) || (k02 != controlState.k02))
        sounds[REVERS_CHANGE_POS_SOUND].play();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::stepKeysControl(double t, double dt)
{
    Q_UNUSED(t)
    Q_UNUSED(dt)

    // Реверсор
    if (!reverseIsPressedOneTime && (mainShaftPos == 0) && (fieldWeakShaft == 0))
        reverseState += ((getKeyState(KEY_W) && (reverseState != 1)) -
                         (getKeyState(KEY_S) && (reverseState != -1)));
    // Запрещаем управлять реверсором дальше, пока не отпустим клавишу
    reverseIsPressedOneTime = (getKeyState(KEY_W) || getKeyState(KEY_S));

    // При реверсоре в нуле контроллер заблокирован, дальше делать нечего
    if (reverseState == 0)
        return;

    // Здесь страшным образом описывается состояние контроллера
    mainShaftPos = (-10 * autoReset) + (4 * autoSet) +
                   (!autoReset && !autoSet && !isShift() && !isControl()) *
                       (-5 * getKeyState(KEY_D) +
                        2 * getKeyState(KEY_A));

    mainShaftPos = mainShaftPos * TO_INT(hs_n(mainShaftHeight - 0.99));

    // Отмена автоматического сброса позиций
    if (autoReset)
    {
        if ( (!getKeyState(KEY_E)) &&
             ( getKeyState(KEY_A) || getKeyState(KEY_Q) || (isControl() && getKeyState(KEY_D)) ) )
        {
            autoReset = false;
            sounds[MAIN_FIXED_RESET_OFF_SOUND].play();
        }
        return;
    }

    // Отмена автоматического набора позиций
    if (autoSet)
    {
        if (!getKeyState(KEY_Q))
        {
            autoSet = false;
            sounds[MAIN_NONFIXED_OFF_SOUND].play();
        }
        return;
    }

    // Автоматический набор-сброс, если не в позициях ослабления поля
    if (fieldWeakShaft == 0)
    {
        // Автоматический сброс позиций
        if (getKeyState(KEY_E))
        {
            if (!autoReset)
            {
                sounds[MAIN_NONFIXED_ON_SOUND].play();
                sounds[MAIN_FIXED_RESET_ON_SOUND].play();
            }
            autoReset = true;
            return;
        }

        // Автоматический набор позиций
        if (getKeyState(KEY_Q))
        {
            if (!autoSet)
                sounds[MAIN_NONFIXED_ON_SOUND].play();
            autoSet = true;
            return;
        }
    }

    // Сброс одной позиции
    if (getKeyState(KEY_D))
    {
        // Возврат контроллера - сброс ослабления поля полностью
        if (isControl() && (fieldWeakShaft > 0))
        {
            fieldWeakShaft = 0;
            mainShaftHeight = 0.0;
            sounds[MAIN_CHANGE_FIELDWEAK_SOUND].play();
            // Запрещаем озвучку обычного возврата
            no_from_weak = false;
            return;
        }

        // Ослабление поля
        if (is_dec && isShift() && (fieldWeakShaft > 0))
        {
            // Сброс одной позиции ослабления поля
            fieldWeakShaft -= 2;
            sounds[MAIN_CHANGE_FIELDWEAK_SOUND].play();

            // Сбрасываем вдавленное состояние контроллера
            if (fieldWeakShaft == 0)
            {
                mainShaftHeight = 0.0;
                // Запрещаем озвучку обычного возврата
                no_from_weak = false;
            }
        }
        else
        {
            // Озвучка сброса одной позиции
            if (is_dec && (!isControl()) && (!isShift()) && (fieldWeakShaft == 0))
                sounds[MAIN_NONFIXED_ON_SOUND].play();
        }
        // Запрещаем озвучку следующего сброса позиции, пока не отпустим клавишу
        is_dec = false;

        return;
    }
    else
    {
        // Озвучка возврата контроллера
        if ((!is_dec) && (no_from_weak) && (!isControl()) && (!isShift()) && (fieldWeakShaft == 0))
            sounds[MAIN_NONFIXED_OFF_SOUND].play();

        // Клавиша отпущена, разрешаем озвучку следующего сброса позиции
        is_dec = true;
        no_from_weak = true;
    }

    // Набор одной позиции
    if (getKeyState(KEY_A))
    {
        // Ослабление поля
        if (is_inc && isShift())
        {
            // Задаём вдавленное состояние контроллера
            if (fieldWeakShaft == 0)
                mainShaftHeight = 1.0;

            // Если контроллер вдавился, набор одной позиции ослабления поля
            if ((getY(0) > 0.99) && (fieldWeakShaft != 10))
            {
                fieldWeakShaft += 2;
                sounds[MAIN_CHANGE_FIELDWEAK_SOUND].play();

                // Запрещаем набор следующей позиции, пока не отпустим клавишу
                is_inc = false;
            }
        }
        else
        {
            // Озвучка набора одной позиции
            if (is_inc && (fieldWeakShaft == 0))
                sounds[MAIN_NONFIXED_ON_SOUND].play();

            // Запрещаем озвучку следующего набора позиции, пока не отпустим клавишу
            is_inc = false;
        }
    }
    else
    {
        // Озвучка возврата контроллера
        if ((!is_inc) && (fieldWeakShaft == 0))
            sounds[MAIN_NONFIXED_OFF_SOUND].play();

        // Клавиша отпущена, разрешаем озвучку следующего набора позиции
        is_inc = true;

        // Сбрасываем вдавленное состояние контроллера,
        // если не успели набрать позиции ослабления поля пока нажата клавиша
        if (fieldWeakShaft == 0)
            mainShaftHeight = 0.0;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::stepExternalControl(double t, double dt)
{
    Q_UNUSED(t)
    Q_UNUSED(dt)

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
    if (control_signals.analogSignal[cs].is_active)
        k = static_cast<bool>(control_signals.analogSignal[cs].cur_value);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Km21KR2::addSignalsInControllerState()
{
    controlState.k01 = k01;
    controlState.k02 = k02;

    controlState.k21 = k21;
    controlState.k22 = k22;
    controlState.k23 = k23;
    controlState.k25 = k25;

    controlState.k31 = k31;
    controlState.k32 = k32;
    controlState.k33 = k33;
}
