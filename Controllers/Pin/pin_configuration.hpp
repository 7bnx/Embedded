//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Configuration parameters for controller's Pin
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _PIN_CONFIGURATION_HPP
#define _PIN_CONFIGURATION_HPP

#include "controller_define.hpp"
#include "type_traits_custom.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief Possible configurations of Pin. Depends on controller
*/ 
enum class pinConfiguration {
#ifdef STM32F1

  /*! @brief Float input.*/ 
  Input_Float = 0x04,

  /*! @brief Digital input with internal pull-up*/ 
  Input_PullUp = 0x18,

  /*! @brief Digital input with internal pull-down*/ 
  Input_PullDown = 0x08,

  /*! @brief Analog input (Low Power)*/ 
  Input_Analog = 0x00,

  /*! @brief Open-drain output with 2MHz fronts*/ 
  Output_OpenDrain_2MHz = 0x06,

  /*! @brief Open-drain output with 10MHz fronts*/ 
  Output_OpenDrain_10MHz = 0x05,

  /*! @brief Open-drain output with 50MHz fronts*/ 
  Output_OpenDrain_50MHz = 0x07,

  /*! @brief Digital output with 2MHz fronts. Set to High level*/ 
  Output_High_2MHz = 0x12,

  /*! @brief Digital output with 10MHz fronts. Set to High level*/ 
  Output_High_10MHz = 0x11,

  /*! @brief Digital output with 50MHz fronts. Set to High level*/ 
  Output_High_50MHz = 0x13,

  /*! @brief Digital output with 2MHz fronts. Set to Low level*/ 
  Output_Low_2MHz = 0x02,

  /*! @brief Digital output with 10MHz fronts. Set to Low level*/ 
  Output_Low_10MHz = 0x01,

  /*! @brief Digital output with 50MHz fronts. Set to Low level*/ 
  Output_Low_50MHz = 0x03,

  /*! @brief Input interrupt with Rising trigger*/ 
  Interrupt_Rising = 0x108,

  /*! @brief Input interrupt with Falling trigger*/ 
  Interrupt_Falling = 0x118,

  /*! @brief Input event with Rising trigger*/ 
  Event_Rising = 0x208,

  /*! @brief Input event with Falling trigger*/ 
  Event_Falling = 0x218,

  /*! @brief Push-pull output for alternative function (USART...)*/ 
  Alternative_PushPull = 0x0B,

  /*! @brief Open-drain output for alternative function (I2C...)*/ 
  Alternative_OpenDrain = 0x0F,

  /*! @brief Mask to clear pin's configuration*/ 
  ClearMask = 0x0F
#endif
};

/*!
  @brief Classes with common interface
*/
namespace abstarct{

/*!
  @brief Pin driver. Inherited from controller-specific interface
  @tparam <gpioBase> Base address of port (E.g.: GPIOA_BASE)
  @tparam <pinNumber> Number of pin
  @tparam <config> Configuration for pin
*/ 
template<uint32_t gpioBase, uint8_t pinNumber, pinConfiguration config>
class Pin{
public:

  /*!
    @brief Get the base address of Pin
  */
  static constexpr auto baseAddress = gpioBase;

  /*!
    @brief Get the Pin number
  */
  static constexpr auto number = pinNumber;

  /*!
    @brief Get the configuration of pin
  */
  static constexpr auto configuration = config;

  /*!
    @brief Get pin's mask. E.g.: if number = 2, then mask = 0b100
  */
  static constexpr auto mask = 1U << pinNumber;

  /*!
    @brief Get list of types for usage in Power class
  */
  using power = std::conditional_t<
                                   config == pinConfiguration::Alternative_PushPull || 
                                   config == pinConfiguration::Alternative_OpenDrain, 
                                   utils::Typelist<utils::ctv<gpioBase>, utils::ctv<AFIO_BASE>>, 
                                   utils::Typelist<utils::ctv<gpioBase>>
                                 >;
  
  /*!
    @brief Get Pin type for usage in Pinlist
  */
  using pins = utils::Typelist<Pin<gpioBase, pinNumber, config>>;
};

} // !namespace abstarct

} //!namespace controller

#endif //!_PIN_CONFIGURATION_HPP