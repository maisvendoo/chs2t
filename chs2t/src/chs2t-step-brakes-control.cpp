#include    "chs2t.h"

#include    "dako.h"
#include    "electropneumovalve-emergency.h"
#include    "electropneumovalve-release.h"
#include    "generator.h"
#include    "handle-edt.h"

#include    <pneumo-shutoff-valve.h>
#include    <pneumo-splitter.h>
#include    <pneumo-switching-valve.h>
#include    <reservoir.h>

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
void CHS2T::stepBrakesControl(const double& t, const double& dt)
{
    for (size_t cab_idx : {CAB1, CAB2})
    {
        // Разобщительный кран питательной магистрали к кранам машиниста
        shutoff_crane[cab_idx]->setPipePressure(main_reservoir->getPressure());
        shutoff_crane[cab_idx]->setDeviceFlow(brake_crane[cab_idx]->getFLflow() + loco_crane[cab_idx]->getFLflow());
        shutoff_crane[cab_idx]->step(t, dt);

        // Комбинированный кран усл.№114
        combine_crane[cab_idx]->setBPpressure(brakepipe->getPressure());
        combine_crane[cab_idx]->setCraneBPflow(brake_crane[cab_idx]->getBPflow());
        combine_crane[cab_idx]->step(t, dt);

        // Поездной кран машиниста
        brake_crane[cab_idx]->setFLpressure(shutoff_crane[cab_idx]->getPressureToDevice());
        brake_crane[cab_idx]->setBPpressure(combine_crane[cab_idx]->getCraneBPpressure());
        brake_crane[cab_idx]->step(t, dt);

        // Кран вспомогательного тормоза
        loco_crane[cab_idx]->setFLpressure(shutoff_crane[cab_idx]->getPressureToDevice());
        loco_crane[cab_idx]->setBCpressure((cab_idx == CAB1) ?
                                               loco_crane_switch_valve->getPressure1() :
                                               loco_crane_switch_valve->getPressure2());
        loco_crane[cab_idx]->setILpressure(0.0);
        loco_crane[cab_idx]->step(t, dt);

        // Рукоятка задатчика тормозного усилия
        handleEDT[cab_idx]->step(t, dt);
    }

    // Электропневматический вентиль экстренного торможения
    emergency_valve->setFLpressure(main_reservoir->getPressure());
    emergency_valve->setBPpressure(brakepipe->getPressure());
    emergency_valve->step(t, dt);

    // Электропневматический вентиль отпуска тормозов
    release_valve->setEDTcurrent(generator->getIa());
    release_valve->step(t, dt);

    // Управляющая камера воздухораспределителя (ложный ТЦ)
    brake_ref_res->setFlow(electro_air_dist->getBCflow());
    brake_ref_res->step(t, dt);

    // Переключательный клапан потоков от локомотивных кранов в кабинах
    loco_crane_switch_valve->setInputFlow1(loco_crane[CAB1]->getBCflow());
    loco_crane_switch_valve->setInputFlow2(loco_crane[CAB2]->getBCflow());
    loco_crane_switch_valve->setOutputPressure(loco_crane_splitter->getInputPressure());
    loco_crane_switch_valve->step(t, dt);

    // Разветвитель потока воздуха от локомотивного крана к тележкам
    loco_crane_splitter->setInputFlow(loco_crane_switch_valve->getOutputFlow());
    loco_crane_splitter->setPipePressure1(bc_switch_valve[TROLLEY_FWD]->getPressure1());
    loco_crane_splitter->setPipePressure2(bc_switch_valve[TROLLEY_BWD]->getPressure1());
    loco_crane_splitter->step(t, dt);

    // Скоростной клапан ДАКО
    double no_release = static_cast<double>(!release_valve->isPneumoBrakesRelease());
    dako->setAngularVelocity1(wheel_omega[0]);
    dako->setAngularVelocity6(wheel_omega[5]);
    dako->setFLpressure(main_reservoir->getPressure());
    dako->setBCpressure(bc_switch_valve[TROLLEY_BWD]->getPressure2());
    dako->setLocoCranePressure(max(loco_crane_splitter->getInputPressure(),
                                   emergency_valve->getAdditionalPressure()) );
    dako->setAirDistPressure(no_release * brake_ref_res->getPressure());
    dako->step(t, dt);

    // Повторительное реле давления №304
    // Подаёт в переключательный клапан передней тележки такое же давление,
    // какое установилось в клапане задней тележки от ДАКО и воздухораспределителя
    bc_pressure_relay->setFLpressure(main_reservoir->getPressure());
    bc_pressure_relay->setControlPressure(bc_switch_valve[TROLLEY_BWD]->getPressure2());
    bc_pressure_relay->setPipePressure(bc_switch_valve[TROLLEY_FWD]->getPressure2());
    bc_pressure_relay->step(t, dt);

    // Переключательные клапаны ЗПК потока в тормозные цилиндры
    // Первые входы клапана подключены к крану локомотивного тормоза
    // Вторые входы клапана подключены к ДАКО и воздухораспределителю
    // (у передней тележки через повторительное реле)
    // Выходы клапана подключены к ТЦ тележек
    bc_switch_valve[TROLLEY_FWD]->setInputFlow1(loco_crane_splitter->getPipeFlow1());
    bc_switch_valve[TROLLEY_FWD]->setInputFlow2(bc_pressure_relay->getPipeFlow());
    bc_switch_valve[TROLLEY_FWD]->setOutputPressure(brake_mech[TROLLEY_FWD]->getBCpressure());
    bc_switch_valve[TROLLEY_FWD]->step(t, dt);
    bc_switch_valve[TROLLEY_BWD]->setInputFlow1(loco_crane_splitter->getPipeFlow2());
    bc_switch_valve[TROLLEY_BWD]->setInputFlow2(dako->getBCflow());
    bc_switch_valve[TROLLEY_BWD]->setOutputPressure(brake_mech[TROLLEY_BWD]->getBCpressure());
    bc_switch_valve[TROLLEY_BWD]->step(t, dt);
}
