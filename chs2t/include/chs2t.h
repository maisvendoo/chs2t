//------------------------------------------------------------------------------
//
//      Магистральный пассажирский электровоз постоянного тока ЧС2т.
//      Дополнение для Russian Railway Simulator (RRS)
//
//      (c) RRS development team:
//          Дмитрий Притыкин (maisvendoo),
//          Николай Авилкин (avilkin.nick)
//
//      Дата: 21/08/2019
//
//------------------------------------------------------------------------------
#ifndef     CHS2T_H
#define     CHS2T_H

#include    "vehicle-api.h"

#include    "km-21kr2.h"
#include    "stepswitch.h"
#include    "pusk-rez.h"
#include    "motor.h"
#include    "overload-relay.h"
#include    "electropneumovalve-emergency.h"
#include    "electropneumovalve-release.h"
#include    "dako.h"
#include    "generator.h"
#include    "pulse-converter.h"
#include    "brake-regulator.h"
#include    "handle-edt.h"
#include    "motor-fan-dc.h"
#include    "blinds.h"
#include    "hardware-signals.h"
#include    "convert-physics-to-modbus.h"
#include    "sl2m.h"
#include    "energy-counter.h"
#include    "alsn-ukbm.h"

/*!
 * \class
 * \brief Основной класс, описывающий весь электровоз
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class CHS2T : public Vehicle
{
public:

    /// Конструктор
    CHS2T();

    /// Деструктор
    ~CHS2T();

    /// Инициализация тормозных приборов
    void initBrakeDevices(double p0, double pBP, double pFL) override;

private:

    enum
    {
        NUM_PANTOGRAPHS = 2,
        PANT1 = 0,
        PANT2 = 1,
        WIRE_VOLTAGE = 3000
    };

    enum
    {
        NUM_MOTOR_KOMPRESSORS = 2,
        MK1 = 0,
        MK2 = 1
    };

    /// Имя модуля сцепного устройства
    QString coupling_module_name = "sa3";
    /// Имя конфига сцепного устройства
    QString coupling_config_name = "sa3";
    /// Имя модуля поездного крана
    QString brake_crane_module_name = "krm395";
    /// Имя конфига поездного крана
    QString brake_crane_config_name = "krm395";
    /// Имя модуля локомотивного крана
    QString loco_crane_module_name = "kvt254";
    /// Имя конфига локомотивного крана
    QString loco_crane_config_name = "kvt254";
    /// Имя модуля воздухораспределителя
    QString airdist_module_name = "vr292";
    /// Имя конфига воздухорапределителя
    QString airdist_config_name = "vr292";
    /// Имя модуля электровоздухораспределителя
    QString electro_airdist_module_name = "evr305";
    /// Имя конфига электровоздухорапределителя
    QString electro_airdist_config_name = "evr305";

    /// Регистратор, для записи параметров
    Registrator *reg = nullptr;

    /// Сцепка спереди
    Coupling *coupling_fwd = nullptr;
    /// Сцепка сзади
    Coupling *coupling_bwd = nullptr;

    /// Расцепной рычаг спереди
    OperatingRod *oper_rod_fwd = nullptr;
    /// Расцепной рычаг сзади
    OperatingRod *oper_rod_bwd = nullptr;

    enum
    {
        CABS_NUM = 2,
        CAB1 = 0,
        CAB2 = 1
    };

    /// Кнопка "Свисток"
    TriggerControl button_svistok[CABS_NUM];

    /// Кнопка "Тифон"
    TriggerControl button_tifon[CABS_NUM];

    /// Кнопка "Песок"
    TriggerControl button_sand[CABS_NUM];

    /// Кнопка "Отпуск электровоза"
    TriggerControl button_loco_release[CABS_NUM];

    /// Тумблер включения ЭПТ
    TriggerControl epb_switch[CABS_NUM];

    /// Галетник управления мотор-вентиляторами
    SwitcherControl motor_fan_switcher[CABS_NUM];

    /// Галетники управления мотор-компрессорами
    SwitcherControl mk_switcher[CABS_NUM][NUM_MOTOR_KOMPRESSORS];

    /// Галетники управления токоприемниками
    SwitcherControl pant_switcher[CABS_NUM][NUM_PANTOGRAPHS];

    /// Галетник управления БВ
    SwitcherControl fastswitch_switcher[CABS_NUM];

    /// Галетник управления жалюзи
    SwitcherControl blinds_switcher[CABS_NUM];

    /// Галетник управления освещением кабины и приборов
    SwitcherControl cab_light_switcher[CABS_NUM];

    /// Галетник управления левым буферным огнём
    SwitcherControl bufferlight_L_switcher[CABS_NUM];

    /// Галетник управления правым буферным огнём
    SwitcherControl bufferlight_R_switcher[CABS_NUM];

    /// Галетник управления прожектором
    SwitcherControl spotlight_switcher[CABS_NUM];

    enum
    {
        NUM_RB = 3,
        RBS = 0,
        RB1 = 1,
        RBP = 2
    };

    /// Триггеры рукояток бдительности
    TriggerControl rb[CABS_NUM][NUM_RB];

    /// Ключ ЭПК
    TriggerControl key_epk[CABS_NUM];

    /// Выключатель ЭДТ
    TriggerControl EDT_switch[CABS_NUM];

    /// Кнопка "Сброс СП-С"
    TriggerControl button_sbros_cpc[CABS_NUM];

    /// Контроллер машиниста
    Km21KR2* km21KR2[CABS_NUM] = {nullptr, nullptr};


    /// Напряжение аккумуляторной батареи
    double U_bat = 55.0;

    /// Схема тяги
    Motor* motor = nullptr;

    /// Токоприемники
    std::array<Pantograph*, NUM_PANTOGRAPHS> pantographs = {nullptr, nullptr};

    /// Быстрый выключатель
    ProtectiveDevice* bv = nullptr;

    /// Пусковой резистор
    PuskRez* puskRez = nullptr;

    /// Переключатель ступеней
    StepSwitch* stepSwitch = nullptr;

    /// Возврат защиты
    bool bv_return = false;

    /// Реле перегрузки ТЭД
    OverloadRelay* overload_relay = nullptr;

    PhysToModbus* TM_manometer = nullptr;
    PhysToModbus* UR_manometer = nullptr;
    PhysToModbus* ZT_manometer = nullptr;
    PhysToModbus* GR_manometer = nullptr;
    PhysToModbus* TC_manometer = nullptr;

    /// Мотор-компрессоры (МК)
    std::array<DCMotorCompressor*, 2> motor_compressor = {nullptr, nullptr};

    /// Регулятор давления ГР
    PressureRegulator* press_reg = nullptr;

    /// Главный резервуар
    Reservoir*  main_reservoir = nullptr;

    /// Концевой кран питательной магистрали спереди
    PneumoAngleCock* anglecock_fl_fwd = nullptr;

    /// Концевой кран питательной магистрали сзади
    PneumoAngleCock* anglecock_fl_bwd = nullptr;

    /// Рукав питательной  магистрали спереди
    PneumoHose* hose_fl_fwd = nullptr;

    /// Рукав питательной  магистрали сзади
    PneumoHose* hose_fl_bwd = nullptr;

    /// Поездной кран машиниста усл.№395
    BrakeCrane* brake_crane[CABS_NUM] = {nullptr, nullptr};

    /// Кран впомогательного тормоза усл.№254
    LocoCrane*  loco_crane[CABS_NUM] = {nullptr, nullptr};

    /// Рукоятка задатчика тормозного усилия
    HandleEDT*  handleEDT[CABS_NUM] = {nullptr, nullptr};

    /// Электропневматический клапан автостопа усл.№150
    AutoTrainStop*  epk[CABS_NUM] = {nullptr, nullptr};

    /// Электропневматический вентиль экстренного торможения (при ТМ < 0.3 МПа)
    ElectroPneumoValveEmergency* emergency_valve = nullptr;

    /// Электропневматический вентиль отпуска пневматических тормозов
    ElectroPneumoValveRelease*  release_valve = nullptr;

    /// Управляющая камера воздухораспределителя (ложный ТЦ)
    Reservoir*   brake_ref_res = nullptr;

    /// Тормозная магистраль
    Reservoir*   brakepipe = nullptr;

    /// Воздухораспределитель
    AirDistributor*  air_dist = nullptr;

    /// Электровоздухораспределитель
    ElectroAirDistributor*  electro_air_dist = nullptr;

    /// Запасный резервуар
    Reservoir*   supply_reservoir = nullptr;

    /// Разветвитель потока воздуха от локомотивного крана к тележкам
    PneumoSplitter*  loco_crane_splitter = nullptr;

    /// Скоростной клапан ДАКО
    Dako* dako = nullptr;

    /// Повторительное реле давления усл.№304
    PneumoRelay*     bc_pressure_relay = nullptr;

    /// Концевой кран тормозной магистрали спереди
    PneumoAngleCock* anglecock_bp_fwd = nullptr;

    /// Концевой кран тормозной магистрали сзади
    PneumoAngleCock* anglecock_bp_bwd = nullptr;

    /// Рукав тормозной магистрали спереди
    PneumoHoseEPB*   hose_bp_fwd = nullptr;

    /// Рукав тормозной магистрали сзади
    PneumoHoseEPB*   hose_bp_bwd = nullptr;

    enum
    {
        NUM_TROLLEYS = 2,
        NUM_AXIS_PER_TROLLEY = 3,
        TROLLEY_FWD = 0,
        TROLLEY_BWD = 1
    };

    /// Переключательные клапаны ЗПК потока в тормозные цилиндры
    std::array<SwitchingValve*, NUM_TROLLEYS> bc_switch_valve = {nullptr, nullptr};

    /// Тормозные механизмы тележек
    std::array<BrakeMech*, NUM_TROLLEYS> brake_mech = {nullptr, nullptr};

    /// Преобразователь питания ЭПТ
    EPBConverter* epb_converter = nullptr;

    /// Блок управления ЭПТ
    EPBControl* epb_control = nullptr;

    DCMotorFan* motor_fan_ptr = nullptr;

    /// Свисток и тифон
    TrainHorn*   horn = nullptr;

    /// Система подачи песка
    SandingSystem*   sand_system = nullptr;

    /// Схема реостатного тормоза
    Generator*   generator = nullptr;

    /// Импульсный преобразователь возбуждения
    PulseConverter*  pulseConv = nullptr;

    /// Регулятор тормозного усилия (САРТ)
    BrakeRegulator*  BrakeReg = nullptr;

    /// Мотор-вентиляторы
    std::array<DCMotorFan*, NUM_TROLLEYS> motor_fan = {nullptr, nullptr};

    /// Напряжение на крышевой шине токоприемников
    double U_kr = 0.0;

    /// Разъединители токоприемников
    std::array<Trigger, NUM_PANTOGRAPHS> pant_switch;

    /// Тригеры поднятия/опускания ТП
    std::array<Trigger, NUM_PANTOGRAPHS> pantup_trigger;

    /// Тригер включения БВ
    Trigger     fast_switch_trigger;

    /// Разрешение тяги
    Trigger     allowTrac;

    bool dropPosition = false;

    Timer EDT_timer;

    /// Передаточное число тягового редуктора
    double      ip = 1.75;

    /// Флаг сбора схемы ЭДТ
    bool        EDT = false;

    /// Флаг разрешения работы ЭДТ
    bool        allowEDT = false;

    /// Жалюзи пуско-тормозных резисторов
    Blinds*      blinds = nullptr;

    /// Скоростемер 3СЛ2М
    SL2M*        speed_meter[CABS_NUM] = {nullptr, nullptr};

    /// Счетчик энергии
    EnergyCounter*   energy_counter = nullptr;

    /// Ограничения скорости на путевой инфраструктуре для кабины А
    SpeedMap*    speedmap_fwd = nullptr;
    /// Ограничения скорости на путевой инфраструктуре для кабины Б
    SpeedMap*    speedmap_bwd = nullptr;

    /// Приёмная катушка АЛСН для кабины А
    CoilALSN*    coil_ALSN_fwd = nullptr;
    /// Приёмная катушка АЛСН для кабины Б
    CoilALSN*    coil_ALSN_bwd = nullptr;

    /// Дешифратор сигнала АЛСН
    DecoderALSN* alsn_decoder[CABS_NUM] = {nullptr, nullptr};

    /// Устройство безопасности
    SafetyDevice*    safety_device[CABS_NUM] = {nullptr, nullptr};


    /// Загрузка данных из конфигурационных файлов
    void loadConfig(QString cfg_path) override;


    /// Общая инициализация локомотива
    void initialization() override;

    /// Инициализация управления тумблерами
    void initTumblers(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициализация сцепных устройств
    void initCouplings(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициализация токоприемников
    void initPantographs(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициадизация рычажки
    void initBrakesMech(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициализация БВ
    void initFastSwitch(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициализация защит
    void initProtection(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициализация питательной магистрали
    void initPneumoSupply(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициализация приборов управления тормозами
    void initBrakesControl(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициализация тормозного оборудования
    void initBrakesEquipment(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициализация ЭПТ
    void initEPB(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициализация схемы управления тягой
    void initTractionControl(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициализация ЭДТ
    void initEDT(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициализация вспомогательного оборудования
    void initSupportEquipment(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициализация приборов безопасности
    void initSafetyDevices(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициализация прочего оборудования
    void initOtherEquipment(const QString& modules_dir, const QString& custom_cfg_dir);

    ///
    void initModbus(const QString& modules_dir, const QString& custom_cfg_dir);

    /// Инициализация регистратора
    void initRegistrator(const QString& modules_dir, const QString& custom_cfg_dir);



    /// Процесс симуляции
    void process(const simulator_time_t& t, const double& dt) override;

    /// Управление
    void keyProcess(const simulator_time_t& t, const double& dt);

    /// Отладочная строка
    void debugPrint(const simulator_time_t& t, const double& dt);

    /// Сигналы для анимации
    void signalsOutput(const simulator_time_t& t, const double& dt);

    /// Сигналы для озвучки
    void soundsOutput(const simulator_time_t& t, const double& dt);
    float motorSoundSignal();

    /// Изменение положений пакетников и тумблеров
    void setSwitcherState(Switcher& sw, signal_t signal);
    void setTriggerState(Trigger& trig, signal_t signal);
    void stepSwitcherPanel(const simulator_time_t& t, const double& dt);

    /// Вывод данных на внешние СОИ
    void hardwareOutput();


    /// Предварительные расчёты перед симуляцией
    void preStep(const double& t) override;

    /// Предварительный расчёт координат сцепных устройств
    void preStepCouplings(const double& t);


    /// Шаг симуляции всех систем электровоза
    void step(const double& t, const double& dt) override;

    /// Моделирование сцепных устройств
    void stepCouplings(const double& t, const double& dt);

    /// Моделирование работы токоприемников
    void stepPantographs(const double& t, const double& dt);

    void stepFastSwitch(const double& t, const double& dt);

    void stepProtection(const double& t, const double& dt);

    /// Моделирование питательной магистрали
    void stepPneumoSupply(const double& t, const double& dt);

    /// Моделирование приборов управления тормозами
    void stepBrakesControl(const double& t, const double& dt);

    /// Моделирование тормозного оборудования
    void stepBrakesEquipment(const double& t, const double& dt);

    /// Моделирование ЭПТ
    void stepEPB(const double& t, const double& dt);

    void stepTractionControl(const double& t, const double& dt);

    void stepEDT(const double& t, const double& dt);

    void stepEDT2(const double& t, const double& dt);

    void stepSupportEquipment(const double& t, const double& dt);

    /// Моделирование приборов безопасности
    void stepSafetyDevices(const double& t, const double& dt);

    void stepOtherEquipment(const double& t, const double& dt);

    /// Сброс данных в регистратор
    void registrate(const double& t, const double& dt);

    /// Состояние удерживающей катушки БВ
    bool getHoldingCoilState() const;

    void disableEDT() { EDT = allowEDT = false; }

private slots:

    void enableEDT()
    {
        EDT = allowEDT = true;
        EDT_timer.stop();
    }
};

#endif // CHS2T_H
