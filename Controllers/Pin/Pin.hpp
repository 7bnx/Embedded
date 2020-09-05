//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Pin common interface
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _PIN_HPP
#define _PIN_HPP

#include <cstdint>
#include "interface.hpp"
#include "type_traits_custom.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller::common{

/*!
  @brief Pin driver. Inherited from controller-specific interface
  @tparam <gpioBase> Base address of port (E.g.: GPIOA_BASE)
  @tparam <pinNumber> Number of pin
  @tparam <config> Configuration for pin
*/ 
template<typename implementation>
class Pin{
public:

  /*!
    @brief Interface
  */
  static constexpr auto interface = controller::interface::parallel;

  /*!
    @brief Initialization of pin
  */
  static void Init(){ implementation::_Init(); }

  /*!
    @brief Reset pin to default value
  */ 
  static void Reset(){ implementation::_Reset(); }

  /*!
    @brief Configure pin to low power mode
  */
  static void ToLowPowerMode(){ implementation::_ToLowPowerMode(); }

  /*!
    @brief Set pin output to High level
  */ 
  static void High(){ implementation::_High(); }

  /*!
    @brief Set pin output to Low level
  */
  static void Low(){ implementation::_Low(); }

  /*!
    @brief Get current input pin level
  */
  static bool Get(){ return implementation::_Get(); }

  /*!
    @brief Set output level for pin
  */
  static void Set(bool state){ implementation::_Set(state); }

  /*!
    @brief Toggle current state of pin
  */
  static void Toggle(){ implementation::_Toggle(); }

private:

  friend implementation;

};

} //!namespace controller

#endif //!_PIN_HPP