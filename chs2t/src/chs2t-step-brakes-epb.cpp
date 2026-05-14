#include    "chs2t.h"

#include    "handle-edt.h"

#include    <electro-airdistributor.h>
#include    <epb-2line-control.h>
#include    <epb-converter.h>
#include    <pneumo-hose-epb.h>
#include    <reservoir.h>

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
void CHS2T::stepEPB(const double& t, const double& dt)
{
    // Потребляемый ток электровоздухораспределителя
    double evr_current = electro_air_dist->getCurrent(0);

    // Потребляемый ток в рабочей линии ЭПТ
    double epb_work_curr = 0.0;
    epb_work_curr += evr_current;
    epb_work_curr += hose_bp_fwd->getCurrent(0);
    epb_work_curr += hose_bp_bwd->getCurrent(0);

    // Преобразователь напряжения для ЭПТ
    epb_converter->setInputVoltage(U_bat);
    epb_converter->setOutputCurrent(epb_work_curr);
    epb_converter->step(t, dt);

    // Контроллер двухпроводного ЭПТ
    const bool cab1_on = sw_panel[CAB1].isSwitched(CHS2tSwitchers::EPB, CHS2tSwitchers::EPB_ON);
    const bool cab2_on = sw_panel[CAB2].isSwitched(CHS2tSwitchers::EPB, CHS2tSwitchers::EPB_ON);
    epb_control->setInputVoltage(epb_converter->getOutputVoltage()
                                 * static_cast<double>(cab1_on || cab2_on) );
    epb_control->setHoldState((cab1_on && brake_crane[CAB1]->isHold()) ||
                              (cab2_on && brake_crane[CAB2]->isHold()));
    epb_control->setBrakeState((cab1_on && brake_crane[CAB1]->isBrake()) ||
                               (cab2_on && brake_crane[CAB2]->isBrake()));
    epb_control->setControlVoltage(  hose_bp_fwd->getVoltage(1)
                                      + hose_bp_bwd->getVoltage(1) );
    epb_control->step(t, dt);
    double epb_work_U = epb_control->getWorkVoltage();
    double epb_work_f = epb_control->getWorkFrequency();

    double evr_U = 0.0;
    double evr_f = 0.0;
    // Управление электровоздухораспределителем: отключается кнопкой "Отпуск электровоза"
    if (!(button_loco_release[CAB1].getState() || button_loco_release[CAB2].getState()))
    {
        // Управление от задатчика ЭДТ ("карандаша") - до давления в 0.22 МПа
        if (brake_ref_res->getPressure() < 0.22)
        {
            if ((km21KR2[CAB1].getReversHandlePos() != 0))
            {
                evr_U = handleEDT[CAB1]->getControlSignal() * epb_converter->getOutputVoltage();
            }
            if ((km21KR2[CAB2].getReversHandlePos() != 0))
            {
                evr_U = handleEDT[CAB2]->getControlSignal() * epb_converter->getOutputVoltage();
            }
        }

        // Если управления с задатчика нет, сигнал из рабочей линии ЭПТ
        if (evr_U == 0.0)
        {
            evr_U = epb_work_U + hose_bp_fwd->getVoltage(0) + hose_bp_bwd->getVoltage(0);
            evr_f = epb_work_f + hose_bp_fwd->getFrequency(0) + hose_bp_bwd->getFrequency(0);
        }
    }
    electro_air_dist->setVoltage  (0, evr_U);
    electro_air_dist->setFrequency(0, evr_f);

    // Межвагонные сигналы линий ЭПТ по рукавам тормозной магистрали
    // Рабочая линия спереди
    hose_bp_fwd->setVoltage  (0, hose_bp_bwd->getVoltage(0) + epb_work_U);
    hose_bp_fwd->setFrequency(0, hose_bp_bwd->getFrequency(0) + epb_work_f);
    hose_bp_fwd->setCurrent  (0, hose_bp_bwd->getCurrent(0) + evr_current);
    // Контрольная линия спереди
    hose_bp_fwd->setVoltage  (1, hose_bp_bwd->getVoltage(1));
    hose_bp_fwd->setFrequency(1, hose_bp_bwd->getFrequency(1));
    hose_bp_fwd->setCurrent  (1, hose_bp_bwd->getCurrent(1));

    // Рабочая линия сзади
    hose_bp_bwd->setVoltage  (0, hose_bp_fwd->getVoltage(0) + epb_work_U);
    hose_bp_bwd->setFrequency(0, hose_bp_fwd->getFrequency(0) + epb_work_f);
    hose_bp_bwd->setCurrent  (0, hose_bp_fwd->getCurrent(0) + evr_current);
    // Контрольная линия сзади
    hose_bp_bwd->setVoltage  (1, hose_bp_fwd->getVoltage(1));
    hose_bp_bwd->setFrequency(1, hose_bp_fwd->getFrequency(1));
    hose_bp_bwd->setCurrent  (1, hose_bp_fwd->getCurrent(1));
}
