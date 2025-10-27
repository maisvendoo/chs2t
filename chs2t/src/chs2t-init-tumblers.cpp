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
        // Разобщительный кран усл.№328
        shutoff_crane[cab_idx]->setKeySymbolOpen(KEY_BackSpace);
        shutoff_crane[cab_idx]->setKeyModifierOpen(MODIFIER_OnlyAlt);
        shutoff_crane[cab_idx]->setKeySymbolClose(KEY_BackSpace);
        shutoff_crane[cab_idx]->setKeyModifierClose(MODIFIER_OnlyAlt);
        shutoff_crane[cab_idx]->setControl(&pressed_keys_by_cabine[cab_idx]);

        // Комбинированный кран усл.№114
        combine_crane[cab_idx]->setControl(&pressed_keys_by_cabine[cab_idx]);

        // Электропневматический клапан автостопа
        epk[cab_idx]->setControl(&pressed_keys_by_cabine[cab_idx]);

        // Контроллер машиниста
        km21KR2[cab_idx].setControl(&pressed_keys_by_cabine[cab_idx]/*, (cab_idx == CAB1) ? &control_signals : nullptr*/);

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
    }
}
