#include    "chs2t.h"

#include    "dako.h"
#include    "electropneumovalve-emergency.h"
#include    "electropneumovalve-release.h"
#include    "handle-edt.h"

#include    <pneumo-shutoff-valve.h>
#include    <pneumo-splitter.h>
#include    <pneumo-switching-valve.h>
#include    <reservoir.h>

#include    <QDir>

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
void CHS2T::initBrakesControl(const QString& modules_dir, const QString& custom_cfg_dir)
{
    for (size_t cab_idx : {CAB1, CAB2})
    {
        // Разобщительный кран питательной магистрали к кранам машиниста
        shutoff_crane[cab_idx] = new PneumoShutoffValve();
        shutoff_crane[cab_idx]->read_config("pneumo-shutoff-valve");

        // Комбинированный кран усл.№114
        combine_crane[cab_idx] = new PneumoCombineCrane();
        combine_crane[cab_idx]->read_config("pneumo-combine-crane");

        // Поездной кран машиниста
        brake_crane[cab_idx] = loadBrakeCrane(
            modules_dir + QDir::separator() + brake_crane_module_name);
        brake_crane[cab_idx]->read_config(brake_crane_config_name);

        // Кран вспомогательного тормоза
        loco_crane[cab_idx] = loadLocoCrane(
            modules_dir + QDir::separator() + loco_crane_module_name);
        loco_crane[cab_idx]->read_config(loco_crane_config_name);

        // Рукоятка задатчика тормозного усилия
        handleEDT[cab_idx] = new HandleEDT();
        handleEDT[cab_idx]->read_config("handle-edt", custom_cfg_dir);
        handleEDT[cab_idx]->setBrakeKey(KEY_Period);
        handleEDT[cab_idx]->setReleaseKey(KEY_Comma);
    }

    // Электропневматический вентиль экстренного торможения
    emergency_valve = new ElectroPneumoValveEmergency();
    emergency_valve->read_config("valve-emergency", custom_cfg_dir);

    // Электропневматический вентиль отпуска тормозов
    release_valve = new ElectroPneumoValveRelease();
    release_valve->read_config("valve-release", custom_cfg_dir);

    // Управляющая камера воздухораспределителя (ложный ТЦ)
    brake_ref_res = new Reservoir(0.01);

    // Переключательный клапан потоков от локомотивных кранов в кабинах
    loco_crane_switch_valve = new SwitchingValve();
    loco_crane_switch_valve->read_config("zpk");

    // Разветвитель потока воздуха от локомотивного крана к тележкам
    loco_crane_splitter = new PneumoSplitter();
    loco_crane_splitter->read_config("pneumo-splitter");

    // Скоростной клапан ДАКО
    dako = new Dako();
    dako->setWheelRadius(rk[5]);
    dako->read_config("dako", custom_cfg_dir);

    // Переключательный клапан магистрали тормозных цилиндров
    bc_switch_valve[TROLLEY_FWD] = new SwitchingValve();
//    bc_switch_valve[TROLLEY_FWD]->read_config("zpk", custom_cfg_dir);
    bc_switch_valve[TROLLEY_FWD]->read_config("zpk");
    bc_switch_valve[TROLLEY_BWD] = new SwitchingValve();
//    bc_switch_valve[TROLLEY_BWD]->read_config("zpk", custom_cfg_dir);
    bc_switch_valve[TROLLEY_BWD]->read_config("zpk");

    // Повторительное реле давления
    bc_pressure_relay = new PneumoRelay();
    bc_pressure_relay->read_config("rd304");
}
