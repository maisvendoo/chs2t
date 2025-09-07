#include    "chs2t.h"

#include    "chs2t-signals.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::signalsOutput(const simulator_time_t& t, const double& dt)
{
    (void) t;
    (void) dt;

    analogSignal[STRELKA_POS] = static_cast<float>(stepSwitch->getPoz()) / 42.0f;

    analogSignal[STRELKA_AMP1] = static_cast<float>(motor->getI12() / 1000.0);

    if (EDT)
    {
        analogSignal[STRELKA_AMP3] = static_cast<float>(abs(generator->getIa()) / 1000.0);
        analogSignal[STRELKA_AMP2] = static_cast<float>(abs(generator->getIf()) / 1000.0);
    }
    else
    {
        analogSignal[STRELKA_AMP3] = static_cast<float>(motor->getI56() / 1000.0);
        analogSignal[STRELKA_AMP2] = static_cast<float>(motor->getI34() / 1000.0);
    }

    analogSignal[STRELKA_PM] = static_cast<float>(main_reservoir->getPressure() / 1.6);
    analogSignal[STRELKA_TC] = static_cast<float>(brake_mech[TROLLEY_FWD]->getBCpressure() / 1.0);
    analogSignal[STRELKA_EDT] = static_cast<float>(brake_ref_res->getPressure() / 1.0);
    analogSignal[STRELKA_UR] = static_cast<float>(brake_crane[CAB1]->getERpressure() / 1.0);
    analogSignal[STRELKA_TM] = static_cast<float>(brakepipe->getPressure() / 1.0);

    analogSignal[STRELKA_UKS] = static_cast<float>(U_kr / 4000.0);

    analogSignal[STRELKA_U_BAT] = static_cast<float>(U_bat / 100.0);
    analogSignal[STRELKA_U_EPT] = static_cast<float>(epb_converter->getOutputVoltage() / 100.0);

    analogSignal[KRAN395_RUK] = static_cast<float>(brake_crane[CAB1]->getHandlePosition());
    analogSignal[KRAN254_RUK] = static_cast<float>(loco_crane[CAB1]->getHandlePosition());

    analogSignal[KONTROLLER] = static_cast<float>(km21KR2[CAB1]->getMainShaftPos());
    analogSignal[REVERSOR] = static_cast<float>(stepSwitch->getReverseState());
    analogSignal[SHTURVAL] = static_cast<float>(km21KR2[CAB1]->getHandleHeight());
    analogSignal[SHTUR_SVISTOK] = static_cast<float>(horn->isSvistok());

    analogSignal[SIGLIGHT_P] = static_cast<float>(stepSwitch->isParallel());
    analogSignal[SIGLIGHT_SP] = static_cast<float>(stepSwitch->isSeriesParallel());
    analogSignal[SIGLIGHT_S] = static_cast<float>(stepSwitch->isSeries());
    analogSignal[SIGLIGHT_ZERO] = static_cast<float>(stepSwitch->isZero());

    analogSignal[SIGLIGHT_R] = static_cast<float>(EDT);

    analogSignal[SIGLIGHT_O] = static_cast<float>(epb_control->stateReleaseLamp());
    analogSignal[SIGLIGHT_PEREKRISHA] = static_cast<float>(epb_control->stateHoldLamp());
    analogSignal[SIGLIGHT_T] = static_cast<float>(epb_control->stateBrakeLamp());

    analogSignal[SIGLIGHT_NO_BRAKES_RELEASE] = static_cast<float>(brake_mech[0]->getBCpressure() >= 0.1);

    analogSignal[PANT1] = static_cast<float>(pantographs[0]->getHeight());
    analogSignal[PANT2] = static_cast<float>(pantographs[1]->getHeight());

    analogSignal[SW_PNT1] = pant_switcher[CAB1][0].getHandlePosition();
    analogSignal[SW_PNT2] = pant_switcher[CAB1][1].getHandlePosition();

    analogSignal[SIGLIGHT_RAZED] = static_cast<float>( !pant_switch[0].getState() && !pant_switch[1].getState() );

    analogSignal[INDICATOR_BV] = static_cast<float>(bv->getLampState());

    analogSignal[SW_BV] = fastswitch_switcher[CAB1].getHandlePosition();
    analogSignal[SW_MV] = motor_fan_switcher[CAB1].getHandlePosition();
    analogSignal[SW_MK1] = mk_switcher[CAB1][0].getHandlePosition();
    analogSignal[SW_MK2] = mk_switcher[CAB1][1].getHandlePosition();

    analogSignal[SW_EPT] = static_cast<float>(epb_switch[CAB1].getState());

    analogSignal[SW_VK] = blinds_switcher[CAB1].getHandlePosition();

    analogSignal[BLINDS] = blinds->getPosition();
    analogSignal[SIGLIGHT_GALYZI] = static_cast<float>(blinds->isOpened());

    analogSignal[HANDLE_RT] = handleEDT[CAB1]->getHandlePos();

    analogSignal[STRELKA_SPEED] = speed_meter[CAB1]->getArrowPos();
    analogSignal[VAL_PR_SKOR1] = speed_meter[CAB1]->getShaftPos();
    analogSignal[VAL_PR_SKOR2] = speed_meter[CAB1]->getShaftPos();

    analogSignal[SW_EDT] = EDT_switch[CAB1].getState();

    // Лампы локомотивного светофора
    analogSignal[LS_W] = safety_device[CAB1]->getWhiteLamp();
    analogSignal[LS_YR] = safety_device[CAB1]->getRedYellowLamp();
    analogSignal[LS_R] = safety_device[CAB1]->getRedLamp();
    analogSignal[LS_Y] = safety_device[CAB1]->getYellowLamp();
    analogSignal[LS_G] = safety_device[CAB1]->getGreenLamp();
    analogSignal[EPK] = static_cast<float>(epk[CAB1]->isKeyOn());

    analogSignal[CAB1_RBS] = static_cast<float>(rb[CAB1][RBS].getState());
    analogSignal[CAB1_RB1] = static_cast<float>(rb[CAB1][RB1].getState());

    analogSignal[BUTTON_SBROS_CPC] = static_cast<float>(button_sbros_cpc[CAB1].getState());
    analogSignal[BUTTON_LOCO_RELEASE] = static_cast<float>(button_loco_release[CAB1].getState());

    analogSignal[WHEEL_1] = static_cast<float>(wheel_rotation_angle[0] / 2.0 / Physics::PI);
    analogSignal[WHEEL_2] = static_cast<float>(wheel_rotation_angle[1] / 2.0 / Physics::PI);
    analogSignal[WHEEL_3] = static_cast<float>(wheel_rotation_angle[2] / 2.0 / Physics::PI);
    analogSignal[WHEEL_4] = static_cast<float>(wheel_rotation_angle[3] / 2.0 / Physics::PI);
    analogSignal[WHEEL_5] = static_cast<float>(wheel_rotation_angle[4] / 2.0 / Physics::PI);
    analogSignal[WHEEL_6] = static_cast<float>(wheel_rotation_angle[5] / 2.0 / Physics::PI);
}
