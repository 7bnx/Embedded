//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Systick library
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _SYSTICK_HPP
#define _SYSTICK_HPP

#include <cstdint>
#include "controller_periphery.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller
{

/*!
  @brief Systick class
*/
class Systick{
public:

  /*!
    @brief Enable Systick
  */
  __attribute__ ((always_inline)) inline
  static void Enable(bool isEnable){
	  if (isEnable)SysTick->CTRL |= SysTick_CTRL_ENABLE;
	  else SysTick->CTRL &=~ SysTick_CTRL_ENABLE;
  }

  /*!
    @brief Enable Systick interrupt
  */
  __attribute__ ((always_inline)) inline
  static void EnableInterrupt(bool isEnable){
    if (isEnable) NVIC_EnableIRQ(SysTick_IRQn);
	  else NVIC_DisableIRQ(SysTick_IRQn);
  }

  /*!
    @brief Reset Systick counter
  */
  __attribute__ ((always_inline)) inline
  static void Reset(){
	  SysTick->CTRL &=~ SysTick_CTRL_ENABLE;
	  SysTick->CTRL |= SysTick_CTRL_ENABLE;
  }

  /*!
    @brief Get current counter value of Systick
  */
  __attribute__ ((always_inline)) inline
  static uint32_t Get(){return SysTick->VAL;}


  /*!
    @brief Initialization of Systick
    @param [in] usTimeOverload time to overload timer in us
  */
  static bool Init(uint32_t usTimeOverload){
    SysTick->CTRL &=~ SysTick_CTRL_ENABLE;
    freqSystem = Clock::System();
    uint32_t count = (freqSystem/1000000UL)*usTimeOverload - 1U;
    if (count > SysTick_LOAD_RELOAD) return false;
    usTimeOverload = usTimeOverload;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE | SysTick_CTRL_ENABLE;
    SysTick->LOAD = count;
    SysTick->VAL = 0;
    nanoSecInTick = 1000000UL/(freqSystem/1000UL);
    return true;
  }

  /*!
    @brief Check if Systick is enabled
  */
  __attribute__ ((always_inline)) inline
  bool IsEnable(){ return SysTick->CTRL & SysTick_CTRL_ENABLE; }

  /*!
    @brief Check if Systick is elapsed
  */
  __attribute__ ((always_inline)) inline
  bool IsElapsed(){ return SysTick->CTRL & SysTick_CTRL_COUNTFLAG; }

  /*!
    @brief Delay
    @param [in] value of delay in ns
  */
  static void Delay_ns(uint32_t value){
    volatile uint32_t ticksCurrent = SysTick->VAL;
    volatile uint32_t ticksValue = value / nanoSecInTick + 1U;
    volatile uint32_t ticksToCompare = 0U;
    while(ticksValue > ticksToCompare){
      volatile uint32_t ticksNewValue = SysTick->VAL;
      ticksToCompare += ticksCurrent > ticksNewValue ? 
                        ticksCurrent - ticksNewValue : 
                        SysTick->LOAD + ticksCurrent - ticksNewValue;
      ticksCurrent = ticksNewValue;
    }
  }

  /*!
    @brief Delay
    @param [in] value of delay in us
  */
  __attribute__ ((always_inline)) inline
  static void Delay_us(uint32_t value){ Delay_ns(1000*value); }

  /*!
    @brief Delay
    @param [in] value of delay in ms
  */
  __attribute__ ((always_inline)) inline
  static void Delay_ms(uint32_t value){ Delay_ns(1000000*value); }

private:

  static inline uint32_t freqSystem = 0;
  static inline uint32_t nanoSecInTick = 0;

};

} // !namespace controller


#endif // !_SYSTICK_HPP