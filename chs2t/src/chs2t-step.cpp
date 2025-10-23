#include "chs2t.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepPantographs(const double& t, const double& dt)
{
    // Управление разъединителями токоприемников
    for (size_t i = 0; i < NUM_PANTOGRAPHS; ++i)
    {
        if (pant_switcher[CAB1][i].getPosition() == 3)
            pant_switch[i].set();

        if (pant_switcher[CAB1][i].getPosition() == 0)
            pant_switch[i].reset();

        if (pant_switcher[CAB1][i].getPosition() == 2 && pant_switch[i].getState())
            pantup_trigger[i].set();

        if (pant_switcher[CAB1][i].getPosition() == 1)
            pantup_trigger[i].reset();

        // Подъем/опускание ТП
        pantographs[i]->setState(pant_switch[i].getState() && pantup_trigger[i].getState());

        pantographs[i]->step(t, dt);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepFastSwitch(const double& t, const double& dt)
{
    bv->setHoldingCoilState(getHoldingCoilState());
    bv_return = getHoldingCoilState() && bv_return;
    bv->setReturn(bv_return);

    U_kr = max(pantographs[0]->getUout() * pant_switch[0].getState() ,
            pantographs[1]->getUout() * pant_switch[1].getState());

    bv->setU_in(U_kr);

    bv->setState(fast_switch_trigger.getState());
    bv->step(t, dt);

    if (fastswitch_switcher->getPosition() == 3)
    {
        fast_switch_trigger.set();
        bv_return = true;
    }

    if (fastswitch_switcher->getPosition() == 1)
    {
        fast_switch_trigger.reset();
        bv_return = false;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepProtection(const double& t, const double& dt)
{
    overload_relay->setCurrent(motor->getIa());
    overload_relay->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepTractionControl(const double& t, const double& dt)
{
    ip = 1.75;

    km21KR2[CAB1]->setHod(stepSwitch->getHod());
    km21KR2[CAB1]->step(t, dt);

    stepSwitch->setDropPosition(dropPosition);
    stepSwitch->setDropButtonState(button_sbros_cpc[CAB1].getState());
    stepSwitch->setCtrlState(km21KR2[CAB1]->getCtrlState());
    stepSwitch->step(t, dt);

    puskRez->setPoz(stepSwitch->getPoz());
    puskRez->step(t, dt);

    if (EDT || (!epk[CAB1]->isKeyOn()) || (!emergency_valve->isTractionAllow()))
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
void CHS2T::stepSupportEquipment(const double& t, const double& dt)
{
    double R = 0.6;
    bool hod = stepSwitch->getHod();

    // Мотор-вентилятор ПТР
    motor_fan_ptr->setPowerVoltage(R * (motor->getIa() * !hod + abs(generator->getIa())));
    motor_fan_ptr->step(t, dt);

    if (motor_fan_switcher[CAB1].getPosition() == 0)
    {
        motor_fan[0]->setPowerVoltage(0.0);
        motor_fan[1]->setPowerVoltage(0.0);
    }

    if (motor_fan_switcher[CAB1].getPosition() == 1)
    {
        motor_fan[0]->setPowerVoltage((bv->getU_out() / 2.0) * (stepSwitch->getPoz() > 0 || motor_fan[0]->isPowered()));
        motor_fan[1]->setPowerVoltage((bv->getU_out() / 2.0) * (stepSwitch->getPoz() > 0 || motor_fan[1]->isPowered()));
    }

    if (motor_fan_switcher[CAB1].getPosition() == 2)
    {
        motor_fan[0]->setPowerVoltage(bv->getU_out() / 2.0);
        motor_fan[1]->setPowerVoltage(bv->getU_out() / 2.0);
    }

    motor_fan[0]->step(t, dt);
    motor_fan[1]->step(t, dt);

    if (blinds_switcher[CAB1].getPosition() == 0 || blinds_switcher[CAB1].getPosition() == 1)
    {
        blinds->setState(false);
    }

    if (blinds_switcher[CAB1].getPosition() == 2)
    {
        blinds->setState(true);
    }

    if (blinds_switcher[CAB1].getPosition() == 3 || blinds_switcher[CAB1].getPosition() == 4)
    {
        blinds->setState((!hod && !stepSwitch->isZero()) || EDT);
    }

    blinds->step(t, dt);

    energy_counter->setFullPower(Uks * (motor->getI12() + motor->getI34() + motor->getI56()) );
    energy_counter->setResistorsPower( puskRez->getR() * ( pow(motor->getI12(), 2) + pow(motor->getI34(), 2) + pow(motor->getI56(), 2) ) );
    energy_counter->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepOtherEquipment(const double& t, const double& dt)
{
    horn[CAB1]->setFLpressure(main_reservoir->getPressure());
    horn[CAB1]->step(t, dt);
    horn[CAB2]->setFLpressure(main_reservoir->getPressure());
    horn[CAB2]->step(t, dt);

    // Система подачи песка
    sand_system->setFLpressure(main_reservoir->getPressure());
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
