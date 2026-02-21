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

#include    "chs2t.h"
#include    "chs2t-signals.h"

#include    "filesystem.h"

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
CHS2T::CHS2T() : Vehicle()
{
    analogSignal.resize(SIGNALS_NUM_TOTAL);

    pressed_keys_by_cabine.resize(CABS_NUM);
    pressed_keys_by_cabine.shrink_to_fit();

    Uks = WIRE_VOLTAGE;
    current_kind = 1;
}

//------------------------------------------------------------------------------
// Деструктор
//------------------------------------------------------------------------------
CHS2T::~CHS2T()
{

}

//------------------------------------------------------------------------------
// Общая инициализация локомотива
//------------------------------------------------------------------------------
void CHS2T::initialization()
{
    FileSystem &fs = FileSystem::getInstance();
    QString modules_dir = QString(fs.getModulesDir().c_str());
    QString custom_cfg_dir(fs.getVehiclesDir().c_str());
    custom_cfg_dir += fs.separator() + config_dir;

    initCouplings(modules_dir, custom_cfg_dir);

    initPantographs(modules_dir, custom_cfg_dir);

    initFastSwitch(modules_dir, custom_cfg_dir);

    initProtection(modules_dir, custom_cfg_dir);

    initPneumoSupply(modules_dir, custom_cfg_dir);

    initBrakesControl(modules_dir, custom_cfg_dir);

    initBrakesEquipment(modules_dir, custom_cfg_dir);

    initEPB(modules_dir, custom_cfg_dir);

    initTractionControl(modules_dir, custom_cfg_dir);

    initEDT(modules_dir, custom_cfg_dir);

    initSupportEquipment(modules_dir, custom_cfg_dir);

    initSafetyDevices(modules_dir, custom_cfg_dir);

    initOtherEquipment(modules_dir, custom_cfg_dir);

    initControl(modules_dir, custom_cfg_dir);

    initModbus(modules_dir, custom_cfg_dir);

    initRegistrator(modules_dir, custom_cfg_dir);

    for (size_t i = SWP1_POWER_1; i <= SWP1_POWER_10; ++i)
        feedback_signals.analogSignal[i].cur_value = 1;

    for (size_t i = SWP2_POWER_1; i <= SWP2_POWER_10; ++i)
        feedback_signals.analogSignal[i].cur_value = 1;

    connect(autoStartTimer, &Timer::process, this, &CHS2T::slotAutostart);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::process(const simulator_time_t& t, const double& dt)
{
    if (needDebugMsg)
        debugPrint(t, dt);

    keyProcess(t, dt);

    signalsOutput(t, dt);

    soundsOutput(t, dt);

    hardwareOutput();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::preStep(const double& t)
{
    preStepCouplings(t);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::step(const double& t, const double& dt)
{
    (void) t;
    (void) dt;

    stepCouplings(t, dt);

    stepPantographs(t, dt);

    stepFastSwitch(t, dt);

    stepTractionControl(t, dt);

    stepProtection(t, dt);

    stepPneumoSupply(t, dt);

    stepBrakesControl(t, dt);

    stepEPB(t, dt);

    stepBrakesEquipment(t, dt);

    stepEDT(t, dt);

    stepSupportEquipment(t, dt);

    stepOtherEquipment(t, dt);

    stepSafetyDevices(t, dt);

    registrate(t, dt);

    autoStartTimer->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::disableEDT()
{
    EDT = allowEDT = false;
    if (EDT_timer.isStarted())
    {
        EDT_timer.stop();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::enableEDT()
{
    EDT = allowEDT = true;
    EDT_timer.stop();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::hardwareOutput()
{
    feedback_signals.analogSignal[0].cur_value = TM_manometer->getModbus(brakepipe->getPressure());
    feedback_signals.analogSignal[1].cur_value = UR_manometer->getModbus(brake_crane[CAB1]->getERpressure());
    feedback_signals.analogSignal[2].cur_value = ZT_manometer->getModbus(brake_ref_res->getPressure());
    feedback_signals.analogSignal[3].cur_value = GR_manometer->getModbus(main_reservoir->getPressure());
    feedback_signals.analogSignal[4].cur_value = TC_manometer->getModbus(brake_mech[0]->getBCpressure());
}

GET_VEHICLE(CHS2T)
