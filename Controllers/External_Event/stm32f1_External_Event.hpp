//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  External signals events on pins
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _STM32F1_EXTERNAL_EVENT_HPP
#define _STM32F1_EXTERNAL_EVENT_HPP

#include <cstdint>
#include "../Common/Compiler/Compiler.h"
#include "../Common/Core/Interrupt.hpp"
#include "../Pinlist/Pinlist_Helper.hpp"
#include "../Power/stm32f1_Power.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief External Event
  @tparam <Pin> with external event or interrupt
*/
template<typename Pin>
class ExternalEvent: protected hardware::Registers{

public:

  /*!
    @brief Interrupt Handler
  */
  __FORCE_INLINE static void ISR(){
    if (IsPending()){
      ClearPending();
      if (CallbackEvent) CallbackEvent();
    }
  }

  /*!
    @brief Force interrupt by software
  */
  __FORCE_INLINE static void SoftwareInterrupt(){ Registers::_Set<Pin::address::EXTI_SWIER, Pin::mask::pin>(); }

  /*!
    @brief Clear pending bit
  */
  __FORCE_INLINE static void ClearPending(){ Registers::_Set<Pin::address::EXTI_PR, Pin::mask::pin>(); }

  /*!
    @brief Check if event is pending 
  */
  __FORCE_INLINE static bool IsPending(){ return Registers::_Read<Pin::address::EXTI_PR, Pin::mask::pin>(); }

  /*!
    @brief Initialization of pin
  */
  __FORCE_INLINE static void Init(){ Pin::Init(); }

  /*!
    @brief Reset pin to default configuration
  */ 
  static void DeInit(){ Pin::DeInit(); }

  /*!
    @brief Configure pin to low power mode
  */
  static void LowPower(){ Pin::LowPower(); }

  /*!
    @brief Get current input pin level
  */
  static bool Get(){ return Pin::Get(); }

  /*!
    @brief Callback Event
  */
  static inline void (*CallbackEvent)() = nullptr; 

private:

  template<typename>
  friend class controller::interfaces::IPower;

  template<typename, typename...>
  friend class helper::pinlist::Helper;

  friend controller::Interrupt;

  using initialization = typename Pin::initialization;

};

} // !namespace controller

#endif // !_STM32F1_EXTERNAL_EVENT_HPP