#include    "chs2t.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepSafetyDevices(double t, double dt)
{
    // Приёмные катушки АЛСН
    coil_ALSN_fwd->step(t, dt);
    coil_ALSN_bwd->step(t, dt);

    // Дешифратор АЛСН
    alsn_decoder->setCoilSignal(coil_ALSN_fwd->getCode());
    alsn_decoder->step(t, dt);

    // Скоростемер
    speed_meter->setOmega(wheel_omega[0]);
    speed_meter->step(t, dt);

    // УКБМ
    safety_device->setAlsnCode(alsn_decoder->getCode());
    safety_device->setRBstate(state_RB);
    safety_device->setRBSstate(state_RBS);
    safety_device->setKeyEPK(epk->isKeyOn());
    safety_device->setVelocity(speed_meter->getVelocity());
    safety_device->step(t, dt);

    // Электропневматический клапан автостопа
    epk->setFLpressure(main_reservoir->getPressure());
    epk->setBPpressure(brakepipe->getPressure());
    epk->setPowered(safety_device->getEPKstate());
    epk->setKeyOn(key_epk.getState());
    epk->step(t, dt);
}
