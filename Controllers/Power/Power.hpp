//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Power configuration for controllers
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _POWER_HPP
#define _POWER_HPP

#include <cstdint>
#include "controller_define.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief Enable or Disable peripheral Power. Inherited from controller-specific interface
*/  
class Power: public hardware::_PowerImplementation{

private:
  Power() = delete;
};

} // !namespace controller

#endif // !_POWER_HPP