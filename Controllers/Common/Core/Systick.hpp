//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Systick
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _SYSTICK_HPP
#define _SYSTICK_HPP

#include <cstdint>
#include "../Compiler/GCC.h"
#include "Registers.hpp"
#include "Interrupt.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller
{

namespace configuration::systick{
 
 enum class source{
  External = 0,
  Internal = 4
 };

 enum class isr{
  ISR_Disable = 1,
  ISR_Enable = 2
 };

}

/*!
  @brief Systick class
*/
class Systick: private hardware::Registers{
public:

  /*!
    @brief Enable Systick
  */
  __FORCE_INLINE static void Enable(bool isEnable = true){
    Registers::_Set<address::CTRL, mask::CTRL::ENABLE>(isEnable ? mask::CTRL::ENABLE : 0);
  }

  /*!
    @brief Enable Systick interrupt
  */
  __FORCE_INLINE static void EnableInterrupt(bool isEnable){ 
    Registers::_Set<address::CTRL, mask::CTRL::TICKINT>(isEnable ? mask::CTRL::TICKINT : 0);
  }

  /*!
    @brief Reset Systick counter
  */
  __FORCE_INLINE static void Reset(){
    Registers::_Clear<address::CTRL, mask::CTRL::ENABLE>();
    Registers::_Set<address::CTRL, mask::CTRL::ENABLE>();
  }

  /*!
    @brief Get current counter value of Systick
  */
  __FORCE_INLINE static uint32_t GetValue(){ return Registers::_Read<address::VAL>(); }


  /*!
    @brief Initialization of Systick
    @tparam [in] usTimeOverload time to overload timer in us
  */
  template<uint32_t usTimeOverload,
           configuration::systick::isr isr = configuration::systick::isr::ISR_Enable,
           configuration::systick::source source = configuration::systick::source::Internal>
  static bool Init(){
    uint32_t freqSystem = Clock::Get();
    valueLOAD = (freqSystem/1000000UL)*usTimeOverload - 1U;
    if (valueLOAD > value::maxLOAD) return false;
    Registers::_Clear<address::CTRL, mask::CTRL::ENABLE>();
    Registers::_Write<address::LOAD>(valueLOAD);
    Registers::_Write<address::VAL, 0>();
    Registers::_Write<address::CTRL, mask::CTRL::ENABLE | (uint32_t)isr | (uint32_t)source>();
    valueNanoSecInTick = 1000000UL/(freqSystem/1000UL);
    return true;
  }

  /*!
    @brief Check if Systick is enabled
  */
  __FORCE_INLINE bool IsEnable(){ return Registers::_Read<address::CTRL, mask::CTRL::ENABLE>(); }

  /*!
    @brief Check if Systick is elapsed
  */
  __FORCE_INLINE bool IsElapsed(){ return Registers::_Read<address::CTRL, mask::CTRL::COUNTFLAG>(); }

  /*!
    @brief Delay
    @param [in] value of delay in ns
  */
  static void Delay_ns(uint32_t value){
    uint32_t ticksCurrent = Registers::_Read<address::VAL, mask::LOAD>();
    uint32_t ticksValue = value / valueNanoSecInTick + 1;
    volatile uint32_t ticksToCompare = 0;
    while(ticksValue > ticksToCompare){
      uint32_t ticksNewValue = Registers::_Read<address::VAL, mask::LOAD>();
      ticksToCompare += ticksCurrent > ticksNewValue ? 
                        ticksCurrent - ticksNewValue : 
                        valueLOAD + ticksCurrent - ticksNewValue;
      ticksCurrent = ticksNewValue;
    }
  }

  /*!
    @brief Delay
    @param [in] value of delay in us
  */
  __FORCE_INLINE static void Delay_us(uint32_t value){ Delay_ns(1000*value); }

  /*!
    @brief Delay
    @param [in] value of delay in ms
  */
  __FORCE_INLINE static void Delay_ms(uint32_t value){ Delay_ns(1000000*value); }

  /*!
    @brief Interrupt Handler
  */
  __FORCE_INLINE static void ISR(){
    (uint32_t)Registers::_Read<address::CTRL, mask::CTRL::COUNTFLAG>();
    if (CallbackElapsed) CallbackElapsed();
  }

  /*!
    @brief Executes when the timer is elapsed
  */
  static inline void (*CallbackElapsed)() = nullptr;

private:

  friend Interrupt;

  static inline uint32_t 
    valueLOAD = 0,
    valueNanoSecInTick = 0;

  struct address{
    static constexpr uint32_t
      base = 0xE000E010,
      CTRL = base,
      LOAD = base + 4,
      VAL = base + 8,
      CALIB = base + 12;
  };

  struct mask{
    static constexpr uint32_t
      LOAD = 0xFFFFFF;
    struct CTRL{
      static constexpr uint32_t
        ENABLE = 1,
        TICKINT = 2,
        COUNTFLAG = 0x10000;
    };
  };

  struct value{
    static constexpr uint32_t
      maxLOAD = 0xFFFFFF;
  };

  struct initialization{
    using power = trait::Valuelist<>;
    using pins = trait::Typelist<>;
    using interrupts = trait::Valuelist<>;
  };

};

} // !namespace controller


#endif // !_SYSTICK_HPP