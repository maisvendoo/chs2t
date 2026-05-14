#include "chs2t.h"

#include    "generator.h"
#include    "motor.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::registrate(const double& t, const double& dt)
{
    if (reg == nullptr)
        return;

    QString msg = "";
    msg += QString("v%1 kmh|").arg(velocity * 3.6, 10, 'f', 5);
    msg += QString("omega%1|").arg(wheel_omega[0], 10, 'f', 5);
    msg += QString("motor%1|").arg(motor->getTorque(), 12, 'f', 5);
    msg += QString("gener%1|").arg(generator->getTorque(), 12, 'f', 5);
    reg->print(msg, t, dt);
}
