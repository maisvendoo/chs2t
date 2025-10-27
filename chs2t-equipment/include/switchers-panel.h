#ifndef     CHS2T_SWITCHERS_PANEL_H
#define     CHS2T_SWITCHERS_PANEL_H

#include    "device.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class CHS2tSwitchers final : public Device
{
public:

    enum : std::uint8_t
    {
        /// Переключатели
        EPB = 0,
        COMPR_1 = 1,
        PANT_FWD = 2,
        FAST_SW = 3,

        FANS = 4,
        COMPR_2 = 5,
        PANT_BWD = 6,
        BLINDS = 7,

        CAB_LIGHT = 8,
        BUFLIGHT_L = 9,
        BUFLIGHT_R = 10,
        SPOTLIGHT = 11,

        EDT = 12,

        /// Позиции переключателей
        EPB_NUM = 2,
        EPB_OFF = 0,
        EPB_ON = 1,

        FANS_NUM = 3,
        FANS_ON = 0,
        FANS_OFF = 1,
        FANS_AUTO = 2,

        COMPR_NUM = 4,
        COMPR_HEAT = 0,
        COMPR_OFF = 1,
        COMPR_AUTO = 2,
        COMPR_ON = 3,

        BLINDS_NUM = 5,
        AUXCOMPR = 0,
        AUTO_SAND = 1,
        BLINDS_OPEN = 2,
        AUTO_BLINDS = 3,
        AUTO_BLINDS_SAND = 4,

        PANT_NUM = 4,
        PANT_GROUND = 0,
        PANT_DOWN = 1,
        PANT_UP = 2,
        PANT_ON = 3,

        FAST_SW_NUM = 4,
        FAST_SW_NO_CTRL = 0,
        FAST_SW_OFF = 1,
        FAST_SW_WORK = 2,
        FAST_SW_ON = 3,

        CAB_LIGHT_NUM = 5,
        CAB_LIGHT_DEVICES_HIGH = 0,
        CAB_LIGHT_DEVICES_LOW = 1,
        CAB_LIGHT_OFF = 2,
        CAB_LIGHT_LOW = 3,
        CAB_LIGHT_HIGH = 4,

        BUFLIGHT_NUM = 3,
        BUFLIGHT_RED = 0,
        BUFLIGHT_OFF = 1,
        BUFLIGHT_WHITE = 2,

        SPOTLIGHT_NUM = 3,
        SPOTLIGHT_OFF = 0,
        SPOTLIGHT_LOW = 1,
        SPOTLIGHT_HIGH = 2,

        EDT_NUM = 2,
        EDT_OFF = 0,
        EDT_ON = 1,
    };

    CHS2tSwitchers(QObject *parent = nullptr);

    ~CHS2tSwitchers() override;

    void setControl(std::set<std::uint16_t>* keys = nullptr,
                    control_signals_t* control_signals = nullptr) override;

    void step(double t, double dt) override;

    /// Разрешить установить ключ (для реализации одного ключа на несколько кабин)
    void allowKey(bool allow);

    /// Разрешение установить ключ (для реализации одного ключа на несколько кабин)
    bool isKeyAllowed() const;

    /// Вставить/извлечь ключ
    void insertKey(bool insert);

    /// Признак установленного ключа
    bool isKey() const;

    /// Переключить ключ: false - заблокировать тумблеры, true - разблокировать
    void setKeyOn(bool state);

    /// Состояние ключа: false - тумблеры заблокированы, true - разблокированы
    bool isKeyOn() const;

    /// Задать начальную позицию [0 .. N-1] (без озвучки), возвращает true если состояние изменилось
    bool setSwitcherInitPosition(size_t switcher_idx, std::uint16_t position);

    /// Задать позицию [0 .. N-1], возвращает true если состояние изменилось
    bool setSwitcherPosition(size_t switcher_idx, std::uint16_t position);

    /// Количество позиций N
    std::uint16_t getSwitcherNumPositions(size_t switcher_idx) const;

    /// Текущая позиция, 0 - N-1
    std::uint16_t getSwitcherPosition(size_t switcher_idx) const;

    /// Текущее относительное положение переключателя, 0.0 - 1.0
    float getSwitcherHandlePosition(size_t switcher_idx) const;

    /// Состояние позиции
    bool isSwitched(size_t switcher_idx, std::uint16_t pos) const;

    float getKeyInsertSoundSignal(size_t idx = Trigger::CHANGE_SOUND) const;

    float getKeyTurnSoundSignal(size_t idx = Trigger::CHANGE_SOUND) const;

    float getSoundSignal(size_t switcher_idx) const override;

private:

    enum { SWITCHERS_NUMBER = 13 };
    static constexpr std::uint8_t default_pos[] = {EPB_OFF, COMPR_OFF, PANT_DOWN, FAST_SW_OFF, FANS_OFF, COMPR_OFF, PANT_DOWN, BLINDS_OPEN, CAB_LIGHT_OFF, BUFLIGHT_OFF, BUFLIGHT_OFF, SPOTLIGHT_OFF, EDT_OFF};
    static constexpr std::uint8_t locked_idx[] = {EPB, COMPR_1, PANT_FWD, FAST_SW, FANS, COMPR_2, PANT_BWD, BLINDS};
    static constexpr std::uint8_t no_locked_idx[] = {CAB_LIGHT, BUFLIGHT_L, BUFLIGHT_R, SPOTLIGHT, EDT};

    /// Управляющая клавиша ключа
    std::uint16_t key_symbol = KEY_Undefined;

    /// Предыдущее состояние управляющей клавиши
    bool prev_key = false;

    /// Разрешение установить ключ (для реализации одного ключа на несколько кабин)
    bool is_key_allowed = true;

    /// Признак установленного ключа
    Trigger is_key = Trigger();

    /// Состояние ключа: false - переключатели заблокированы, true - разблокированы
    Trigger key_state = Trigger();

    /// Переключатели
    SwitcherControl switchers[SWITCHERS_NUMBER];

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t) override;

    void initControl();

    bool isSwitchersOff() const;
};

#endif // CHS2T_SWITCHERS_PANEL_H
