#include    "chs2t.h"

#include    <QDir>

//------------------------------------------------------------------------------
// Инициализация токоприемников
//------------------------------------------------------------------------------
void CHS2T::initPantographs(const QString &modules_dir, const QString &custom_cfg_dir)
{
    (void) modules_dir;

    for (size_t i = 0; i < NUM_PANTOGRAPHS; ++i)
    {
        pantographs[i] = new Pantograph();
        pantographs[i]->read_config("pantograph", custom_cfg_dir);
        pantographs[i]->setUks(Uks);
    }

    for (size_t i = 0; i < NUM_PANTOGRAPHS; ++i)
    {
        pantoSwitcher[i] = new Switcher(Q_NULLPTR, 0, 4);
        pantoSwitcher[i]->setSpringLast();
    }

    pantoSwitcher[0]->setKeyCode(KEY_I);
    pantoSwitcher[1]->setKeyCode(KEY_O);
}

//------------------------------------------------------------------------------
// Инициализация быстродействующего выключателя
//------------------------------------------------------------------------------
void CHS2T::initFastSwitch(const QString &modules_dir, const QString &custom_cfg_dir)
{
    (void) modules_dir;

    bv = new ProtectiveDevice();
    bv->read_config("bv", custom_cfg_dir);

    fastSwitchSw = new Switcher(Q_NULLPTR, KEY_P, 4);
    fastSwitchSw->setSpringLast();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::initProtection(const QString &modules_dir, const QString &custom_cfg_dir)
{
    (void) modules_dir;

    overload_relay = new OverloadRelay();
    overload_relay->read_config("1RPD6", custom_cfg_dir);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::initTractionControl(const QString &modules_dir, const QString &custom_cfg_dir)
{
    (void) modules_dir;

    km21KR2 = new Km21KR2();
    connect(km21KR2, &Km21KR2::soundPlay, this, &CHS2T::soundPlay);

    stepSwitch = new StepSwitch();
    stepSwitch->read_config("step-switch", custom_cfg_dir);

    motor = new Motor();
    motor->setCustomConfigDir(config_dir);
    motor->read_config("AL-4846dT", custom_cfg_dir);
    connect(motor, &Motor::soundSetVolume, this, &CHS2T::soundSetVolume);
    connect(motor, &Motor::soundSetPitch, this, &CHS2T::soundSetPitch);

    puskRez = new PuskRez;
    puskRez->read_config("puskrez", custom_cfg_dir);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::initEDT(const QString &modules_dir, const QString &custom_cfg_dir)
{
    (void) modules_dir;

    generator = new Generator();
    generator->setCustomConfigDir(config_dir);
    generator->read_config("AL-4846dT", custom_cfg_dir);
    connect(generator, &Generator::soundSetPitch, this, &CHS2T::soundSetPitch);
    connect(generator, &Generator::soundSetVolume, this, &CHS2T::soundSetVolume);

    pulseConv = new PulseConverter();

    BrakeReg = new BrakeRegulator();
    BrakeReg->read_config("brake-regulator", custom_cfg_dir);

    EDT_timer.setTimeout(3.0);
    EDT_timer.firstProcess(false);
    connect(&EDT_timer, &Timer::process, this, &CHS2T::enableEDT);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::initOtherEquipment(const QString &modules_dir, const QString &custom_cfg_dir)
{
    (void) modules_dir;

    horn = new TrainHorn();
    horn->read_config("train-horn");

    // Система подачи песка
    sand_system = new SandingSystem();
    sand_system->read_config("sanding-system");
    sand_system->setSandMassMax(payload_mass);
    sand_system->setSandLevel(payload_coeff);

    speed_meter = new SL2M();
    speed_meter->read_config("3SL-2M", custom_cfg_dir);

    safety_device = new SafetyDevice();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::initSupportEquipment(const QString &modules_dir, const QString &custom_cfg_dir)
{
    (void) modules_dir;

    motor_fan_ptr = new DCMotorFan();
    motor_fan_ptr->read_config("dc-motor-fan", custom_cfg_dir);

    for (size_t i = 0; i < motor_fan.size(); ++i)
    {
        motor_fan[i] = new DCMotorFan();
        motor_fan[i]->read_config("motor-fan", custom_cfg_dir);
    }

    motor_fan_switcher = new Switcher(Q_NULLPTR, KEY_F, 3);

    blinds = new Blinds();
    blinds->read_config("blinds", custom_cfg_dir);

    blindsSwitcher = new Switcher(Q_NULLPTR, KEY_G, 5);

    energy_counter = new EnergyCounter();
    energy_counter->read_config("energy-counter", custom_cfg_dir);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::initModbus(const QString &modules_dir, const QString &custom_cfg_dir)
{
    (void) modules_dir;

    QString modbusCfgDir = custom_cfg_dir + QDir::separator() + "modbus";

    TM_manometer = new PhysToModbus();
    TM_manometer->load((modbusCfgDir + QDir::separator() + "manometer-TM").toStdString());

    UR_manometer = new PhysToModbus();
    UR_manometer->load((modbusCfgDir + QDir::separator() + "manometer-UR").toStdString());

    ZT_manometer = new PhysToModbus();
    ZT_manometer->load((modbusCfgDir + QDir::separator() + "manometer-ZT").toStdString());

    GR_manometer = new PhysToModbus();
    GR_manometer->load((modbusCfgDir + QDir::separator() + "manometer-GR").toStdString());

    TC_manometer = new PhysToModbus();
    TC_manometer->load((modbusCfgDir + QDir::separator() + "manometer-TC").toStdString());

}

//------------------------------------------------------------------------------
// Инициализация регистратора
//------------------------------------------------------------------------------
void CHS2T::initRegistrator(const QString &modules_dir, const QString &custom_cfg_dir)
{
    (void) modules_dir;
    (void) custom_cfg_dir;

    reg = nullptr;

    reg = new Registrator(0.1);
    reg->setFileName("motor");
    reg->init();

}

//------------------------------------------------------------------------------
// Инициализация списка звуков перестука
//------------------------------------------------------------------------------
void CHS2T::initTapSounds()
{
    QString f_p = "tap_";

    tap_sounds << (f_p + "5-10");
    tap_sounds << (f_p + "10-20");
    tap_sounds << (f_p + "20-30");
    tap_sounds << (f_p + "30-40");
    tap_sounds << (f_p + "40-50");
    tap_sounds << (f_p + "50-60");
    tap_sounds << (f_p + "60-70");
    tap_sounds << (f_p + "70-80");
    tap_sounds << (f_p + "80-90");
    tap_sounds << (f_p + "90-100");
    tap_sounds << (f_p + "100-110");
    tap_sounds << (f_p + "110-~");
}
