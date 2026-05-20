#include    <chs2t.h>

#include    <epb-2line-control.h>
#include    <core/load_module.h>

#include    <QDir>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::initAutopilot(const QString &modules_dir,
                          const QString &custom_cfg_dir)
{
    // Модули автоведения
    for (auto cab_idx : {CAB1, CAB2})
    {
        Autopilot* autopilot = LOAD_MODULE(Autopilot,
            modules_dir + QDir::separator() +
            custom_modules_dir + QDir::separator() +
            autopilot_module_name);

        if (autopilot != nullptr)
        {
            autopilot->setVehicleIndex(model_idx);
            autopilot->read_config(autopilot_config_name, custom_cfg_dir);
            autopilot->initAutoBrakeControl(autopilot_config_name, custom_cfg_dir);
            autopilot_switcher[cab_idx].setKeyModifierOn(MODIFIER_OnlyAlt);
            autopilot_switcher[cab_idx].setKeySymbolOn(KEY_F);
            autopilot_switcher[cab_idx].setKeyModifierOff(MODIFIER_OnlyAlt);
            autopilot_switcher[cab_idx].setKeySymbolOff(KEY_F);
            autopilot_switcher[cab_idx].setControl(&pressed_keys);

            auto_feedback[cab_idx] = new chs2t_feedback_t();
            autopilot->setFeedback(auto_feedback[cab_idx]);

            connect(autopilot, &Autopilot::sigInitTrainParams, this, &CHS2T::slotInitTrainForAutopilot);

            this->autopilot.push_back(autopilot);

            connect(pantCtrlTimer, &Timer::process, this, &CHS2T::slotPantCtrl);
        }
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::slotInitTrainForAutopilot()
{
    double train_len = 0;
    double train_mass = 0;

    emit sigGetTrainParams(train_idx, train_len, train_mass);

    for (auto cab_idx : {CAB1, CAB2})
    {
        autopilot[cab_idx]->setTrainLength(train_len);
        autopilot[cab_idx]->setTrainMass(train_mass);
    }

    // Кое-какие другие действия при активации автоведения
    if (km21KR2[CAB1].isReversHandle())
    {
        prepareCabineForAutopilot(CAB1, CAB2);
    }

    if (km21KR2[CAB2].isReversHandle())
    {
        prepareCabineForAutopilot(CAB2, CAB1);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::prepareCabineForAutopilot(int my_cab_idx, int other_cab_idx)
{
    // В нашей кабине

    // включаем ЭПТ
    if (!epb_control->stateReleaseLamp())
    {
        sw_panel[my_cab_idx].getSwitcherPtr(CHS2tSwitchers::EPB)->setPosition(CHS2tSwitchers::EPB_ON);
    }

    // Включаем буферные фонари
    sw_panel[my_cab_idx].getSwitcherPtr(CHS2tSwitchers::BUFLIGHT_L)->setPosition(CHS2tSwitchers::BUFLIGHT_WHITE);
    sw_panel[my_cab_idx].getSwitcherPtr(CHS2tSwitchers::BUFLIGHT_R)->setPosition(CHS2tSwitchers::BUFLIGHT_WHITE);

    // Включаем освещение приборов
    sw_panel[my_cab_idx].getSwitcherPtr(CHS2tSwitchers::CAB_LIGHT)->setPosition(CHS2tSwitchers::CAB_LIGHT_DEVICES_HIGH);

    // В другой кабине
    sw_panel[other_cab_idx].getSwitcherPtr(CHS2tSwitchers::BUFLIGHT_L)->setPosition(CHS2tSwitchers::BUFLIGHT_WHITE);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::OnAutopilot()
{
    Vehicle::OnAutopilot();

    // Делаем автозапуск
    if (km21KR2[CAB1].isReversHandle())
    {
        initAutostartProgram(CAB1);
    }

    if (km21KR2[CAB2].isReversHandle())
    {
        initAutostartProgram(CAB2);
    }

    autoStartTimer->start();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS2T::OffAutopilot()
{
    for (auto cab_idx : {CAB1, CAB2})
    {
        autopilot_switcher[cab_idx].reset();
    }
}
