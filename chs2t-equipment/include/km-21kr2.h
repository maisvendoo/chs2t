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

    /// Деструктор
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

    /// Состояние электромагнитной защёлки, разрешающей переключение реверса
    bool isChangeReversAllowed() const;

    /// Задать положение реверсивной рукоятки (-1, 0, 1)
    void setReversHandlePos(std::int8_t pos);

    /// Положение реверсивной рукоятки
    std::int8_t getReversHandlePos() const;

    /// Задать положение контроллера для управления тяговыми позициями (-2, -1, 0, 1, 2)
    void setControlPos(std::int8_t pos);

    /// Задать положение контроллера для управления ослаблением поля (0, 1, 2, 3, 4, 5)
    void setFieldWeakPos(std::int8_t pos);

    /// Положение контроллера (-2 .. 2 или 0 .. 5)
    std::int8_t getMainPos() const;

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

    void lockManualControl(bool lock)
    {
        lock_manual_control = lock;
    }

private:

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void stepKeysControl(double t, double dt);

    void stepExternalControl(double t, double dt);

    void connectSignals(ControllerSignals cs, bool &k);

    /// Разрешение установить реверсивку (для реализации одной рукоятки на несколько кабин)
    bool is_reverse_handle_allowed = true;

    /// Cостояние электромагнитной защёлки, разрешающей переключение реверса
    bool is_reverse_change_allowed = true;

    /// Вдавленное состояние контроллера для управления ослаблением поля
    bool ref_height_for_field_weak = false;

    /// Предыдущее состояние управляющих клавиш
    bool old_key_state_fwd_or_bwd = false;
    bool old_key_state_inc_or_dec = false;

    /// Положение реверсивной рукоятки
    std::int8_t reverseState = 0;

    /// Положение командного вала
    std::int8_t mainShaftPos = 0;

    /// Положение вала ослабления поля
    std::int8_t fieldWeakShaft = 0;

    /// Состояние всех контактов
    ControllerState controlState;

    bool lock_manual_control = false;

    /// Признак реверсивной рукоятки
    Trigger is_revers_handle;

    /// Счётчик и состояние звуков
    std::array <sound_state_t, NUM_SOUNDS> sounds;
};

#endif // KM21KR2_H
