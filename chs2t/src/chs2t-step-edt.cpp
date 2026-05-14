#include    "chs2t.h"

#include    "brake-regulator.h"
#include    "dako.h"
#include    "generator.h"
#include    "pulse-converter.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepEDT(const double& t, const double& dt)
{
    pulseConv->setUakb(110.0 * static_cast<double>(EDT));
    pulseConv->setU(BrakeReg->getU());
    pulseConv->setUt(generator->getUt() * static_cast<double>(EDT));

    generator->setUf(pulseConv->getUf());
    generator->setOmega(wheel_omega[0] * ip);
    generator->setRt(3.35);

    const bool EDT_cab1 = km21KR2[CAB1].isReversHandle() &&
        sw_panel[CAB1].isSwitched(CHS2tSwitchers::EDT, CHS2tSwitchers::EDT_ON);
    const bool EDT_cab2 = km21KR2[CAB2].isReversHandle() &&
        sw_panel[CAB2].isSwitched(CHS2tSwitchers::EDT, CHS2tSwitchers::EDT_ON);

    BrakeReg->setActive(EDT_cab1 || EDT_cab2);
    BrakeReg->setAllowEDT(dako->isEDTAllow());
    BrakeReg->setIa(generator->getIa());
    BrakeReg->setIf(generator->getIf());
    BrakeReg->setBref(brake_ref_res->getPressure());

    pulseConv->step(t, dt);
    generator->step(t, dt);
    BrakeReg->step(t, dt);
    if (EDT_timer.step(t, dt))
    {
        enableEDT();
    }

    if (EDT_cab1 || EDT_cab2)
    {
        if ( (brake_ref_res->getPressure() >= 0.07) && !EDT_timer.isStarted())
        {
            dropPosition = true;
            EDT_timer.start();
        }

        if (allowEDT)
        {
            if (!dako->isEDTAllow() || (brake_ref_res->getPressure() < 0.07) )
            {
                disableEDT();
                dropPosition = false;
                BrakeReg->reset();
            }
        }
    }
    else
    {
        disableEDT();
    }
}
