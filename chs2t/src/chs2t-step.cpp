#include "chs2t.h"

#include    "blinds.h"
#include    "electropneumovalve-emergency.h"
#include    "energy-counter.h"
#include    "generator.h"
#include    "motor.h"
#include    "overload-relay.h"
#include    "pusk-rez.h"
#include    "stepswitch.h"

#include    <motor-fan-dc.h>
#include    <pantograph.h>
#include    <protective-device.h>
#include    <reservoir.h>
#include    <sanding-system.h>
#include    <train-horn.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepPantographs(const double& t, const double& dt)
{
    // Соответствия, какими переключателями в кабинах управляются токоприемники
    struct indexes {
        std::uint8_t pant_idx;
        std::uint8_t cab_idx[CABS_NUM];
        std::uint8_t sw_idx[CABS_NUM];
    };
    constexpr indexes pant_indexes[NUM_PANTOGRAPHS] =
        {{PANT1, {CAB1, CAB2}, {CHS2tSwitchers::PANT_FWD, CHS2tSwitchers::PANT_BWD}},
         {PANT2, {CAB1, CAB2}, {CHS2tSwitchers::PANT_BWD, CHS2tSwitchers::PANT_FWD}}};

    // Управление токоприемниками и их разъединителями
    for (const auto& [pant_idx, cab_idx, sw_idx] : pant_indexes)
    {
        bool pant_off = false;
        bool pant_down = true;
        bool pant_up = false;
        bool pant_on = false;

        for (const auto& cab : {CAB1, CAB2})
        {
            const bool is_off = sw_panel[cab_idx[cab]].isSwitched(sw_idx[cab], CHS2tSwitchers::PANT_GROUND);
            const bool is_down = sw_panel[cab_idx[cab]].isSwitched(sw_idx[cab], CHS2tSwitchers::PANT_DOWN);
            const bool is_up = sw_panel[cab_idx[cab]].isSwitched(sw_idx[cab], CHS2tSwitchers::PANT_UP);
            const bool is_on = sw_panel[cab_idx[cab]].isSwitched(sw_idx[cab], CHS2tSwitchers::PANT_ON);

            pant_off |= is_off;
            pant_down &= is_down;
            pant_up |= is_up;
            pant_on |= is_on;
        }

        if (pant_off)
            pant_switch[pant_idx].reset();

        if (pant_off || pant_down)
            pantup_trigger[pant_idx].reset();

        if (pant_up && pant_switch[pant_idx].getState())
            pantup_trigger[pant_idx].set();

        if (pant_on)
            pant_switch[pant_idx].set();

        pantographs[pant_idx]->setState(pant_switch[pant_idx].getState() && pantup_trigger[pant_idx].getState());
        pantographs[pant_idx]->step(t, dt);
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

    bool fs_off = true;
    bool fs_on = false;
    for (const auto& cab_idx : {CAB1, CAB2})
    {
        const bool is_off = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::FAST_SW, CHS2tSwitchers::FAST_SW_OFF);
        const bool is_on = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::FAST_SW, CHS2tSwitchers::FAST_SW_ON);

        fs_off &= is_off;
        fs_on |= is_on;
    }

    if (fs_on)
    {
        fast_switch_trigger.set();
        bv_return = true;
    }

    if (fs_off)
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

    // Контроллер машиниста
    ControllerState km_state = ControllerState();
    for (size_t cab_idx : {CAB1, CAB2})
    {
        km21KR2[cab_idx].allowChangeReversPos(stepSwitch->isZero());
        km21KR2[cab_idx].step(t, dt);

        // Контакты контроллера из кабины с реверсивкой
        if (km21KR2[cab_idx].isReversHandle())
        {
            km_state = km21KR2[cab_idx].getCtrlState();

            // Из задней кабины контакты реверсивного вала наоборот
            if (cab_idx == CAB2)
            {
                std::swap(km_state.k01, km_state.k02);
            }
        }
    }

    stepSwitch->setDropPosition(dropPosition);
    stepSwitch->setDropButtonState(button_sbros_cpc[CAB1].getState() || button_sbros_cpc[CAB2].getState());
    stepSwitch->setCtrlState(km_state);
    stepSwitch->step(t, dt);

    puskRez->setPoz(stepSwitch->getPoz());
    puskRez->step(t, dt);

    if (EDT || (!(epk[CAB1]->isKeyOn() || epk[CAB2]->isKeyOn())) || (!emergency_valve->isTractionAllow()))
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

    // Управление мотор-вентиляторами ТЭД
    bool fan_off = true;
    bool fan_auto = false;
    bool fan_on = false;

    // Управление жалюзи ПТР
    bool blinds_on = true;
    bool blinds_auto = false;

    for (const auto& cab_idx : {CAB1, CAB2})
    {
        bool is_off = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::FANS, CHS2tSwitchers::FANS_OFF);
        bool is_auto = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::FANS, CHS2tSwitchers::FANS_AUTO);
        bool is_on = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::FANS, CHS2tSwitchers::FANS_ON);

        fan_off &= is_off;
        fan_auto |= is_auto;
        fan_on |= is_on;


        is_on = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::BLINDS, CHS2tSwitchers::BLINDS_OPEN) &&
                (km21KR2[cab_idx].getReversHandlePos() != 0);
        is_auto = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::BLINDS, CHS2tSwitchers::AUTO_BLINDS) ||
                  sw_panel[cab_idx].isSwitched(CHS2tSwitchers::BLINDS, CHS2tSwitchers::AUTO_BLINDS_SAND);

        blinds_on &= is_on;
        blinds_auto |= is_auto;
    }

    if (fan_off)
    {
        motor_fan[0]->setPowerVoltage(0.0);
        motor_fan[1]->setPowerVoltage(0.0);
    }

    if (fan_auto)
    {
        motor_fan[0]->setPowerVoltage((bv->getU_out() / 2.0) * (stepSwitch->getPoz() > 0 || motor_fan[0]->isPowered()));
        motor_fan[1]->setPowerVoltage((bv->getU_out() / 2.0) * (stepSwitch->getPoz() > 0 || motor_fan[1]->isPowered()));
    }

    if (fan_on)
    {
        motor_fan[0]->setPowerVoltage(bv->getU_out() / 2.0);
        motor_fan[1]->setPowerVoltage(bv->getU_out() / 2.0);
    }

    motor_fan[0]->step(t, dt);
    motor_fan[1]->step(t, dt);

    blinds_on |= EDT;
    blinds_on |= blinds_auto && !(hod || stepSwitch->isZero());

    blinds->setState(blinds_on);
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
