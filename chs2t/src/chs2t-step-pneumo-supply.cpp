#include    "chs2t.h"

#include    "dako.h"

#include    <sanding-system.h>
#include    <train-horn.h>

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
void CHS2T::stepPneumoSupply(const double& t, const double& dt)
{
    // Регулятор давления
    press_reg->setFLpressure(main_reservoir->getPressure());
    press_reg->step(t, dt);

    double FL_flow = 0.0;

    // Соответствия, какими переключателями в кабинах управляются мотор-компрессоры
    struct indexes {
        std::uint8_t compr_idx;
        std::uint8_t cab_idx[CABS_NUM];
        std::uint8_t sw_idx[CABS_NUM];
    };
    constexpr indexes compr_indexes[NUM_MOTOR_KOMPRESSORS] =
        {{MK1, {CAB1, CAB2}, {CHS2tSwitchers::COMPR_1, CHS2tSwitchers::COMPR_1}},
         {MK2, {CAB1, CAB2}, {CHS2tSwitchers::COMPR_2, CHS2tSwitchers::COMPR_2}}};

    // Мотор-компрессоры
    for (const auto& [compr_idx, cab_idx, sw_idx] : compr_indexes)
    {
        bool compr_on = false;

        for (const auto& cab : {CAB1, CAB2})
        {
            const bool is_on = sw_panel[cab_idx[cab]].isSwitched(sw_idx[cab], CHS2tSwitchers::COMPR_ON);
            const bool is_auto = sw_panel[cab_idx[cab]].isSwitched(sw_idx[cab], CHS2tSwitchers::COMPR_AUTO);

            compr_on |= is_on;
            compr_on |= (is_auto && press_reg->getState());
        }
        double U_power = compr_on ? bv->getU_out() : 0.0;

        motor_compressor[compr_idx]->setFLpressure(main_reservoir->getPressure());
        motor_compressor[compr_idx]->setPowerVoltage(U_power);
        motor_compressor[compr_idx]->step(t, dt);

        FL_flow += motor_compressor[compr_idx]->getFLflow();
    }

    // Питательная магистраль
    FL_flow += horn[CAB1]->getFLflow();
    FL_flow += horn[CAB2]->getFLflow();
    FL_flow += sand_system->getFLflow();
    FL_flow += shutoff_crane[CAB1]->getFlowToPipe();
    FL_flow += shutoff_crane[CAB2]->getFlowToPipe();
    FL_flow += epk[CAB1]->getFLflow();
    FL_flow += epk[CAB2]->getFLflow();
    FL_flow += dako->getFLflow();
    FL_flow += bc_pressure_relay->getFLflow();

    anglecock_fl_fwd->setHoseFlow(hose_fl_fwd->getFlow());
    FL_flow += anglecock_fl_fwd->getFlowToPipe();

    anglecock_fl_bwd->setHoseFlow(hose_fl_bwd->getFlow());
    FL_flow += anglecock_fl_bwd->getFlowToPipe();

    main_reservoir->setFlow(FL_flow);
    main_reservoir->step(t, dt);

    // Концевые краны питательной магистрали
    anglecock_fl_fwd->setPipePressure(main_reservoir->getPressure());
    anglecock_fl_fwd->step(t, dt);
    anglecock_fl_bwd->setPipePressure(main_reservoir->getPressure());
    anglecock_fl_bwd->step(t, dt);

    // Рукава питательной магистрали
    hose_fl_fwd->setPressure(anglecock_fl_fwd->getPressureToHose());
    hose_fl_fwd->setFlowCoeff(anglecock_fl_fwd->getFlowCoeff());
    hose_fl_fwd->setCoord(train_coord + dir * (length / 2.0 - anglecock_fl_fwd->getShiftCoord()));
    hose_fl_fwd->setShiftSide(anglecock_fl_fwd->getShiftSide());
    hose_fl_fwd->step(t, dt);
    hose_fl_bwd->setPressure(anglecock_fl_bwd->getPressureToHose());
    hose_fl_bwd->setFlowCoeff(anglecock_fl_bwd->getFlowCoeff());
    hose_fl_bwd->setCoord(train_coord - dir * (length / 2.0 - anglecock_fl_bwd->getShiftCoord()));
    hose_fl_bwd->setShiftSide(anglecock_fl_bwd->getShiftSide());
    hose_fl_bwd->step(t, dt);
}
