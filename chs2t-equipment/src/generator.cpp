#include "generator.h"

#include    "filesystem.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Generator::Generator(QObject* parent) : Device(parent)
  , Uf(0.0)
  , Lf(0.01)
  , Rf(0.0)
  , omega(0.0)
  , E(0.0)
  , La(0.01)
  , Ra(0.0)
  , Rt(0.0)
  , M(0.0)
  , Ut(0.0)
  , Rgp(0.0)
  , Rdp(0.0)
  , torque_max(2000.0)
  , omega_nom(50.0)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Generator::~Generator()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
sound_state_t Generator::getSoundState(size_t idx) const
{
    (void) idx;
    return sound_state;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
float Generator::getSoundSignal(size_t idx) const
{
    (void) idx;
    return sound_state.createSoundSignal();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Generator::preStep(state_vector_t& Y, double t)
{
    Q_UNUSED(t)

    M = calcCPhi((Y[0])) * Y[1];
    Ut = Y[1] * Rt;

    // Озвучка
    // Относительная громкость пропорциональна крутящему моменту
    double relative_volume = abs(M) / torque_max;
    // Плавное включение на низкой скорости
    // Не нужно, на низких скоростях ЭДТ отключено
    //relative_volume = relative_volume * max(abs(omega) / 2.0, 1.0);
    sound_state.volume = static_cast<float>(relative_volume);

    // Относительная частота вращения от номинальной, для которой записан звук
    double relative_pitch = abs(omega) / omega_nom;
    // Костыль - нелинейное преобразование частоты (0.5 x^2 + 0.5),
    // чтобы охватить низкие скорости с частотой хотя бы 0.5
    // Не нужно, на низких скоростях ЭДТ отключено
    //relative_pitch = 0.5 * relative_pitch * relative_pitch + 0.5;
    sound_state.pitch = static_cast<float>(relative_pitch);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Generator::ode_system(const state_vector_t& Y, state_vector_t& dYdt, double t)
{
    Q_UNUSED(t)

    E = calcCPhi(Y[0]) * omega;

    dYdt[0] = (Uf - Y[0] * 6 * Rf) / Lf;

    dYdt[1] = -(E + Y[1] * (Ra + Rt)) / La;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Generator::load_config(CfgReader& cfg)
{
    QString secName = "Device";

    cfg.getDouble(secName, "R_a", Ra);
    cfg.getDouble(secName, "R_gp", Rgp);
    cfg.getDouble(secName, "R_dp", Rdp);
    cfg.getDouble(secName, "TorqueMax", torque_max);
    cfg.getDouble(secName, "OmegaNom", omega_nom);

    Rf = Rgp + Rdp;

    QString cPhiFileName = "";

    cfg.getString(secName, "cPhi", cPhiFileName);

    FileSystem &fs = FileSystem::getInstance();
    QString cfg_dir(fs.getVehiclesDir().c_str());
    cfg_dir += QDir::separator() + custom_cfg_dir;
    cPhi.load((cfg_dir + QDir::separator() + cPhiFileName).toStdString());
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double Generator::calcCPhi(double I)
{
    return 1.2 * cPhi.getValue(I);
}
