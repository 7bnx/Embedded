//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Clock configuration for controllers
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _CLOCK_HPP
#define _CLOCK_HPP

#include <cstdint>
#include "controller_define.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief Set and get system and bus clock. Inherited from controller-specific interface
*/  
class Clock: public hardware::_ClockImplementation{

private:
  Clock() = delete;
};

} // !namespace controller

#endif //!_CLOCK_HPP