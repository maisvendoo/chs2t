#include    "chs2t.h"

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
void CHS2T::initControl(const QString &modules_dir, const QString &custom_cfg_dir)
{
    (void) modules_dir;
    (void) custom_cfg_dir;

    // Расцепные рычаги
    oper_rod_fwd->setKeySymbol(KEY_X);
    oper_rod_fwd->setControl(&pressed_keys);

    oper_rod_bwd->setKeySymbol(KEY_C);
    oper_rod_bwd->setControl(&pressed_keys);

    // Концевые краны питательной магистрали
    anglecock_fl_fwd->setKeySymbolOpen(KEY_F6);
    anglecock_fl_fwd->setKeyModifierOpen(MODIFIER_OnlyShift);
    anglecock_fl_fwd->setKeySymbolClose(KEY_F6);
    anglecock_fl_fwd->setKeyModifierClose(MODIFIER_OnlyControl);
    anglecock_fl_fwd->setControl(&pressed_keys);

    anglecock_fl_bwd->setKeySymbolOpen(KEY_F7);
    anglecock_fl_bwd->setKeyModifierOpen(MODIFIER_OnlyShift);
    anglecock_fl_bwd->setKeySymbolClose(KEY_F7);
    anglecock_fl_bwd->setKeyModifierClose(MODIFIER_OnlyControl);
    anglecock_fl_bwd->setControl(&pressed_keys);

    // Рукава питательной магистрали
    hose_fl_fwd->setKeySymbolConnect(KEY_F5);
    hose_fl_fwd->setKeyModifierConnect(MODIFIER_OnlyShift);
    hose_fl_fwd->setKeySymbolDisconnect(KEY_F5);
    hose_fl_fwd->setKeyModifierDisconnect(MODIFIER_OnlyControl);
    hose_fl_fwd->setControl(&pressed_keys);

    hose_fl_bwd->setKeySymbolConnect(KEY_F8);
    hose_fl_bwd->setKeyModifierConnect(MODIFIER_OnlyShift);
    hose_fl_bwd->setKeySymbolDisconnect(KEY_F8);
    hose_fl_bwd->setKeyModifierDisconnect(MODIFIER_OnlyControl);
    hose_fl_bwd->setControl(&pressed_keys);

    // Концевые краны тормозной магистрали
    anglecock_bp_fwd->setKeySymbolOpen(KEY_F2);
    anglecock_bp_fwd->setKeyModifierOpen(MODIFIER_OnlyShift);
    anglecock_bp_fwd->setKeySymbolClose(KEY_F2);
    anglecock_bp_fwd->setKeyModifierClose(MODIFIER_OnlyControl);
    anglecock_bp_fwd->setControl(&pressed_keys);

    anglecock_bp_bwd->setKeySymbolOpen(KEY_F3);
    anglecock_bp_bwd->setKeyModifierOpen(MODIFIER_OnlyShift);
    anglecock_bp_bwd->setKeySymbolClose(KEY_F3);
    anglecock_bp_bwd->setKeyModifierClose(MODIFIER_OnlyControl);
    anglecock_bp_bwd->setControl(&pressed_keys);

    // Рукава тормозной магистрали
    hose_bp_fwd->setKeySymbolConnect(KEY_F1);
    hose_bp_fwd->setKeyModifierConnect(MODIFIER_OnlyShift);
    hose_bp_fwd->setKeySymbolDisconnect(KEY_F1);
    hose_bp_fwd->setKeyModifierDisconnect(MODIFIER_OnlyControl);
    hose_bp_fwd->setControl(&pressed_keys);

    hose_bp_bwd->setKeySymbolConnect(KEY_F4);
    hose_bp_bwd->setKeyModifierConnect(MODIFIER_OnlyShift);
    hose_bp_bwd->setKeySymbolDisconnect(KEY_F4);
    hose_bp_bwd->setKeyModifierDisconnect(MODIFIER_OnlyControl);
    hose_bp_bwd->setControl(&pressed_keys);

    // Песочница
    sand_system->setControl(&pressed_keys);


    for (auto cab_idx : {CAB1, CAB2})
    {
        // Электропневматический клапан автостопа
        epk[cab_idx]->setControl(&pressed_keys_by_cabine[cab_idx]);

        // Контроллер машиниста
        km21KR2[cab_idx]->setControl(&pressed_keys_by_cabine[cab_idx]/*, (cab_idx == CAB1) ? &control_signals : nullptr*/);

        // Панель переключателей
        sw_panel[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]/*, (cab_idx == CAB1) ? &control_signals : nullptr*/);

        // Рукоятка задатчика тормозного усилия
        handleEDT[cab_idx]->setControl(&pressed_keys_by_cabine[cab_idx]/*, (cab_idx == CAB1) ? &control_signals : nullptr*/);

        // Тифон, свисток
        horn[cab_idx]->setControl(&pressed_keys_by_cabine[cab_idx]);

        // Кнопка "Свисток"
        button_svistok[cab_idx].setKeySymbolOn(KEY_Space);
        button_svistok[cab_idx].setKeyModifierOn(ANY_MODIFIERS);
        button_svistok[cab_idx].setKeySymbolOff(KEY_Undefined);
        button_svistok[cab_idx].setKeyModifierOff(KEY_Undefined);
        button_svistok[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);

        // Кнопка "Тифон"
        button_tifon[cab_idx].setKeySymbolOn(KEY_B);
        button_tifon[cab_idx].setKeyModifierOn(ANY_MODIFIERS);
        button_tifon[cab_idx].setKeySymbolOff(KEY_Undefined);
        button_tifon[cab_idx].setKeyModifierOff(KEY_Undefined);
        button_tifon[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);

        // Кнопка "Песок"
        button_sand[cab_idx].setKeySymbolOn(KEY_Delete);
        button_sand[cab_idx].setKeyModifierOn(ANY_MODIFIERS);
        button_sand[cab_idx].setKeySymbolOff(KEY_Undefined);
        button_sand[cab_idx].setKeyModifierOff(KEY_Undefined);
        button_sand[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);

        // Кнопка "Отпуск электровоза"
        button_loco_release[cab_idx].setKeySymbolOn(KEY_Slash);
        button_loco_release[cab_idx].setKeyModifierOn(ANY_MODIFIERS);
        button_loco_release[cab_idx].setKeySymbolOff(KEY_Undefined);
        button_loco_release[cab_idx].setKeyModifierOff(KEY_Undefined);
        button_loco_release[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);

        // Кнопка "Сброс СП-С"
        button_sbros_cpc[cab_idx].setKeySymbolOn(KEY_R);
        button_sbros_cpc[cab_idx].setKeyModifierOn(ANY_MODIFIERS);
        button_sbros_cpc[cab_idx].setKeySymbolOff(KEY_Undefined);
        button_sbros_cpc[cab_idx].setKeyModifierOff(KEY_Undefined);
        button_sbros_cpc[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);

        // Триггеры рукояток бдительности
        rb[cab_idx][RBS].setKeySymbolOn(KEY_M);
        rb[cab_idx][RBS].setKeyModifierOn(ANY_MODIFIERS);
        rb[cab_idx][RBS].setKeySymbolOff(KEY_Undefined);
        rb[cab_idx][RBS].setKeyModifierOff(KEY_Undefined);
        rb[cab_idx][RBS].setControl(&pressed_keys_by_cabine[cab_idx]);

        rb[cab_idx][RB1].setKeySymbolOn(KEY_Z);
        rb[cab_idx][RB1].setKeyModifierOn(ANY_MODIFIERS);
        rb[cab_idx][RB1].setKeySymbolOff(KEY_Undefined);
        rb[cab_idx][RB1].setKeyModifierOff(KEY_Undefined);
        rb[cab_idx][RB1].setControl(&pressed_keys_by_cabine[cab_idx]);

        rb[cab_idx][RBP].setKeySymbolOn(KEY_Tilde);
        rb[cab_idx][RBP].setKeyModifierOn(ANY_MODIFIERS);
        rb[cab_idx][RBP].setKeySymbolOff(KEY_Undefined);
        rb[cab_idx][RBP].setKeyModifierOff(KEY_Undefined);
        rb[cab_idx][RBP].setControl(&pressed_keys_by_cabine[cab_idx]);

// Удалить после переписывания на sw_panel
        // Тумблер включения ЭПТ
        epb_switch[cab_idx].setKeySymbolOn(KEY_V);
        epb_switch[cab_idx].setKeyModifierOn(MODIFIER_OnlyShift);
        epb_switch[cab_idx].setKeySymbolOff(KEY_V);
        epb_switch[cab_idx].setKeyModifierOff(MODIFIER_OnlyControl);
        epb_switch[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);

        // Галетник управления мотор-вентиляторами
        motor_fan_switcher[cab_idx].setNumPositions(3);
        motor_fan_switcher[cab_idx].setKeySymbolIncrease(KEY_1);
        motor_fan_switcher[cab_idx].setKeyModifierIncrease(MODIFIER_OnlyShift);
        motor_fan_switcher[cab_idx].setKeySymbolDecrease(KEY_1);
        motor_fan_switcher[cab_idx].setKeyModifierDecrease(MODIFIER_OnlyControl);
        motor_fan_switcher[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);
        motor_fan_switcher[cab_idx].setInitPosition(0);

        // Галетники управления мотор-компрессорами
        mk_switcher[cab_idx][MK1].setNumPositions(4);
        mk_switcher[cab_idx][MK1].setKeySymbolIncrease(KEY_2);
        mk_switcher[cab_idx][MK1].setKeyModifierIncrease(MODIFIER_OnlyShift);
        mk_switcher[cab_idx][MK1].setKeySymbolDecrease(KEY_2);
        mk_switcher[cab_idx][MK1].setKeyModifierDecrease(MODIFIER_OnlyControl);
        mk_switcher[cab_idx][MK1].setControl(&pressed_keys_by_cabine[cab_idx]);
        mk_switcher[cab_idx][MK1].setInitPosition(0);

        mk_switcher[cab_idx][MK2].setNumPositions(4);
        mk_switcher[cab_idx][MK2].setKeySymbolIncrease(KEY_3);
        mk_switcher[cab_idx][MK2].setKeyModifierIncrease(MODIFIER_OnlyShift);
        mk_switcher[cab_idx][MK2].setKeySymbolDecrease(KEY_3);
        mk_switcher[cab_idx][MK2].setKeyModifierDecrease(MODIFIER_OnlyControl);
        mk_switcher[cab_idx][MK2].setControl(&pressed_keys_by_cabine[cab_idx]);
        mk_switcher[cab_idx][MK2].setInitPosition(0);

        // Галетники управления токоприемниками
        pant_switcher[cab_idx][PANT1].setNumPositions(4);
        pant_switcher[cab_idx][PANT1].setKeySymbolIncrease(KEY_I);
        pant_switcher[cab_idx][PANT1].setKeyModifierIncrease(MODIFIER_OnlyShift);
        pant_switcher[cab_idx][PANT1].setKeySymbolDecrease(KEY_I);
        pant_switcher[cab_idx][PANT1].setKeyModifierDecrease(MODIFIER_OnlyControl);
        pant_switcher[cab_idx][PANT1].setControl(&pressed_keys_by_cabine[cab_idx]);
        pant_switcher[cab_idx][PANT1].setSpringLast();
        pant_switcher[cab_idx][PANT1].setInitPosition(0);

        pant_switcher[cab_idx][PANT2].setNumPositions(4);
        pant_switcher[cab_idx][PANT2].setKeySymbolIncrease(KEY_O);
        pant_switcher[cab_idx][PANT2].setKeyModifierIncrease(MODIFIER_OnlyShift);
        pant_switcher[cab_idx][PANT2].setKeySymbolDecrease(KEY_O);
        pant_switcher[cab_idx][PANT2].setKeyModifierDecrease(MODIFIER_OnlyControl);
        pant_switcher[cab_idx][PANT2].setControl(&pressed_keys_by_cabine[cab_idx]);
        pant_switcher[cab_idx][PANT2].setSpringLast();
        pant_switcher[cab_idx][PANT2].setInitPosition(0);

        // Галетник управления БВ
        fastswitch_switcher[cab_idx].setNumPositions(4);
        fastswitch_switcher[cab_idx].setKeySymbolIncrease(KEY_P);
        fastswitch_switcher[cab_idx].setKeyModifierIncrease(MODIFIER_OnlyShift);
        fastswitch_switcher[cab_idx].setKeySymbolDecrease(KEY_P);
        fastswitch_switcher[cab_idx].setKeyModifierDecrease(MODIFIER_OnlyControl);
        fastswitch_switcher[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);
        fastswitch_switcher[cab_idx].setSpringLast();
        fastswitch_switcher[cab_idx].setInitPosition(0);

        // Галетник управления жалюзи
        blinds_switcher[cab_idx].setNumPositions(5);
        blinds_switcher[cab_idx].setKeySymbolIncrease(KEY_4);
        blinds_switcher[cab_idx].setKeyModifierIncrease(MODIFIER_OnlyShift);
        blinds_switcher[cab_idx].setKeySymbolDecrease(KEY_4);
        blinds_switcher[cab_idx].setKeyModifierDecrease(MODIFIER_OnlyControl);
        blinds_switcher[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);
        blinds_switcher[cab_idx].setInitPosition(0);

        // Галетник управления освещением кабины и приборов
        cab_light_switcher[cab_idx].setNumPositions(5);
        cab_light_switcher[cab_idx].setKeySymbolIncrease(KEY_K);
        cab_light_switcher[cab_idx].setKeyModifierIncrease(MODIFIER_OnlyShift);
        cab_light_switcher[cab_idx].setKeySymbolDecrease(KEY_K);
        cab_light_switcher[cab_idx].setKeyModifierDecrease(MODIFIER_OnlyControl);
        cab_light_switcher[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);
        cab_light_switcher[cab_idx].setInitPosition(2);

        // Галетник управления левым буферным огнём
        bufferlight_L_switcher[cab_idx].setNumPositions(3);
        bufferlight_L_switcher[cab_idx].setKeySymbolIncrease(KEY_G);
        bufferlight_L_switcher[cab_idx].setKeyModifierIncrease(MODIFIER_OnlyShift);
        bufferlight_L_switcher[cab_idx].setKeySymbolDecrease(KEY_G);
        bufferlight_L_switcher[cab_idx].setKeyModifierDecrease(MODIFIER_OnlyControl);
        bufferlight_L_switcher[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);
        bufferlight_L_switcher[cab_idx].setInitPosition(1);

        // Галетник управления правым буферным огнём
        bufferlight_R_switcher[cab_idx].setNumPositions(3);
        bufferlight_R_switcher[cab_idx].setKeySymbolIncrease(KEY_J);
        bufferlight_R_switcher[cab_idx].setKeyModifierIncrease(MODIFIER_OnlyShift);
        bufferlight_R_switcher[cab_idx].setKeySymbolDecrease(KEY_J);
        bufferlight_R_switcher[cab_idx].setKeyModifierDecrease(MODIFIER_OnlyControl);
        bufferlight_R_switcher[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);
        bufferlight_R_switcher[cab_idx].setInitPosition(1);

        // Галетник управления прожектором
        spotlight_switcher[cab_idx].setNumPositions(3);
        spotlight_switcher[cab_idx].setKeySymbolIncrease(KEY_H);
        spotlight_switcher[cab_idx].setKeyModifierIncrease(MODIFIER_OnlyShift);
        spotlight_switcher[cab_idx].setKeySymbolDecrease(KEY_H);
        spotlight_switcher[cab_idx].setKeyModifierDecrease(MODIFIER_OnlyControl);
        spotlight_switcher[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);
        spotlight_switcher[cab_idx].setInitPosition(0);

        // Выключатель ЭДТ
        EDT_switch[cab_idx].setKeySymbolOn(KEY_F);
        EDT_switch[cab_idx].setKeyModifierOn(MODIFIER_OnlyShift);
        EDT_switch[cab_idx].setKeySymbolOff(KEY_F);
        EDT_switch[cab_idx].setKeyModifierOff(MODIFIER_OnlyControl);
        EDT_switch[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]);
    }
}
