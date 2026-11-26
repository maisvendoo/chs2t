#include    "chs2t-iocontroller.h"

#include    <core/get_module.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CHS2tIOController::CHS2tIOController(QObject *parent) : IOController(parent)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CHS2tIOController::~CHS2tIOController()
{

}

GET_MODULE(CHS2tIOController)
