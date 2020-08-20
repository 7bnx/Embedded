//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Pin object wrapper for hardware implementation
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _PIN_HPP
#define _PIN_HPP

#include <cstdint>
#include "controller_define.hpp"
#include "pin_configuration.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief Pin driver. Inherited from controller-specific interface
  @tparam <gpioBase> Base address of port (E.g.: GPIOA_BASE)
  @tparam <pinNumber> Number of pin
  @tparam <config> Configuration for pin
*/ 
template<uint32_t gpioBase, uint8_t pinNumber, controller::pinConfiguration config>
class Pin: public hardware::_PinImplementation<gpioBase, pinNumber, config>{

private:

};

} //!namespace controller

#endif //!_PIN_HPP