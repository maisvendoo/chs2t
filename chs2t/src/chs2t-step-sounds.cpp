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
/* TODO озвучка в контроллере
    // Реверсор и контроллер
    analogSignal[SOUND_REVERSOR] = km21KR2->getSoundSignal(Km21KR2::REVERS_CHANGE_POS_SOUND);
    analogSignal[SOUND_CONTROLLER1] = km21KR2->getSoundSignal(Km21KR2::MAIN_CHANGE_POS_SOUND);
    analogSignal[SOUND_CONTROLLER2] = km21KR2->getSoundSignal(Km21KR2::MAIN_CHANGE_POS_SOUND);
    analogSignal[SOUND_CONTROLLER3] = km21KR2->getSoundSignal(Km21KR2::MAIN_CHANGE_POS_SOUND);
    analogSignal[SOUND_CONTROLLER4] = km21KR2->getSoundSignal(Km21KR2::MAIN_CHANGE_POS_SOUND);
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
    // ЭПК
    analogSignal[SOUND_EPK_KEY] = key_epk.getSoundSignal();
    analogSignal[SOUND_EPK_WHISTLE] = epk->getSoundSignal();

    // Токоприёмники
    analogSignal[SOUND_PANT_BWD_UP] = pantographs[1]->getSoundSignal(Pantograph::UP_SOUND);
    analogSignal[SOUND_PANT_BWD_DOWN] = pantographs[1]->getSoundSignal(Pantograph::DOWN_SOUND);
    analogSignal[SOUND_PANT_FWD_UP] = pantographs[0]->getSoundSignal(Pantograph::UP_SOUND);
    analogSignal[SOUND_PANT_FWD_DOWN] = pantographs[0]->getSoundSignal(Pantograph::DOWN_SOUND);
    // Главный выключатель
    analogSignal[SOUND_BV_ON] = bv->getSoundSignal(ProtectiveDevice::ON_SOUND);
    analogSignal[SOUND_BV_OFF] = bv->getSoundSignal(ProtectiveDevice::OFF_SOUND);

    // Мотор-компрессоры
    analogSignal[SOUND_COMPRESSOR_1] = motor_compressor[0]->getSoundSignal();
    analogSignal[SOUND_COMPRESSOR_2] = motor_compressor[1]->getSoundSignal();
    // Мотор-вентиляторы
    analogSignal[SOUND_FAN_1] = motor_fan[0]->getSoundSignal();
    analogSignal[SOUND_FAN_2] = motor_fan[1]->getSoundSignal();
    analogSignal[SOUND_FAN_PTR] = motor_fan_ptr->getSoundSignal();

    // Переключатели
    // Верхний ряд
    analogSignal[SOUND_SWITCHER_EPB] = epb_switch.getSoundSignal();
    analogSignal[SOUND_SWITCHER_COMPR_1] = mk_switcher[0]->getSoundSignal();
    analogSignal[SOUND_SWITCHER_PANT_F] = pantoSwitcher[0]->getSoundSignal();
    analogSignal[SOUND_SWITCHER_GV] = fastSwitchSw->getSoundSignal();
    // Средний ряд
    analogSignal[SOUND_SWITCHER_FANS] = motor_fan_switcher->getSoundSignal();
    analogSignal[SOUND_SWITCHER_COMPR_2] = mk_switcher[1]->getSoundSignal();
    analogSignal[SOUND_SWITCHER_PANT_B] = pantoSwitcher[1]->getSoundSignal();
    analogSignal[SOUND_SWITCHER_BLINDS] = blindsSwitcher->getSoundSignal();
    // Нижний ряд - не реализован
    //analogSignal[SOUND_SWITCHER_LIGHT];
    //analogSignal[SOUND_SWITCHER_BUF_LIGHT_L];
    //analogSignal[SOUND_SWITCHER_BUF_LIGHT_R];
    //analogSignal[SOUND_SWITCHER_PROJECTOR];
    // Выключатель ЭДТ
    analogSignal[SOUND_SWITCHER_EDT] = EDTSwitch.getSoundSignal();


/* ============ VL60 ==================

    // Тяговые электродвигатели
    analogSignal[SOUND_TRACTION_ELETROENGINE_1] = motor[TED1]->getSoundSignal();
    analogSignal[SOUND_TRACTION_ELETROENGINE_2] = motor[TED2]->getSoundSignal();
    analogSignal[SOUND_TRACTION_ELETROENGINE_3] = motor[TED3]->getSoundSignal();
    analogSignal[SOUND_TRACTION_ELETROENGINE_4] = motor[TED4]->getSoundSignal();
    analogSignal[SOUND_TRACTION_ELETROENGINE_5] = motor[TED5]->getSoundSignal();
    analogSignal[SOUND_TRACTION_ELETROENGINE_6] = motor[TED6]->getSoundSignal();
*/
}
