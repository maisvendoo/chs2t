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
    void initBrakeDevices(double p0, double pBP, double pFL);

private:

    /// Имя модуля сцепного устройства
    QString coupling_module_name;
    /// Имя конфига сцепного устройства
    QString coupling_config_name;
    /// Имя модуля поездного крана
    QString brake_crane_module_name;
    /// Имя конфига поездного крана
    QString brake_crane_config_name;
    /// Имя модуля локомотивного крана
    QString loco_crane_module_name;
    /// Имя конфига локомотивного крана
    QString loco_crane_config_name;
    /// Имя модуля воздухораспределителя
    QString airdist_module_name;
    /// Имя конфига воздухорапределителя
    QString airdist_config_name;
    /// Имя модуля электровоздухораспределителя
    QString electro_airdist_module_name;
    /// Имя конфига электровоздухорапределителя
    QString electro_airdist_config_name;

    /// Сцепка спереди
    Coupling *coupling_fwd;
    /// Сцепка сзади
    Coupling *coupling_bwd;

    /// Расцепной рычаг спереди
    OperatingRod *oper_rod_fwd;
    /// Расцепной рычаг сзади
    OperatingRod *oper_rod_bwd;

    enum
    {
        NUM_PANTOGRAPHS = 2,
        WIRE_VOLTAGE = 3000
    };

    /// Напряжение аккумуляторной батареи
    double U_bat;

    /// Схема тяги
    Motor *motor;

    /// Токоприемники
    std::array<Pantograph *, NUM_PANTOGRAPHS>    pantographs;

    /// Быстрый выключатель
    ProtectiveDevice *bv;

    /// Пусковой резистор
    PuskRez *puskRez;

    /// Регистратор
    Registrator *reg;

    /// Контроллер машиниста
    Km21KR2 *km21KR2;

    /// Переключатель ступеней
    StepSwitch *stepSwitch;

    double tracForce_kN;

    /// Возврат защиты
    bool bv_return;

    /// Список звуков перестука
    QList<QString> tap_sounds;

    /// Реле перегрузки ТЭД
    OverloadRelay *overload_relay;

    PhysToModbus *TM_manometer;
    PhysToModbus *UR_manometer;
    PhysToModbus *ZT_manometer;
    PhysToModbus *GR_manometer;
    PhysToModbus *TC_manometer;

    /// Тумблер включенияМК
    Trigger     mk_tumbler;

    /// Галетники управления МК
    std::array<Switcher *, 2> mk_switcher;

    /// Мотор-компрессоры (МК)
    std::array<DCMotorCompressor *, 2> motor_compressor;

    /// Регулятор давления ГР
    PressureRegulator *press_reg;

    /// Главный резервуар
    Reservoir   *main_reservoir;

    /// Концевой кран питательной магистрали спереди
    PneumoAngleCock *anglecock_fl_fwd;

    /// Концевой кран питательной магистрали сзади
    PneumoAngleCock *anglecock_fl_bwd;

    /// Рукав питательной  магистрали спереди
    PneumoHose      *hose_fl_fwd;

    /// Рукав питательной  магистрали сзади
    PneumoHose      *hose_fl_bwd;

    /// Поездной кран машиниста усл.№395
    BrakeCrane  *brake_crane;

    /// Кран впомогательного тормоза усл.№254
    LocoCrane   *loco_crane;

    /// Рукоятка задатчика тормозного усилия
    HandleEDT   *handleEDT;


    /// Электропневматический клапан автостопа усл.№150
    AutoTrainStop   *epk;
    /// Ключ ЭПК
    Trigger   key_epk;

    /// Электропневматический вентиль экстренного торможения (при ТМ < 0.3 МПа)
    ElectroPneumoValveEmergency *emergency_valve;

    /// Электропневматический вентиль отпуска пневматических тормозов
    ElectroPneumoValveRelease   *release_valve;

    /// Управляющая камера воздухораспределителя (ложный ТЦ)
    Reservoir   *brake_ref_res;

    /// Тормозная магистраль
    Reservoir   *brakepipe;

    /// Воздухораспределитель
    AirDistributor  *air_dist;

    /// Электровоздухораспределитель
    ElectroAirDistributor  *electro_air_dist;

    /// Запасный резервуар
    Reservoir   *supply_reservoir;

    /// Разветвитель потока воздуха от локомотивного крана к тележкам
    PneumoSplitter  *loco_crane_splitter;

    /// Скоростной клапан ДАКО
    Dako *dako;

    /// Повторительное реле давления усл.№304
    PneumoRelay     *bc_pressure_relay;

    /// Концевой кран тормозной магистрали спереди
    PneumoAngleCock *anglecock_bp_fwd;

    /// Концевой кран тормозной магистрали сзади
    PneumoAngleCock *anglecock_bp_bwd;

    /// Рукав тормозной магистрали спереди
    PneumoHoseEPB   *hose_bp_fwd;

    /// Рукав тормозной магистрали сзади
    PneumoHoseEPB   *hose_bp_bwd;

    enum
    {
        NUM_TROLLEYS = 2,
        NUM_AXIS_PER_TROLLEY = 3,
        TROLLEY_FWD = 0,
        TROLLEY_BWD = 1
    };

    /// Переключательные клапаны ЗПК потока в тормозные цилиндры
    std::array<SwitchingValve *, NUM_TROLLEYS> bc_switch_valve;

    /// Тормозные механизмы тележек
    std::array<BrakeMech *, NUM_TROLLEYS> brake_mech;

    /// Выключатель ЭПТ
    Trigger     epb_switch;

    /// Преобразователь питания ЭПТ
    EPBConverter *epb_converter;

    /// Блок управления ЭПТ
    EPBControl *epb_control;

    DCMotorFan *motor_fan_ptr;

    /// Свисток и тифон
    TrainHorn   *horn;

    /// Система подачи песка
    SandingSystem   *sand_system;

    /// Схема реостатного тормоза
    Generator   *generator;

    /// Импульсный преобразователь возбуждения
    PulseConverter  *pulseConv;

    /// Регулятор тормозного усилия (САРТ)
    BrakeRegulator  *BrakeReg;

    /// Галетники управления токоприемниками
    std::array<Switcher *, NUM_PANTOGRAPHS> pantoSwitcher;

    /// Галетник управления БВ
    Switcher    *fastSwitchSw;

    /// Мотор-вентиляторы
    std::array<DCMotorFan*, 2> motor_fan;

    /// Галетник управления мотор-вентиляторами
    Switcher *motor_fan_switcher;

    /// Галетник управления жалюзи
    Switcher *blindsSwitcher;

    /// Зарядное давление
    double charging_press;

    /// Напряжение на крышевой шине токоприемников
    double U_kr;

    /// Разъединители токоприемников
    std::array<Trigger, NUM_PANTOGRAPHS> pant_switch;

    /// Тригеры поднятия/опускания ТП
    std::array<Trigger, NUM_PANTOGRAPHS> pantup_trigger;

    /// Тригер включения БВ
    Trigger     fast_switch_trigger;

    /// Выключатель ЭДТ
    Trigger     EDTSwitch;

    /// Разрешение тяги
    Trigger     allowTrac;

    bool dropPosition;

    Timer EDT_timer;

    /// Передаточное число тягового редуктора
    double      ip;

    /// Флаг сбора схемы ЭДТ
    bool        EDT;

    /// Флаг разрешения работы ЭДТ
    bool        allowEDT;

    /// Жалюзи пуско-тормозных резисторов
    Blinds      *blinds;

    /// Скоростемер 3СЛ2М
    SL2M        *speed_meter;

    /// Счетчик энергии
    EnergyCounter   *energy_counter;

    /// Устройство безопасности
    SafetyDevice    *safety_device;

    /// Состояния РБ и РБС
    bool state_RB;

    bool state_RBS;

    /// Загрузка данных из конфигурационных файлов
    void loadConfig(QString cfg_path);

    /// Обработка клавиш
    void keyProcess();

    /// Вывод данных на внешние СОИ
    void hardwareOutput();

    /// Сброс данных в регистратор
    void registrate(double t, double dt);

    /// Состояние удерживающей катушки БВ
    bool getHoldingCoilState() const;

    /// Общая инициализация локомотива
    void initialization();

    /// Инициализация сцепных устройств
    void initCouplings(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициализация токоприемников
    void initPantographs(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициадизация рычажки
    void initBrakesMech(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициализация БВ
    void initFastSwitch(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициализация защит
    void initProtection(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициализация питательной магистрали
    void initPneumoSupply(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициализация приборов управления тормозами
    void initBrakesControl(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициализация тормозного оборудования
    void initBrakesEquipment(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициализация ЭПТ
    void initEPB(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициализация схемы управления тягой
    void initTractionControl(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициализация ЭДТ
    void initEDT(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициализация вспомогательного оборудования
    void initSupportEquipment(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициализация прочего оборудования
    void initOtherEquipment(const QString &modules_dir, const QString &custom_cfg_dir);

    ///
    void initModbus(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Инициализация регистратора
    void initRegistrator(const QString &modules_dir, const QString &custom_cfg_dir);

    /// Подпрограмма изменения положения пакетника
    void setSwitcherState(Switcher *sw, signal_t signal);

    /// Предварительные расчёты перед симуляцией
    void preStep(double t);

    /// Предварительный расчёт координат сцепных устройств
    void preStepCouplings(double t);

    /// Шаг моделирования всех систем локомотива в целом
    void step(double t, double dt);

    /// Моделирование сцепных устройств
    void stepCouplings(double t, double dt);

    /// Моделирование работы токоприемников
    void stepPantographs(double t, double dt);

    void stepFastSwitch(double t, double dt);

    void stepProtection(double t, double dt);

    /// Моделирование питательной магистрали
    void stepPneumoSupply(double t, double dt);

    /// Моделирование приборов управления тормозами
    void stepBrakesControl(double t, double dt);

    /// Моделирование тормозного оборудования
    void stepBrakesEquipment(double t, double dt);

    /// Моделирование ЭПТ
    void stepEPB(double t, double dt);

    void stepTractionControl(double t, double dt);

    void stepEDT(double t, double dt);

    void stepEDT2(double t, double dt);

    void stepSupportEquipment(double t, double dt);

    void stepOtherEquipment(double t, double dt);

    void stepDebugMsg(double t, double dt);

    void stepSignals();

    /// Вывод сигналов звучки
    void stepSoundSignalsOutput(double t, double dt);
    float motorSoundSignal(double t, double dt);

    void stepSwitcherPanel();

    void stepDecodeAlsn();

    void disableEDT() { EDT = allowEDT = false; }

private slots:

    void enableEDT()
    {
        EDT = allowEDT = true;
        EDT_timer.stop();
    }
};

#endif // CHS2T_H
