#include "chs2t.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepPantographs(double t, double dt)
{
    // Управление разъединителями токоприемников
    for (size_t i = 0; i < NUM_PANTOGRAPHS; ++i)
    {
        pantoSwitcher[i]->setControl(keys);

        if (pantoSwitcher[i]->getPosition() == 3)
            pant_switch[i].set();

        if (pantoSwitcher[i]->getPosition() == 0)
            pant_switch[i].reset();

        if (pantoSwitcher[i]->getPosition() == 2 && pant_switch[i].getState())
            pantup_trigger[i].set();

        if (pantoSwitcher[i]->getPosition() == 1)
            pantup_trigger[i].reset();

        pantoSwitcher[i]->step(t, dt);

        // Подъем/опускание ТП
        pantographs[i]->setState(pant_switch[i].getState() && pantup_trigger[i].getState());

        pantographs[i]->step(t, dt);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepFastSwitch(double t, double dt)
{
    bv->setHoldingCoilState(getHoldingCoilState());
    bv_return = getHoldingCoilState() && bv_return;
    bv->setReturn(bv_return);

    U_kr = max(pantographs[0]->getUout() * pant_switch[0].getState() ,
            pantographs[1]->getUout() * pant_switch[1].getState());

    bv->setU_in(U_kr);

    bv->setState(fast_switch_trigger.getState());
    bv->step(t, dt);

    if (fastSwitchSw->getPosition() == 3)
    {
        fast_switch_trigger.set();
        bv_return = true;
    }

    if (fastSwitchSw->getPosition() == 1)
    {
        fast_switch_trigger.reset();
        bv_return = false;
    }

    fastSwitchSw->setControl(keys);
    fastSwitchSw->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepProtection(double t, double dt)
{
    overload_relay->setCurrent(motor->getIa());
    overload_relay->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepTractionControl(double t, double dt)
{
    ip = 1.75;

    km21KR2->setHod(stepSwitch->getHod());
    km21KR2->setControl(keys, control_signals);
    km21KR2->step(t, dt);

    stepSwitch->setDropPosition(dropPosition);
    stepSwitch->setCtrlState(km21KR2->getCtrlState());
    stepSwitch->setControl(keys);
    stepSwitch->step(t, dt);

    puskRez->setPoz(stepSwitch->getPoz());
    puskRez->step(t, dt);

    if (EDT || (!epk->isKeyOn()) || (!emergency_valve->isTractionAllow()))
    {
        allowTrac.reset();
    }
    else
    {
        if (stepSwitch->getPoz() == 0)
            allowTrac.set();
    }

    motor->setDirection(stepSwitch->getReverseState());
    motor->setBetaStep(stepSwitch->getFieldStep());
    motor->setPoz(stepSwitch->getPoz());
    motor->setR(puskRez->getR());
    motor->setU(bv->getU_out() * stepSwitch->getSchemeState() * static_cast<double>(allowTrac.getState()));
    motor->setOmega(wheel_omega[0] * ip);
    motor->setAmpermetersState(stepSwitch->getAmpermetersState());
    motor->step(t, dt);

    for (size_t i = 1; i < Q_a.size(); ++i)
    {
        Q_a[i] = (motor->getTorque() + generator->getTorque()) * ip;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepSupportEquipment(double t, double dt)
{
    double R = 0.6;
    bool hod = stepSwitch->getHod();

    // Мотор-вентилятор ПТР
    motor_fan_ptr->setPowerVoltage(R * (motor->getIa() * !hod + abs(generator->getIa())));
    motor_fan_ptr->step(t, dt);

    motor_fan_switcher->setControl(keys);

    if (motor_fan_switcher->getPosition() == 0)
    {
        motor_fan[0]->setPowerVoltage(0.0);
        motor_fan[1]->setPowerVoltage(0.0);
    }

    if (motor_fan_switcher->getPosition() == 1)
    {
        motor_fan[0]->setPowerVoltage((bv->getU_out() / 2.0) * (stepSwitch->getPoz() > 0 || motor_fan[0]->isPowered()));
        motor_fan[1]->setPowerVoltage((bv->getU_out() / 2.0) * (stepSwitch->getPoz() > 0 || motor_fan[1]->isPowered()));
    }

    if (motor_fan_switcher->getPosition() == 2)
    {
        motor_fan[0]->setPowerVoltage(bv->getU_out() / 2.0);
        motor_fan[1]->setPowerVoltage(bv->getU_out() / 2.0);
    }

    motor_fan_switcher->step(t, dt);
    motor_fan[0]->step(t, dt);
    motor_fan[1]->step(t, dt);

    blindsSwitcher->setControl(keys);

    if (blindsSwitcher->getPosition() == 0 || blindsSwitcher->getPosition() == 1)
    {
        blinds->setState(false);
    }

    if (blindsSwitcher->getPosition() == 2)
    {
        blinds->setState(true);
    }

    if (blindsSwitcher->getPosition() == 3 || blindsSwitcher->getPosition() == 4)
    {
        blinds->setState((!hod && !stepSwitch->isZero()) || EDT);
    }

    blindsSwitcher->step(t, dt);
    blinds->step(t, dt);

    energy_counter->setFullPower(Uks * (motor->getI12() + motor->getI34() + motor->getI56()) );
    energy_counter->setResistorsPower( puskRez->getR() * ( pow(motor->getI12(), 2) + pow(motor->getI34(), 2) + pow(motor->getI56(), 2) ) );
    energy_counter->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepOtherEquipment(double t, double dt)
{
    horn->setFLpressure(main_reservoir->getPressure());
    horn->setControl(keys, control_signals);
    horn->step(t, dt);

    // Система подачи песка
    sand_system->setFLpressure(main_reservoir->getPressure());
    sand_system->setControl(keys);
    sand_system->step(t, dt);
    for (size_t i = 0; i < num_axis; ++i)
    {
        // Пересчёт трения колесо-рельс
        psi[i] = sand_system->getWheelRailFrictionCoeff(psi[i]);
    }
    // Пересчёт массы локомотива
    payload_coeff = sand_system->getSandLevel();
    setPayloadCoeff(payload_coeff);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
bool CHS2T::getHoldingCoilState() const
{
    bool no_overload = (!static_cast<bool>(overload_relay->getState()));

    return no_overload;
}
