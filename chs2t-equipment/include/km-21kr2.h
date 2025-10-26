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

    /// Разрешить установить реверсивку (для реализации одной рукоятки на несколько кабин)
    void allowReversHandle(bool allow);

    /// Разрешение установить реверсивку (для реализации одной рукоятки на несколько кабин)
    bool isReversHandleAllowed() const;

    /// Вставить/извлечь реверсивную рукоятку
    void insertReversHandle(bool insert);

    /// Признак вставленной реверсивной рукоятки
    bool isReversHandle() const;

    /// Задать состояние электромагнитной защёлки, разрешающей переключение реверса
    void allowChangeReversPos(bool allow);

    /// Задать состояние электромагнитной защёлки, разрешающей переключение реверса
    bool isChangeReversAllowed() const;

    /// Задать положение реверсивной рукоятки (-1, 0, 1)
    void setReversHandlePos(int pos);

    /// Положение реверсивной рукоятки
    int getReversHandlePos() const;

    /// Задать положение контроллера для управления тяговыми позициями (-2, -1, 0, 1, 2)
    void setControlPos(int pos);

    /// Задать положение контроллера для управления ослаблением поля (0, 1, 2, 3, 4, 5)
    void setFieldWeakPos(int pos);

    /// Положение контроллера (-2 .. 2 или 0 .. 5)
    int getMainPos() const;

    /// Вдавленное состояние контроллера (0.0 .. 1.0)
    double getMainHeight() const;

    /// Контакты валов контроллера
    ControllerState getCtrlState();

    enum {
        NUM_SOUNDS = 6,
        REVERS_CHANGE_POS_SOUND = 0,    ///< Звук переключения реверсора
        MAIN_NONFIXED_ON_SOUND = 1,     ///< Звук постановки контроллера в нефиксированные положения сброса/набора
        MAIN_NONFIXED_OFF_SOUND = 2,    ///< Звук возврата контроллера из нефиксированных положения сброса/набора
        MAIN_FIXED_RESET_ON_SOUND = 3,  ///< Звук постановки контроллера в фиксированное положение автоматического сброса
        MAIN_FIXED_RESET_OFF_SOUND = 4, ///< Звук возврата контроллера из фиксированного положения автоматического сброса
        MAIN_CHANGE_FIELDWEAK_SOUND = 5,///< Звук переключения позиций ослабления поля во вдавленном положении контроллера
        HANDLE_CHANGE_SOUND = NUM_SOUNDS + Trigger::CHANGE_SOUND,
        HANDLE_INSERTED_SOUND = NUM_SOUNDS + Trigger::ON_SOUND,
        HANDLE_REMOVED_SOUND = NUM_SOUNDS + Trigger::OFF_SOUND,
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

    /// Разрешение установить реверсивку (для реализации одной рукоятки на несколько кабин)
    bool is_reverse_handle_allowed = true;

    /// Cостояние электромагнитной защёлки, разрешающей переключение реверса
    bool is_reverse_change_allowed = true;

    /// Предыдущее состояние управляющих клавиш
    bool old_key_state_fwd_or_bwd = false;
    bool old_key_state_inc_or_dec = false;

    /// Контакты реверсивного вала
    bool k01 = false;
    bool k02 = false;

    /// Контакты командного вала
    bool k21 = true;
    bool k22 = true;
    bool k23 = true;
    bool k25 = false;

    /// Контакты вала ослабления поля
    bool k31 = false;
    bool k32 = false;
    bool k33 = false;

    /// Вдавленное состояние контроллера для управления ослаблением поля
    bool ref_height_for_field_weak = false;

    /// Положение реверсивной рукоятки
    int reverseState;

    int mainShaftPos;
    int fieldWeakShaft;

    bool autoSet;
    bool autoReset;
    bool reverseIsPressedOneTime;


    bool is_inc;
    bool is_dec;
    bool no_from_weak;

    /// Признак реверсивной рукоятки
    Trigger is_revers_handle;

    ControllerState controlState;

    /// Счётчик и состояние звуков
    std::array <sound_state_t, NUM_SOUNDS> sounds;
};

#define TO_INT(variable) static_cast<int>(variable)

#endif // KM21KR2_H
