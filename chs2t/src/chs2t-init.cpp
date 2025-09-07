#include    "chs2t.h"

#include    <QDir>

//------------------------------------------------------------------------------
// Инициализация токоприемников
//------------------------------------------------------------------------------
void CHS2T::initPantographs(const QString& modules_dir, const QString& custom_cfg_dir)
{
    (void) modules_dir;

    for (size_t i = 0; i < NUM_PANTOGRAPHS; ++i)
    {
        pantographs[i] = new Pantograph();
        pantographs[i]->read_config("pantograph", custom_cfg_dir);
        pantographs[i]->setUks(Uks);
    }
}

//------------------------------------------------------------------------------
// Инициализация быстродействующего выключателя
//------------------------------------------------------------------------------
void CHS2T::initFastSwitch(const QString& modules_dir, const QString& custom_cfg_dir)
{
    (void) modules_dir;

    bv = new ProtectiveDevice();
    bv->read_config("bv", custom_cfg_dir);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::initProtection(const QString& modules_dir, const QString& custom_cfg_dir)
{
    (void) modules_dir;

    overload_relay = new OverloadRelay();
    overload_relay->read_config("1RPD6", custom_cfg_dir);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::initTractionControl(const QString& modules_dir, const QString& custom_cfg_dir)
{
    (void) modules_dir;

    for (auto cab_idx : {CAB1, CAB2})
    {
        km21KR2[cab_idx] = new Km21KR2();
        km21KR2[cab_idx]->setControl(&pressed_keys_by_cabine[cab_idx]);
    }

    stepSwitch = new StepSwitch();
    stepSwitch->read_config("step-switch", custom_cfg_dir);

    motor = new Motor();
    motor->setCustomConfigDir(config_dir);
    motor->read_config("AL-4846dT", custom_cfg_dir);

    puskRez = new PuskRez;
    puskRez->read_config("puskrez", custom_cfg_dir);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::initEDT(const QString& modules_dir, const QString& custom_cfg_dir)
{
    (void) modules_dir;

    generator = new Generator();
    generator->setCustomConfigDir(config_dir);
    generator->read_config("AL-4846dT", custom_cfg_dir);

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
void CHS2T::initOtherEquipment(const QString& modules_dir, const QString& custom_cfg_dir)
{
    (void) modules_dir;

    horn = new TrainHorn();
    horn->read_config("train-horn");

    // Система подачи песка
    sand_system = new SandingSystem();
    sand_system->read_config("sanding-system");
    sand_system->setSandMassMax(payload_mass);
    sand_system->setSandLevel(payload_coeff);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::initSupportEquipment(const QString& modules_dir, const QString& custom_cfg_dir)
{
    (void) modules_dir;

    motor_fan_ptr = new DCMotorFan();
    motor_fan_ptr->read_config("dc-motor-fan", custom_cfg_dir);

    for (size_t i = 0; i < motor_fan.size(); ++i)
    {
        motor_fan[i] = new DCMotorFan();
        motor_fan[i]->read_config("motor-fan", custom_cfg_dir);
    }

    blinds = new Blinds();
    blinds->read_config("blinds", custom_cfg_dir);

    energy_counter = new EnergyCounter();
    energy_counter->read_config("energy-counter", custom_cfg_dir);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::initModbus(const QString& modules_dir, const QString& custom_cfg_dir)
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
void CHS2T::initRegistrator(const QString& modules_dir, const QString& custom_cfg_dir)
{
    (void) modules_dir;
    (void) custom_cfg_dir;
/*
    reg = new Registrator(0.1);
    reg->setFileName("motor");
    reg->init();*/
}
