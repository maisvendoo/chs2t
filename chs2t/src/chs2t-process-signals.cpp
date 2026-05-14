#include    "chs2t.h"

#include    "alsn-ukbm.h"
#include    "blinds.h"
#include    "generator.h"
#include    "handle-edt.h"
#include    "motor.h"
#include    "chs2t-signals.h"
#include    "sl2m.h"
#include    "stepswitch.h"

#include    <loco-crane.h>
#include    <motor-compressor-dc.h>
#include    <motor-fan-dc.h>
#include    <pantograph.h>
#include    <pneumo-combine-crane.h>
#include    <pneumo-shutoff-valve.h>
#include    <protective-device.h>
#include    <reservoir.h>
#include    <sanding-system.h>
#include    <train-horn.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::signalsOutput(const simulator_time_t& t, const double& dt)
{
    (void) t;
    (void) dt;

    analogSignal[SERIAL_NUMBER] = 1543.0f;

    // Вращение колёсных пар
    analogSignal[WHEELSET_1] = static_cast<float>(wheel_rotation_angle[0] / 2.0 / Physics::PI);
    analogSignal[WHEELSET_2] = static_cast<float>(wheel_rotation_angle[1] / 2.0 / Physics::PI);
    analogSignal[WHEELSET_3] = static_cast<float>(wheel_rotation_angle[2] / 2.0 / Physics::PI);
    analogSignal[WHEELSET_4] = static_cast<float>(wheel_rotation_angle[3] / 2.0 / Physics::PI);
    analogSignal[WHEELSET_5] = static_cast<float>(wheel_rotation_angle[4] / 2.0 / Physics::PI);
    analogSignal[WHEELSET_6] = static_cast<float>(wheel_rotation_angle[5] / 2.0 / Physics::PI);

    // Открытие жалюзи охлаждения пуско-тормозных резисторов
    analogSignal[BLINDS] = blinds->getPosition();

    // Состояние токоприемников
    analogSignal[PANT1_POS] = static_cast<float>(pantographs[PANT1]->getHeight());
    analogSignal[PANT2_POS] = static_cast<float>(pantographs[PANT2]->getHeight());

    // Поворот часовой и минутной стрелки на скоростемерах
    analogSignal[TIME_3SL2M_HOUR] = static_cast<float>(t.time.hour()) + static_cast<float>(t.time.minute()) / 60.0f;
    analogSignal[TIME_3SL2M_MINUTE] = static_cast<float>(t.time.minute()) + static_cast<float>(t.time.sec()) / 60.0f;

    // Кабины
    for (auto cab_idx : {CAB1, CAB2})
    {
        std::uint16_t d = (SPOTLIGHT_BWD - SPOTLIGHT_FWD) * cab_idx;

        // Прожектор
        const bool s_low = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::SPOTLIGHT, CHS2tSwitchers::SPOTLIGHT_LOW);
        const bool s_high = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::SPOTLIGHT, CHS2tSwitchers::SPOTLIGHT_HIGH);
        analogSignal[SPOTLIGHT_FWD + d] = 0.5f * static_cast<float>(s_low) + static_cast<float>(s_high);

        // Буферные огни
        const bool l_w = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::BUFLIGHT_L, CHS2tSwitchers::BUFLIGHT_WHITE);
        const bool l_r = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::BUFLIGHT_L, CHS2tSwitchers::BUFLIGHT_RED);
        const bool r_w = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::BUFLIGHT_R, CHS2tSwitchers::BUFLIGHT_WHITE);
        const bool r_r = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::BUFLIGHT_R, CHS2tSwitchers::BUFLIGHT_RED);
        analogSignal[BUFFERLIGHT_FWD_L_WHITE + d] = static_cast<float>(l_w);
        analogSignal[BUFFERLIGHT_FWD_L_RED + d] = static_cast<float>(l_r);
        analogSignal[BUFFERLIGHT_FWD_R_WHITE + d] = static_cast<float>(r_w);
        analogSignal[BUFFERLIGHT_FWD_R_RED + d] = static_cast<float>(r_r);

        // Свет в кабине
        const bool k_low = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::CAB_LIGHT, CHS2tSwitchers::CAB_LIGHT_LOW);
        const bool k_high = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::CAB_LIGHT, CHS2tSwitchers::CAB_LIGHT_HIGH);
        analogSignal[CAB1_LIGHT_CABINE + d] = 0.5f * static_cast<float>(k_low) + static_cast<float>(k_high);

        // Подсветка приборов
        const bool d_low = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::CAB_LIGHT, CHS2tSwitchers::CAB_LIGHT_DEVICES_LOW);
        const bool d_high = sw_panel[cab_idx].isSwitched(CHS2tSwitchers::CAB_LIGHT, CHS2tSwitchers::CAB_LIGHT_DEVICES_HIGH);
        analogSignal[CAB1_LIGHT_DEVICES + d] = 0.5f * static_cast<float>(d_low) + static_cast<float>(d_high);

        // Лампы локомотивного светофора
        analogSignal[CAB1_LS_WHITE + d] = safety_device[cab_idx]->getWhiteLamp();
        analogSignal[CAB1_LS_RED + d] = safety_device[cab_idx]->getRedLamp();
        analogSignal[CAB1_LS_REDYELLOW + d] = safety_device[cab_idx]->getRedYellowLamp();
        analogSignal[CAB1_LS_YELLOW + d] = safety_device[cab_idx]->getYellowLamp();
        analogSignal[CAB1_LS_GREEN + d] = safety_device[cab_idx]->getGreenLamp();

        // Сигнальные лампы
        analogSignal[CAB1_SIGLIGHT_SHUTOFF_PANTS + d] = static_cast<float>( !pant_switch[PANT1].getState() && !pant_switch[PANT2].getState() );

        analogSignal[CAB1_SIGLIGHT_BATTERY1 + d] = 0.0f;
        analogSignal[CAB1_SIGLIGHT_BATTERY2 + d] = 0.0f;
        analogSignal[CAB1_SIGLIGHT_RESISTS_FAN + d] = static_cast<float>(motor_fan_ptr->isPowered());
        analogSignal[CAB1_SIGLIGHT_BLINDS + d] = static_cast<float>(blinds->isOpened());
        analogSignal[CAB1_SIGLIGHT_ZASHITA + d] = 0.0f;
        analogSignal[CAB1_SIGLIGHT_PESOK + d] = static_cast<float>(sand_system->isSandDelivery());
        analogSignal[CAB1_SIGLIGHT_NO_BRAKES_RELEASE + d] = static_cast<float>(brake_mech[cab_idx]->getBCpressure() > 0.05);
        analogSignal[CAB1_SIGLIGHT_SYNC_FAIL + d] = 0.0f;
        analogSignal[CAB1_SIGLIGHT_0 + d] = static_cast<float>(stepSwitch->isZero());
        analogSignal[CAB1_SIGLIGHT_S + d] = static_cast<float>(stepSwitch->isSeries());
        analogSignal[CAB1_SIGLIGHT_SP + d] = static_cast<float>(stepSwitch->isSeriesParallel());
        analogSignal[CAB1_SIGLIGHT_P + d] = static_cast<float>(stepSwitch->isParallel());
        analogSignal[CAB1_SIGLIGHT_REOSTAT + d] = static_cast<float>(EDT);
        analogSignal[CAB1_SIGLIGHT_EPB_BRAKE + d] = static_cast<float>(epb_control->stateBrakeLamp());
        analogSignal[CAB1_SIGLIGHT_EPB_HOLD + d] = static_cast<float>(epb_control->stateHoldLamp());
        analogSignal[CAB1_SIGLIGHT_EPB_CONTROL + d] = static_cast<float>(epb_control->stateReleaseLamp());

        analogSignal[CAB1_SIGLIGHT_COMPR_1 + d] = static_cast<float>(motor_compressor[MK1]->isPowered());
        analogSignal[CAB1_SIGLIGHT_COMPR_2 + d] = static_cast<float>(motor_compressor[MK2]->isPowered());

        // Циферблаты
        analogSignal[CAB1_BATTERY_VOLTAGE + d] = static_cast<float>(U_bat / 100.0);
        analogSignal[CAB1_EPB_VOLTAGE + d] = static_cast<float>(epb_converter->getOutputVoltage() / 100.0);
        analogSignal[CAB1_WIRE_VOLTAGE + d] = static_cast<float>(U_kr / 4000.0);

        analogSignal[CAB1_KONTROLLER_POSITION + d] = static_cast<float>(stepSwitch->getPoz()) / 42.0f;

        if (EDT)
        {
            analogSignal[CAB1_ENGINE_CURRENT_1_2 + d] = 0.0f;
            analogSignal[CAB1_ENGINE_CURRENT_3_4 + d] = static_cast<float>(abs(generator->getIf()) / 1000.0);
            analogSignal[CAB1_ENGINE_CURRENT_5_6 + d] = static_cast<float>(abs(generator->getIa()) / 1000.0);
        }
        else
        {
            analogSignal[CAB1_ENGINE_CURRENT_1_2 + d] = static_cast<float>(motor->getI12() / 1000.0);
            analogSignal[CAB1_ENGINE_CURRENT_3_4 + d] = static_cast<float>(motor->getI34() / 1000.0);
            analogSignal[CAB1_ENGINE_CURRENT_5_6 + d] = static_cast<float>(motor->getI56() / 1000.0);
        }

        analogSignal[CAB1_PRESSURE_BP + d] = static_cast<float>(brakepipe->getPressure() / 1.0);
        analogSignal[CAB1_PRESSURE_ER + d] = static_cast<float>(brake_crane[cab_idx]->getERpressure() / 1.0);
        analogSignal[CAB1_PRESSURE_REF + d] = static_cast<float>(brake_ref_res->getPressure() / 1.0);
        analogSignal[CAB1_PRESSURE_FL + d] = static_cast<float>(main_reservoir->getPressure() / 1.6);
        analogSignal[CAB1_PRESSURE_BC + d] = static_cast<float>(brake_mech[cab_idx]->getBCpressure() / 1.0);

        // Контроллер машиниста
        analogSignal[CAB1_KM_IS_REVERS_HANDLE + d] = static_cast<float>(km21KR2[cab_idx].isReversHandle());
        analogSignal[CAB1_KM_REVERSOR_POS + d] = static_cast<float>(stepSwitch->getReverseState());
        analogSignal[CAB1_KM_CONTROLLER_POS + d] = static_cast<float>(km21KR2[cab_idx].getMainPos());
        analogSignal[CAB1_KM_CONTROLLER_HEIGHT + d] = static_cast<float>(km21KR2[cab_idx].getMainHeight());
        analogSignal[CAB1_BRAKE_STICK_POS + d] = handleEDT[cab_idx]->getHandlePos();

        // Приборы управления тормозами
        analogSignal[CAB1_SHUTOFF_CRANE_POS + d] = static_cast<float>(shutoff_crane[cab_idx]->getHandlePosition());
        analogSignal[CAB1_COMBINE_CRANE_POS + d] = static_cast<float>(combine_crane[cab_idx]->getCombineCraneHandlePosition());
        analogSignal[CAB1_BRAKE_CRANE_HANDLE_POS + d] = static_cast<float>(brake_crane[cab_idx]->getHandlePosition());
        analogSignal[CAB1_LOCO_CRANE_HANDLE_POS + d] = static_cast<float>(loco_crane[cab_idx]->getHandlePosition());
        analogSignal[CAB1_AUTOSTOP_IS_KEY + d] = static_cast<float>(epk[cab_idx]->isKey());
        analogSignal[CAB1_AUTOSTOP_KEY_POS + d] = static_cast<float>(epk[cab_idx]->isKeyOn());

        // Педали
        analogSignal[CAB1_SAND + d] = static_cast<float>(sand_system->isSandDelivery());
        analogSignal[CAB1_TIFON + d] = static_cast<float>(horn[cab_idx]->isTifon());

        // Кнопки
        analogSignal[CAB1_WHISTLE + d] = static_cast<float>(horn[cab_idx]->isSvistok());
        analogSignal[CAB1_BRAKE_RELEASE + d] = static_cast<float>(button_loco_release[cab_idx].getState());
        analogSignal[CAB1_RESET_POS_TO_NONREOSTAT + d] = static_cast<float>(button_sbros_cpc[cab_idx].getState());
        analogSignal[CAB1_FAST_SWITCH_OFF + d] = 0.0f;
        analogSignal[CAB1_RBS + d] = static_cast<float>(rb[cab_idx][RBS].getState());
        analogSignal[CAB1_RB_1 + d] = static_cast<float>(rb[cab_idx][RB1].getState());
        analogSignal[CAB1_RBP + d] = static_cast<float>(rb[cab_idx][RBP].getState());
        analogSignal[CAB1_P_SAND + d] = 0.0f;
        analogSignal[CAB1_P_TIFON + d] = 0.0f;
        analogSignal[CAB1_P_WHISTLE + d] = 0.0f;
        analogSignal[CAB1_MANEOURUS_POS_INC + d] = 0.0f;
        analogSignal[CAB1_MANEOURUS_POS_RESET + d] = 0.0f;
        analogSignal[CAB1_MANEOURUS_WHISTLE + d] = 0.0f;

        // Переключатели
        analogSignal[CAB1_SWITCHERS_PANEL_IS_KEY + d] = static_cast<float>(sw_panel[cab_idx].isKey());
        analogSignal[CAB1_SWITCHERS_PANEL_KEY_POS + d] = static_cast<float>(sw_panel[cab_idx].isKeyOn());
        // Верхний ряд
        analogSignal[CAB1_SWITCHER_EPB + d] = sw_panel[cab_idx].getSwitcherHandlePosition(CHS2tSwitchers::EPB);
        analogSignal[CAB1_SWITCHER_COMPR_1 + d] = sw_panel[cab_idx].getSwitcherHandlePosition(CHS2tSwitchers::COMPR_1);
        analogSignal[CAB1_SWITCHER_PANT_FWD + d] = sw_panel[cab_idx].getSwitcherHandlePosition(CHS2tSwitchers::PANT_FWD);
        analogSignal[CAB1_SWITCHER_FAST_SWITCH + d] = sw_panel[cab_idx].getSwitcherHandlePosition(CHS2tSwitchers::FAST_SW);
        // Средний ряд
        analogSignal[CAB1_SWITCHER_FANS + d] = sw_panel[cab_idx].getSwitcherHandlePosition(CHS2tSwitchers::FANS);
        analogSignal[CAB1_SWITCHER_COMPR_2 + d] = sw_panel[cab_idx].getSwitcherHandlePosition(CHS2tSwitchers::COMPR_2);
        analogSignal[CAB1_SWITCHER_PANT_BWD + d] = sw_panel[cab_idx].getSwitcherHandlePosition(CHS2tSwitchers::PANT_BWD);
        analogSignal[CAB1_SWITCHER_AUXCOMPR_SAND_BLINDS + d] = sw_panel[cab_idx].getSwitcherHandlePosition(CHS2tSwitchers::BLINDS);
        // Нижний ряд
        analogSignal[CAB1_SWITCHER_LIGHT_CAB_DEVICES + d] = sw_panel[cab_idx].getSwitcherHandlePosition(CHS2tSwitchers::CAB_LIGHT);
        analogSignal[CAB1_SWITCHER_BUFFERLIGHT_L + d] = sw_panel[cab_idx].getSwitcherHandlePosition(CHS2tSwitchers::BUFLIGHT_L);
        analogSignal[CAB1_SWITCHER_BUFFERLIGHT_R + d] = sw_panel[cab_idx].getSwitcherHandlePosition(CHS2tSwitchers::BUFLIGHT_R);
        analogSignal[CAB1_SWITCHER_SPOTLIGHT + d] = sw_panel[cab_idx].getSwitcherHandlePosition(CHS2tSwitchers::SPOTLIGHT);
        // Выключатель ЭДТ
        analogSignal[CAB1_SWITCHER_EDT + d] = sw_panel[cab_idx].getSwitcherHandlePosition(CHS2tSwitchers::EDT);

        // Скоростемер
        analogSignal[CAB1_3SL2M_SPEED + d] = speed_meter[cab_idx]->getArrowPos();
        analogSignal[CAB1_3SL2M_SHAFT + d] = speed_meter[cab_idx]->getShaftPos();

        // Прочее
        analogSignal[CAB1_INDICATOR_FAST_SWITCH + d] = static_cast<float>(bv->getLampState());
        analogSignal[CAB1_DIMMER_LIGHT_DEVICES + d] = 1.0f;
        analogSignal[CAB1_POWER_TRAINHEAT_KEY + d] = 0.0f;
        analogSignal[CAB1_POWER_TRAINHEAT_INDICATOR + d] = 0.0f;
        analogSignal[CAB1_STOP_HANDLE + d] = 0.0f;
        analogSignal[CAB1_SIGLIGHT_RESERVOIR_CLEAR + d] = 0.0f;
    }
}
