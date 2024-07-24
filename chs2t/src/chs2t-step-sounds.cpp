#include    "chs2t.h"
#include    "chs2t-signals.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::stepSoundSignalsOutput(double t, double dt)
{
    (void) t;
    (void) dt;

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

    // Свисток и тифон
    analogSignal[SOUND_SVISTOK] = horn->getSoundSignal(TrainHorn::SVISTOK_SOUND);
    analogSignal[SOUND_TIFON] = horn->getSoundSignal(TrainHorn::TIFON_SOUND);
/*
    // Реверсор и контроллер
    analogSignal[SOUND_REVERSOR] = km21KR2->getSoundSignal(Km21KR2::REVERS_CHANGE_POS_SOUND);
    analogSignal[SOUND_CONTROLLER] = km21KR2->getSoundSignal(Km21KR2::MAIN_CHANGE_POS_SOUND);
*/
    // Кран поездного тормоза
    analogSignal[SOUND_BRAKE_CRANE_CHANGE_POS] = brake_crane->getSoundSignal(BrakeCrane::CHANGE_POS_SOUND);
    analogSignal[SOUND_BRAKE_CRANE_ER_STAB_FLOW] = brake_crane->getSoundSignal(BrakeCrane::ER_STAB_SOUND);
    analogSignal[SOUND_BRAKE_CRANE_ER_FILL_FLOW] = brake_crane->getSoundSignal(BrakeCrane::ER_FILL_FLOW_SOUND);
    analogSignal[SOUND_BRAKE_CRANE_ER_DRAIN_FLOW] = brake_crane->getSoundSignal(BrakeCrane::ER_DRAIN_FLOW_SOUND);
    analogSignal[SOUND_BRAKE_CRANE_BP_FILL_FLOW] = brake_crane->getSoundSignal(BrakeCrane::BP_FILL_FLOW_SOUND);
    analogSignal[SOUND_BRAKE_CRANE_BP_DRAIN_FLOW] = brake_crane->getSoundSignal(BrakeCrane::BP_DRAIN_FLOW_SOUND);
    // Кран локомотивного тормоза
    analogSignal[SOUND_LOCO_CRANE_CHANGE_POS] = loco_crane->getSoundSignal(LocoCrane::CHANGE_POS_SOUND);
    analogSignal[SOUND_LOCO_CRANE_BC_FILL_FLOW] = loco_crane->getSoundSignal(LocoCrane::BC_FILL_FLOW_SOUND);
    analogSignal[SOUND_LOCO_CRANE_BC_DRAIN_FLOW] = loco_crane->getSoundSignal(LocoCrane::BC_DRAIN_FLOW_SOUND);

    // Скоростемер
    analogSignal[SOUND_SPEED_METER_SL2M] = speed_meter->getSoundSignal();

    // Токоприёмники
    analogSignal[SOUND_PANT_BWD_UP] = pantographs[1]->getSoundSignal(Pantograph::UP_SOUND);
    analogSignal[SOUND_PANT_BWD_DOWN] = pantographs[1]->getSoundSignal(Pantograph::DOWN_SOUND);
    analogSignal[SOUND_PANT_FWD_UP] = pantographs[0]->getSoundSignal(Pantograph::UP_SOUND);
    analogSignal[SOUND_PANT_FWD_DOWN] = pantographs[0]->getSoundSignal(Pantograph::DOWN_SOUND);
    // Главный выключатель
    analogSignal[SOUND_GV_ON] = bv->getSoundSignal(ProtectiveDevice::ON_SOUND);
    analogSignal[SOUND_GV_OFF] = bv->getSoundSignal(ProtectiveDevice::OFF_SOUND);

    // Мотор-компрессоры
    analogSignal[SOUND_COMPRESSOR_1] = motor_compressor[0]->getSoundSignal();
    analogSignal[SOUND_COMPRESSOR_2] = motor_compressor[1]->getSoundSignal();
    // Мотор-вентиляторы
    analogSignal[SOUND_FAN_1] = motor_fan[0]->getSoundSignal();
    analogSignal[SOUND_FAN_2] = motor_fan[1]->getSoundSignal();
    analogSignal[SOUND_FAN_PTR] = motor_fan_ptr->getSoundSignal();

/* ============ VL60 ==================
    // Дальний ряд тумблеров приборной панели машиниста
//    analogSignal[SOUND_TUMBLER_PROJECTOR2_ON] = proj2_tumbler.getSoundSignal(Trigger::ON_SOUND);
//    analogSignal[SOUND_TUMBLER_PROJECTOR1_ON] = proj1_tumbler.getSoundSignal(Trigger::ON_SOUND);
//    analogSignal[SOUND_TUMBLER_RADIO_ON] = radio_tumbler.getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_CTRL_CIRCUIT_ON] = cu_tumbler.getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_PANT_BWD_ON] = pant2_tumbler.getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_PANT_FWD_ON] = pant1_tumbler.getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_PANTS_ON] = pants_tumbler.getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_GV_RETURN_ON] = gv_return_tumbler.getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_GV_ON] = gv_tumbler.getSoundSignal(Trigger::ON_SOUND);

//    analogSignal[SOUND_TUMBLER_PROJECTOR2_OFF] = proj2_tumbler.getSoundSignal(Trigger::OFF_SOUND);
//    analogSignal[SOUND_TUMBLER_PROJECTOR1_OFF] = proj1_tumbler.getSoundSignal(Trigger::OFF_SOUND);
//    analogSignal[SOUND_TUMBLER_RADIO_OFF] = radio_tumbler.getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_CTRL_CIRCUIT_OFF] = cu_tumbler.getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_PANT_BWD_OFF] = pant2_tumbler.getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_PANT_FWD_OFF] = pant1_tumbler.getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_PANTS_OFF] = pants_tumbler.getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_GV_RETURN_OFF] = gv_return_tumbler.getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_GV_OFF] = gv_tumbler.getSoundSignal(Trigger::OFF_SOUND);

    // Ближний ряд тумблеров приборной панели машиниста
//    analogSignal[SOUND_TUMBLER_AUTOSAND_ON] = autosand_tumbler.getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_FAN6_ON] = mv_tumblers[MV6].getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_FAN5_ON] = mv_tumblers[MV5].getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_FAN4_ON] = mv_tumblers[MV4].getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_FAN3_ON] = mv_tumblers[MV3].getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_FAN2_ON] = mv_tumblers[MV2].getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_FAN1_ON] = mv_tumblers[MV1].getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_COMPRESSOR_ON] = mk_tumbler.getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_PHASESPLITTER_ON] = fr_tumbler.getSoundSignal(Trigger::ON_SOUND);

//    analogSignal[SOUND_TUMBLER_PROJECTOR2_OFF] = autosand_tumbler.getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_FAN6_OFF] = mv_tumblers[MV6].getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_FAN5_OFF] = mv_tumblers[MV5].getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_FAN4_OFF] = mv_tumblers[MV4].getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_FAN3_OFF] = mv_tumblers[MV3].getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_FAN2_OFF] = mv_tumblers[MV2].getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_FAN1_OFF] = mv_tumblers[MV1].getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_COMPRESSOR_OFF] = mk_tumbler.getSoundSignal(Trigger::OFF_SOUND);
    analogSignal[SOUND_TUMBLER_PHASESPLITTER_OFF] = fr_tumbler.getSoundSignal(Trigger::OFF_SOUND);

    // Тяговые электродвигатели
    analogSignal[SOUND_TRACTION_ELETROENGINE_1] = motor[TED1]->getSoundSignal();
    analogSignal[SOUND_TRACTION_ELETROENGINE_2] = motor[TED2]->getSoundSignal();
    analogSignal[SOUND_TRACTION_ELETROENGINE_3] = motor[TED3]->getSoundSignal();
    analogSignal[SOUND_TRACTION_ELETROENGINE_4] = motor[TED4]->getSoundSignal();
    analogSignal[SOUND_TRACTION_ELETROENGINE_5] = motor[TED5]->getSoundSignal();
    analogSignal[SOUND_TRACTION_ELETROENGINE_6] = motor[TED6]->getSoundSignal();

    // Тумблер электропневматического тормоза
    analogSignal[SOUND_TUMBLER_EPB_ON] = epb_switch.getSoundSignal(Trigger::ON_SOUND);
    analogSignal[SOUND_TUMBLER_EPB_OFF] = epb_switch.getSoundSignal(Trigger::OFF_SOUND);*/
}
