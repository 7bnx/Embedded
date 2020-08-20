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
#include "pin_configuration.hpp"
#include "type_traits_custom.hpp"

namespace stm32f1::hardware{

/*!
  @brief Pin Driver for STM32F1 series. Don't use it Directly
  @tparam <gpioBase> base address of Port. E.g.: GPIOA_BASE
  @tparam <pinNumber> number of pin. From 0 to 15
  @tparam <config> Configuration for pin
*/ 
template<uint32_t gpioBase, uint8_t pinNumber, controller::pinConfiguration config>
class _PinImplementation: public controller::abstarct::Pin<gpioBase, pinNumber, config>{

public:

  /*!
    @brief Initialization of pin
  */ 
  static void Init(){

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
  static void Reset(){
    GPIO_CHx() = (GPIO_CHx() & (~configurationClear));
    base()->ODR = (base()->ODR & (~mask));
    EXTI->EMR = EXTI->EMR & (~mask);
    EXTI->IMR = EXTI->IMR & (~mask);
  }

  /*!
    @brief Configure pin to low power mode
  */
  __attribute__ ((always_inline)) inline 
  static void ToLowPowerMode(){Reset();}

  /*!
    @brief Set pin output to High level
  */ 
  __attribute__ ((always_inline)) inline 
  static void High(){base()->BSRR = mask;}

  /*!
    @brief Set pin output to Low level
  */
  __attribute__ ((always_inline)) inline 
  static void Low(){base()->BRR = mask;}

  /*!
    @brief Return current pin level
  */
  __attribute__ ((always_inline)) inline 
  static bool Get(){return static_cast<bool>(base()->IDR & mask);}

  /*!
    @brief Set output level for pin
  */
  __attribute__ ((always_inline)) inline 
  static void Set(bool state){base()->BSRR = (0x10000U | state) << pinNumber;}

  /*!
    @brief Toggle current state of pin
  */
  __attribute__ ((always_inline)) inline 
  static void Toggle(){base()->BSRR = (0x10000U << pinNumber) | (~ base()->ODR & mask);};

  /*!
    @brief Pin's number mask. If pin number = 2, then mask = 1 << 2;
  */
  static constexpr uint32_t mask = controller::abstarct::Pin<gpioBase, pinNumber, config>::mask;

private:

  __attribute__ ((always_inline)) inline 
  static constexpr auto base() {return (GPIO_TypeDef*)gpioBase;};

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

}//!namespace stm32f1::hardware

#endif //!_STN32F1_PIN_HPP