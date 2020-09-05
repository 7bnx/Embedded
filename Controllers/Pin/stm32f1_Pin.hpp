//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Hardware-dependent library for Pin driver 
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _STM32F1_PIN_HPP
#define _STM32F1_PIN_HPP

#include <cstdint>
#include "stm32f10x.h"
#include "stm32f1_Power.hpp"
#include "pin.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief Possible configurations of Pin. Depends on controller
*/ 
enum class pinConfiguration {

  /*! @brief Float input*/ 
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

};

/*!
  @brief Pin Driver for STM32F1 series. Don't use it Directly
  @tparam <gpioBase> base address of Port. E.g.: GPIOA_BASE
  @tparam <pinNumber> number of pin. From 0 to 15
  @tparam <config> Configuration for pin
*/ 
template<uint32_t gpioBase, uint8_t pinNumber, controller::pinConfiguration config>
class Pin: public common::Pin<Pin<gpioBase, pinNumber, config>>{

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

private:

  friend common::Pin<Pin<gpioBase, pinNumber, config>>;

  template<typename...>
  friend class Pinlist;

  friend Power;

  /*!
    @brief Get Pin type for usage in Pinlist
  */
  using pins = utils::Typelist<Pin<gpioBase, pinNumber, config>>;

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
    @brief Remap value
  */
  using remap = utils::ctv<0U>;

  /*!
    @brief Initialization of pin
  */ 
  static void _Init(){

    GPIO_CHx() = (GPIO_CHx() & (~configurationClear))| configurationValue;
    base()->ODR = (base()->ODR & (~mask)) | configurationValueODR;
    if constexpr(constexpr auto valueEMR = configurationValueEMR, valueIMR = configurationValueIMR; 
                 valueEMR || valueIMR){
      EXTI->EMR = (EXTI->EMR & (~valueIMR))|(valueEMR);
      EXTI->IMR = (EXTI->IMR & (~valueEMR))|(valueIMR);
    }
  }

  /*!
    @brief Reset pin to default value
  */ 
  static void _Reset(){
    GPIO_CHx() = (GPIO_CHx() & (~configurationClear));
    base()->ODR = (base()->ODR & (~mask));
    EXTI->EMR = EXTI->EMR & (~mask);
    EXTI->IMR = EXTI->IMR & (~mask);
  }

  /*!
    @brief Configure pin to low power mode
  */
  __attribute__ ((always_inline)) inline 
  static void _ToLowPowerMode(){ _Reset(); }

  /*!
    @brief Set pin output to High level
  */ 
  __attribute__ ((always_inline)) inline 
  static void _High(){ base()->BSRR = mask; }

  /*!
    @brief Set pin output to Low level
  */
  __attribute__ ((always_inline)) inline 
  static void _Low(){ base()->BRR = mask; }

  /*!
    @brief Get current input pin level
  */
  __attribute__ ((always_inline)) inline 
  static bool _Get(){ return static_cast<bool>(base()->IDR & mask); }

  /*!
    @brief Set output level for pin
  */
  __attribute__ ((always_inline)) inline 
  static void _Set(bool state){ base()->BSRR = (0x10000U | state) << pinNumber; }

  /*!
    @brief Toggle current state of pin
  */
  __attribute__ ((always_inline)) inline 
  static void _Toggle(){ base()->BSRR = (0x10000U << pinNumber) | (~ base()->ODR & mask); }

  __attribute__ ((always_inline)) inline 
  static constexpr auto base() { return (GPIO_TypeDef*)gpioBase; }

  __attribute__ ((always_inline)) inline 
  static constexpr auto& GPIO_CHx(){
    return pinNumber > 7 ? ((GPIO_TypeDef*)gpioBase)->CRH : ((GPIO_TypeDef*)gpioBase)->CRL;}

  static constexpr uint32_t configurationValue = static_cast<uint32_t>(config) << 
                                                ((pinNumber > 7 ? (pinNumber - 8) : pinNumber) * 4U);

  static constexpr uint32_t configurationClear = static_cast<uint32_t>(controller::pinConfiguration::ClearMask) << 
                                                ((pinNumber > 7 ? (pinNumber - 8) : pinNumber) * 4U);

  static constexpr uint32_t configurationValueODR = 
         (config == controller::pinConfiguration::Output_High_2MHz ||
          config == controller::pinConfiguration::Output_High_10MHz || 
          config == controller::pinConfiguration::Output_High_50MHz || 
          config == controller::pinConfiguration::Input_PullUp ||
          config == controller::pinConfiguration::Interrupt_Falling || 
          config == controller::pinConfiguration::Event_Falling) ? mask : 0U; 

  static constexpr uint32_t configurationValueEMR = 
         (config == controller::pinConfiguration::Event_Falling ||
          config == controller::pinConfiguration::Event_Rising) ? mask : 0U;

  static constexpr uint32_t configurationValueIMR = 
         (config == controller::pinConfiguration::Interrupt_Falling ||
          config == controller::pinConfiguration::Interrupt_Rising) ? mask : 0U;

  static_assert(pinNumber < 16U, 
    "Pin number should be < 16");

  static_assert(gpioBase == GPIOA_BASE || gpioBase == GPIOB_BASE ||
                gpioBase == GPIOC_BASE || gpioBase == GPIOD_BASE ||
                gpioBase == GPIOE_BASE || gpioBase == GPIOF_BASE ||
                gpioBase == GPIOG_BASE, 
    "gpioBase value is incorrect. Should be one of GPIOA_BASE ... GPIOG_BASE");
};

}//!namespace controller

#endif //!_STN32F1_PIN_HPP