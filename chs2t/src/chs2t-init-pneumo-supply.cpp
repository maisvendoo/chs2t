#include    "chs2t.h"

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
void CHS2T::initPneumoSupply(const QString &modules_dir, const QString &custom_cfg_dir)
{
    (void) modules_dir;
    (void) custom_cfg_dir;

    // Регулятор давления в ГР
    press_reg = new PressureRegulator();
    press_reg->read_config("pressure-regulator");

    // Мотор-компрессор
    for (size_t i = 0; i < motor_compressor.size(); ++i)
    {
        motor_compressor[i] = new DCMotorCompressor();
        motor_compressor[i]->read_config("motor-compressor-dc");

        mk_switcher[i] = new Switcher(Q_NULLPTR, 0, 4);
    }

    mk_switcher[0]->setKeyCode(KEY_7);
    mk_switcher[1]->setKeyCode(KEY_8);

    // Главный резервуар
    double volume_main = 1.0;
    main_reservoir = new Reservoir(volume_main);
    main_reservoir->setLeakCoeff(1e-6);

    // Концевые краны питательной магистрали
    anglecock_fl_fwd = new PneumoAngleCock();
    //anglecock_fl_fwd->setKeyCode(0);
    anglecock_fl_fwd->read_config("pneumo-anglecock-FL");

    anglecock_fl_bwd = new PneumoAngleCock();
    //anglecock_fl_bwd->setKeyCode(0);
    anglecock_fl_bwd->read_config("pneumo-anglecock-FL");

    // Рукава питательной магистрали
    hose_fl_fwd = new PneumoHose();
    //hose_fl_fwd->setKeyCode(0);
    hose_fl_fwd->read_config("pneumo-hose-FL");
    forward_connectors.push_back(hose_fl_fwd);

    hose_fl_bwd = new PneumoHose();
    //hose_fl_bwd->setKeyCode(0);
    hose_fl_bwd->read_config("pneumo-hose-FL");
    backward_connectors.push_back(hose_fl_bwd);
}
