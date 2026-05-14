#include    <chs2t.h>

#include    "alsn-ukbm.h"
#include    "sl2m.h"

#include    <automatic-train-stop.h>
#include    <speedmap.h>

#include    <QDir>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::initSafetyDevices(const QString& modules_dir, const QString& custom_cfg_dir)
{
    (void) modules_dir;

    // Карта ограничений скорости
    speedmap_fwd = new SpeedMap();
    speedmap_fwd->setDirection(1);
    addRailwayConnector(speedmap_fwd, length / 2.0);

    speedmap_bwd = new SpeedMap();
    speedmap_bwd->setDirection(-1);
    addRailwayConnector(speedmap_bwd, -length / 2.0);

    // Приёмные катушки АЛСН
    coil_ALSN_fwd = new CoilALSN();
    coil_ALSN_fwd->setDirection(1);
    addRailwayConnector(coil_ALSN_fwd, length / 2.0);

    coil_ALSN_bwd = new CoilALSN();
    coil_ALSN_bwd->setDirection(-1);
    addRailwayConnector(coil_ALSN_bwd, -length / 2.0);

    for (size_t cab_idx : {CAB1, CAB2})
    {
        speed_meter[cab_idx] = new SL2M();
        speed_meter[cab_idx]->read_config("3SL-2M", custom_cfg_dir);

        // ЭПК автостопа
        epk[cab_idx] = loadAutoTrainStop(modules_dir + QDir::separator() + "epk150");
        epk[cab_idx]->read_config("epk150");

        // Дешифратор АЛСН
        alsn_decoder[cab_idx] = new DecoderALSN();
        alsn_decoder[cab_idx]->read_config("ALSN-decoder");

        // УКБМ
        safety_device[cab_idx] = new SafetyDevice();
    }
    speed_meter[CAB1]->setWheelDiameter(wheel_diameter[0]);
    speed_meter[CAB2]->setWheelDiameter(wheel_diameter[5]);
}
