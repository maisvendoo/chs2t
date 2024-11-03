#include "motor.h"

#include    "filesystem.h"

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
Motor::Motor(QObject* parent) : Device(parent)
  , poz(0)
  , n(1)
  , beta(1)
  , R_a(0.0)
  , R_gp(0.0)
  , R_dp(0.0)
  , R_r(0.0)
  , L_af(0.0)
  , omega(0.0)
  , torque(0.0)
  , U(0.0)
  , direction(1)
  , torque_max(10000.0)
  , omega_nom(50.0)
{

}

//------------------------------------------------------------------------------
// Деструктор
//------------------------------------------------------------------------------
Motor::~Motor()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Motor::setBetaStep(int step)
{
    if (fieldStep.contains(step))
    {
        setBeta(fieldStep[step]);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Motor::setDirection(int revers_state)
{
    if (revers_state == 0)
        return;
    else
        direction = revers_state;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double Motor::getI12() const
{
    return getIa() * static_cast<double>(amp_state.is12on);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double Motor::getI34() const
{
    return getIa() * static_cast<double>(amp_state.is34on);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double Motor::getI56() const
{
    return getIa() * static_cast<double>(amp_state.is56on);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
sound_state_t Motor::getSoundState(size_t idx) const
{
    (void) idx;
    return sound_state;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float Motor::getSoundSignal(size_t idx) const
{
    (void) idx;
    return sound_state.createSoundSignal();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Motor::preStep(state_vector_t& Y, double t)
{
    Q_UNUSED(t)

    torque = calcCPhi(Y[0] * beta * direction) * Y[0];

    if (poz < 21)
        n = 6;
    else
    {
        if (poz < 34)
            n = 3;
        else
            n = 2;
    }

    // Озвучка
    // Относительная громкость пропорциональна крутящему моменту
    double relative_volume = abs(torque) / torque_max;
    // Плавное включение на низкой скорости
    relative_volume = relative_volume * max(abs(omega) / 2.0, 1.0);
    sound_state.volume = static_cast<float>(relative_volume);

    // Относительная частота вращения от номинальной, для которой записан звук
    double relative_pitch = abs(omega) / omega_nom;
    // Костыль - нелинейное преобразование частоты (0.5 x^2 + 0.5),
    // чтобы охватить низкие скорости с частотой хотя бы 0.5
    if (relative_pitch < 1.0)
        relative_pitch = 0.5 * relative_pitch * relative_pitch + 0.5;
    sound_state.pitch = static_cast<float>(relative_pitch);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Motor::ode_system(const state_vector_t& Y, state_vector_t& dYdt, double t)
{
    Q_UNUSED(t)

    double R;
    R = R_r / n + R_a + R_gp * beta + R_dp;
    dYdt[0] = (U / n - R * Y[0] - calcCPhi(Y[0] * beta * direction) * omega) / L_af;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Motor::load_config(CfgReader& cfg)
{
    QString secName = "Device";

    cfg.getDouble(secName, "R_a", R_a);
    cfg.getDouble(secName, "R_gp", R_gp);
    cfg.getDouble(secName, "R_dp", R_dp);
    cfg.getDouble(secName, "R_r", R_r);
    cfg.getDouble(secName, "L_af", L_af);
    cfg.getDouble(secName, "TorqueMax", torque_max);
    cfg.getDouble(secName, "OmegaNom", omega_nom);

    QString cPhiFileName = "";

    cfg.getString(secName, "cPhi", cPhiFileName);

    FileSystem &fs = FileSystem::getInstance();
    QString cfg_dir(fs.getVehiclesDir().c_str());
    cfg_dir += QDir::separator() + custom_cfg_dir;
    cPhi.load((cfg_dir + QDir::separator() + cPhiFileName).toStdString());

    QDomNode secNode;

    secNode = cfg.getFirstSection("FieldPos");

    while (!secNode.isNull())
    {
        double field_step = 1.0;
        int number = 0;

        cfg.getInt(secNode, "Number", number);
        cfg.getDouble(secNode, "beta", field_step);

        fieldStep.insert(number, field_step);

        secNode = cfg.getNextSection();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double Motor::calcCPhi(double I)
{
    return 1.2 * cPhi.getValue(I);
}
