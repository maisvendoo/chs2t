#include    "chs2t.h"
#include    "chs2t-signals.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::soundsOutput(const simulator_time_t& t, const double& dt)
{
    (void) t;
    (void) dt;

    // Озвучка кабин
    for (auto cab_idx : {CAB1, CAB2})
    {
        std::uint16_t d = (SOUND_BWD_SVISTOK - SOUND_FWD_SVISTOK) * cab_idx;

        // Свисток и тифон
        analogSignal[SOUND_FWD_SVISTOK + d] = horn[cab_idx]->getSoundSignal(TrainHorn::SVISTOK_SOUND);
        analogSignal[SOUND_FWD_TIFON + d] = horn[cab_idx]->getSoundSignal(TrainHorn::TIFON_SOUND);

        // Реверсор и контроллер
        analogSignal[CAB1_SOUND_INSERT_REVERS_HANDLE + d] = km21KR2[cab_idx].getSoundSignal(Km21KR2::HANDLE_INSERTED_SOUND);
        analogSignal[CAB1_SOUND_REMOVE_REVERS_HANDLE + d] = km21KR2[cab_idx].getSoundSignal(Km21KR2::HANDLE_REMOVED_SOUND);
        analogSignal[CAB1_SOUND_REVERS_CHANGE_POS + d] = km21KR2[cab_idx].getSoundSignal(Km21KR2::REVERS_CHANGE_POS_SOUND);
        analogSignal[CAB1_SOUND_MAIN_NONFIXED_ON + d] = km21KR2[cab_idx].getSoundSignal(Km21KR2::MAIN_NONFIXED_ON_SOUND);
        analogSignal[CAB1_SOUND_MAIN_NONFIXED_OFF + d] = km21KR2[cab_idx].getSoundSignal(Km21KR2::MAIN_NONFIXED_OFF_SOUND);
        analogSignal[CAB1_SOUND_MAIN_FIXED_ON + d] = km21KR2[cab_idx].getSoundSignal(Km21KR2::MAIN_FIXED_RESET_ON_SOUND);
        analogSignal[CAB1_SOUND_MAIN_FIXED_OFF + d] = km21KR2[cab_idx].getSoundSignal(Km21KR2::MAIN_FIXED_RESET_OFF_SOUND);
        analogSignal[CAB1_SOUND_MAIN_CHANGE_FIELDWEAK_POS + d] = km21KR2[cab_idx].getSoundSignal(Km21KR2::MAIN_CHANGE_FIELDWEAK_SOUND);

        // Скоростемер
        analogSignal[CAB1_SOUND_SPEED_METER_SL2M + d] = speed_meter[cab_idx]->getSoundSignal();

        // Разобщительный и комбинированный краны
        analogSignal[CAB1_SOUND_SHUTOFF_CRANE_CHANGE_POS + d] = shutoff_crane[cab_idx]->getSoundSignal(PneumoShutoffValve::CHANGE_SOUND);
        analogSignal[CAB1_SOUND_COMB_CRANE_CHANGE_POS + d] = combine_crane[cab_idx]->getSoundSignal(PneumoCombineCrane::CHANGE_COMB_POS_SOUND);
        analogSignal[CAB1_SOUND_COMB_CRANE_BP_DRAIN_FLOW + d] = combine_crane[cab_idx]->getSoundSignal(PneumoCombineCrane::BP_DRAIN_FLOW_SOUND);

        // ЭПК
        analogSignal[CAB1_SOUND_EPK_INSERT_KEY + d] = epk[cab_idx]->getSoundSignal(AutoTrainStop::KEY_INSERTED);
        analogSignal[CAB1_SOUND_EPK_KEY_ON + d] = epk[cab_idx]->getSoundSignal(AutoTrainStop::KEY_STATE_ON);
        analogSignal[CAB1_SOUND_EPK_REMOVE_KEY + d] = epk[cab_idx]->getSoundSignal(AutoTrainStop::KEY_REMOVED);
        analogSignal[CAB1_SOUND_EPK_KEY_OFF + d] = epk[cab_idx]->getSoundSignal(AutoTrainStop::KEY_STATE_OFF);
        analogSignal[CAB1_SOUND_AUTOSTOP_WHISTLE + d] = epk[cab_idx]->getSoundSignal(AutoTrainStop::AUTOSTOP_WHISTLE);

        // Поездной кран
        analogSignal[CAB1_SOUND_BRAKE_CRANE_CHANGE_POS + d] = brake_crane[cab_idx]->getSoundSignal(BrakeCrane::CHANGE_POS_SOUND);
        analogSignal[CAB1_SOUND_BRAKE_CRANE_ER_STAB_FLOW + d] = brake_crane[cab_idx]->getSoundSignal(BrakeCrane::ER_STAB_SOUND);
        analogSignal[CAB1_SOUND_BRAKE_CRANE_ER_FILL_FLOW + d] = brake_crane[cab_idx]->getSoundSignal(BrakeCrane::ER_FILL_FLOW_SOUND);
        analogSignal[CAB1_SOUND_BRAKE_CRANE_ER_DRAIN_FLOW + d] = brake_crane[cab_idx]->getSoundSignal(BrakeCrane::ER_DRAIN_FLOW_SOUND);
        analogSignal[CAB1_SOUND_BRAKE_CRANE_BP_FILL_FLOW + d] = brake_crane[cab_idx]->getSoundSignal(BrakeCrane::BP_FILL_FLOW_SOUND);
        analogSignal[CAB1_SOUND_BRAKE_CRANE_BP_DRAIN_FLOW + d] = brake_crane[cab_idx]->getSoundSignal(BrakeCrane::BP_DRAIN_FLOW_SOUND);

        // Локомотивный кран
        analogSignal[CAB1_SOUND_LOCO_CRANE_CHANGE_POS + d] = loco_crane[cab_idx]->getSoundSignal(LocoCrane::CHANGE_POS_SOUND);
        analogSignal[CAB1_SOUND_LOCO_CRANE_BC_FILL_FLOW + d] = loco_crane[cab_idx]->getSoundSignal(LocoCrane::BC_FILL_FLOW_SOUND);
        analogSignal[CAB1_SOUND_LOCO_CRANE_BC_DRAIN_FLOW + d] = loco_crane[cab_idx]->getSoundSignal(LocoCrane::BC_DRAIN_FLOW_SOUND);

        // Переключатели
        analogSignal[CAB1_SOUND_INSERT_SWITCHERS_PANEL_KEY + d] = sw_panel[cab_idx].getKeyInsertSoundSignal(Trigger::ON_SOUND);
        analogSignal[CAB1_SOUND_REMOVE_SWITCHERS_PANEL_KEY + d] = sw_panel[cab_idx].getKeyInsertSoundSignal(Trigger::OFF_SOUND);
        analogSignal[CAB1_SOUND_TURN_SWITCHERS_PANEL_KEY + d] = sw_panel[cab_idx].getKeyTurnSoundSignal(Trigger::CHANGE_SOUND);
        // Верхний ряд
        analogSignal[CAB1_SOUND_SWITCHER_EPB + d] = sw_panel[cab_idx].getSoundSignal(CHS2tSwitchers::EPB);
        analogSignal[CAB1_SOUND_SWITCHER_COMPR_1 + d] = sw_panel[cab_idx].getSoundSignal(CHS2tSwitchers::COMPR_1);
        analogSignal[CAB1_SOUND_SWITCHER_PANT_FWD + d] = sw_panel[cab_idx].getSoundSignal(CHS2tSwitchers::PANT_FWD);
        analogSignal[CAB1_SOUND_SWITCHER_FAST_SWITCH + d] = sw_panel[cab_idx].getSoundSignal(CHS2tSwitchers::FAST_SW);
        // Средний ряд
        analogSignal[CAB1_SOUND_SWITCHER_FANS + d] = sw_panel[cab_idx].getSoundSignal(CHS2tSwitchers::FANS);
        analogSignal[CAB1_SOUND_SWITCHER_COMPR_2 + d] = sw_panel[cab_idx].getSoundSignal(CHS2tSwitchers::COMPR_2);
        analogSignal[CAB1_SOUND_SWITCHER_PANT_BWD + d] = sw_panel[cab_idx].getSoundSignal(CHS2tSwitchers::PANT_BWD);
        analogSignal[CAB1_SOUND_SWITCHER_AUXCOMPR_SAND_BLINDS + d] = sw_panel[cab_idx].getSoundSignal(CHS2tSwitchers::BLINDS);
        // Нижний ряд
        analogSignal[CAB1_SOUND_SWITCHER_LIGHT_CAB_DEVICES + d] = sw_panel[cab_idx].getSoundSignal(CHS2tSwitchers::CAB_LIGHT);
        analogSignal[CAB1_SOUND_SWITCHER_BUFFERLIGHT_L + d] = sw_panel[cab_idx].getSoundSignal(CHS2tSwitchers::BUFLIGHT_L);
        analogSignal[CAB1_SOUND_SWITCHER_BUFFERLIGHT_R + d] = sw_panel[cab_idx].getSoundSignal(CHS2tSwitchers::BUFLIGHT_R);
        analogSignal[CAB1_SOUND_SWITCHER_SPOTLIGHT + d] = sw_panel[cab_idx].getSoundSignal(CHS2tSwitchers::SPOTLIGHT);
        // Выключатель ЭДТ
        analogSignal[CAB1_SOUND_SWITCHER_EDT + d] = sw_panel[cab_idx].getSoundSignal(CHS2tSwitchers::EDT);
    }

    // Звуки в движении
    double Vkmh = abs(velocity) * Physics::kmh;
    analogSignal[SOUND_5_10] = sound_state_t::createSoundSignal((Vkmh > 1.0) && (Vkmh <= 10.0), Vkmh / 5.0);
    analogSignal[SOUND_10_20] = sound_state_t::createSoundSignal((Vkmh > 10.0) && (Vkmh <= 20.0));
    analogSignal[SOUND_20_30] = sound_state_t::createSoundSignal((Vkmh > 20.0) && (Vkmh <= 30.0));
    analogSignal[SOUND_30_40] = sound_state_t::createSoundSignal((Vkmh > 30.0) && (Vkmh <= 40.0));
    analogSignal[SOUND_40_50] = sound_state_t::createSoundSignal((Vkmh > 40.0) && (Vkmh <= 50.0));
    analogSignal[SOUND_50_60] = sound_state_t::createSoundSignal((Vkmh > 50.0) && (Vkmh <= 60.0));
    analogSignal[SOUND_60_70] = sound_state_t::createSoundSignal((Vkmh > 60.0) && (Vkmh <= 70.0));
    analogSignal[SOUND_70_80] = sound_state_t::createSoundSignal((Vkmh > 70.0) && (Vkmh <= 80.0));
    analogSignal[SOUND_80_90] = sound_state_t::createSoundSignal((Vkmh > 80.0) && (Vkmh <= 90.0));
    analogSignal[SOUND_90_100] = sound_state_t::createSoundSignal((Vkmh > 90.0) && (Vkmh <= 100.0));
    analogSignal[SOUND_100_110] = sound_state_t::createSoundSignal((Vkmh > 100.0) && (Vkmh <= 110.0));
    analogSignal[SOUND_110_X] = sound_state_t::createSoundSignal(Vkmh > 110.0);

    // Токоприёмники
    analogSignal[SOUND_PANT_FWD_UP] = pantographs[PANT1]->getSoundSignal(Pantograph::UP_SOUND);
    analogSignal[SOUND_PANT_FWD_DOWN] = pantographs[PANT1]->getSoundSignal(Pantograph::DOWN_SOUND);
    analogSignal[SOUND_PANT_BWD_UP] = pantographs[PANT2]->getSoundSignal(Pantograph::UP_SOUND);
    analogSignal[SOUND_PANT_BWD_DOWN] = pantographs[PANT2]->getSoundSignal(Pantograph::DOWN_SOUND);
    // Главный выключатель
    analogSignal[SOUND_BV_ON] = bv->getSoundSignal(ProtectiveDevice::ON_SOUND);
    analogSignal[SOUND_BV_OFF] = bv->getSoundSignal(ProtectiveDevice::OFF_SOUND);

    // Мотор-компрессоры
    analogSignal[SOUND_COMPRESSOR_1] = motor_compressor[MK1]->getSoundSignal();
    analogSignal[SOUND_COMPRESSOR_2] = motor_compressor[MK2]->getSoundSignal();
    // Мотор-вентиляторы
    analogSignal[SOUND_FAN_1] = motor_fan[TROLLEY_FWD]->getSoundSignal();
    analogSignal[SOUND_FAN_2] = motor_fan[TROLLEY_BWD]->getSoundSignal();
    analogSignal[SOUND_FAN_RESIST] = motor_fan_ptr->getSoundSignal();

    // Песочница
    analogSignal[SOUND_SAND_DELIVERY] = sand_system->getSoundSignal();
    // Тяговые электродвигатели
    analogSignal[SOUND_TRACTION_ELECTROENGINE] = motorSoundSignal();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float CHS2T::motorSoundSignal()
{
    // Если есть ток в модуле motor - ТЭД в режиме тяги
    if (motor->getY(0) > 20.0)
        return motor->getSoundSignal();

    // Если есть ток в модуле generator - ТЭД в режиме электродинамического торможения
    if (generator->getY(0) > 20.0)
        return generator->getSoundSignal();

    // ТЭД на выбеге, без звука
    return sound_state_t::createSoundSignal(false);
}
