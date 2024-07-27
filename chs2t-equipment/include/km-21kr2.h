#ifndef KM21KR2_H
#define KM21KR2_H

#include "device.h"
#include "km-21kr2-state.h"
#include  "hardware-signals.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class Km21KR2 : public Device
{
public:
    /// Конструктор
    Km21KR2(QObject *parent = Q_NULLPTR);

    ///Деструктор
    ~Km21KR2();

    void setHod(bool hod) { this->hod = hod; }

    ControllerState getCtrlState() { return controlState; }

    double getMainShaftPos() const
    {
        if (fieldWeakShaft == 0)
            return mainShaftPos * 0.1;
        else
            return fieldWeakShaft * 0.1;
    }

    double getHandleHeight() const { return getY(0); }

    enum {
        NUM_SOUNDS = 6,
        REVERS_CHANGE_POS_SOUND = 0,    ///< Звук переключения реверсора
        MAIN_NONFIXED_ON_SOUND = 1,     ///< Звук постановки контроллера в нефиксированные положения сброса/набора
        MAIN_NONFIXED_OFF_SOUND = 2,    ///< Звук возврата контроллера из нефиксированных положения сброса/набора
        MAIN_FIXED_RESET_ON_SOUND = 3,  ///< Звук постановки контроллера в фиксированное положение автоматического сброса
        MAIN_FIXED_RESET_OFF_SOUND = 4, ///< Звук возврата контроллера из фиксированного положения автоматического сброса
        MAIN_CHANGE_FIELDWEAK_SOUND = 5 ///< Звук переключения позиций ослабления поля во вдавленном положении контроллера
    };
    /// Состояние звука
    sound_state_t getSoundState(size_t idx = REVERS_CHANGE_POS_SOUND) const;

    /// Сигнал состояния звука
    float getSoundSignal(size_t idx = REVERS_CHANGE_POS_SOUND) const;

private:

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);

    void preStep(state_vector_t &Y, double t);

    void stepKeysControl(double t, double dt);

    void stepExternalControl(double t, double dt);

    void connectSignals(ControllerSignals cs, bool &k);

    void addSignalsInControllerState();

    bool k01;
    bool k02;

    bool k21;
    bool k22;
    bool k23;
    bool k25;

    bool k31;
    bool k32;
    bool k33;


    bool autoSet;
    bool autoReset;
    bool reverseIsPressedOneTime;
    bool hod;

    int reverseState;

    int mainShaftPos;
    int fieldWeakShaft;

    double mainShaftHeight;

    bool is_inc;
    bool is_dec;
    bool no_from_weak;

    ControllerState controlState;

    /// Счётчик и состояние звуков
    std::array <sound_state_t, NUM_SOUNDS> sounds;
};

#define TO_INT(variable) static_cast<int>(variable)

#endif // KM21KR2_H
