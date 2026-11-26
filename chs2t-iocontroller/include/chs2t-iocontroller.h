#ifndef     CHS2T_IO_CONTROLLER_H
#define     CHS2T_IO_CONTROLLER_H

#include    <io-controller.h>

//------------------------------------------------------------------------------
//  Модуль обработки ввода пользователя ЧС2т на базе IOController.
//  Наследует всю стандартную логику обработчиков (Toggle/Button/Switcher/Axis).
//------------------------------------------------------------------------------
class CHS2tIOController : public IOController
{
    Q_OBJECT

public:

    CHS2tIOController(QObject *parent = nullptr);

    ~CHS2tIOController() override;
};

#endif // CHS2T_IO_CONTROLLER_H
