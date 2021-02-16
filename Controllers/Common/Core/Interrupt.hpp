//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Interrupt interface
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _INTERRUPT_HPP
#define _INTERRUPT_HPP

#include <cstdint>
#include "../Compiler/Compiler.h"
#include "Registers.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{


/*!
  @brief Enable or Disable list interrupts
*/
class Interrupt : private hardware::Registers{

public:

  /*!
    @brief Enable interrupts
    @tparam <peripherals...> list of peripherals
  */
  template<typename... peripherals>
  __FORCE_INLINE static void Enable(){
    Set<addressISER, peripherals...>();
  }

  /*!
    @brief Disable interrupts
    @tparam <peripherals...> list of peripherals
  */
  template<typename... peripherals>
  __FORCE_INLINE static void Disable(){
    Set<addressICER, peripherals...>();
  }

private:

  template<uint32_t addressBase, typename... peripherals>
  __FORCE_INLINE static void Set(){
    using namespace trait;

    using listISR = lists_expand_t<typename peripherals::initialization::interrupts...>;
    
    if constexpr(size_of_list_v<listISR>){
      using listRegisters = make_unique_t<valuelist_shift_right_t<valueShift, listISR>>;

      using listValues = typename valuelistISR<listRegisters, listISR>::type;
      using listAddresses = valuelist_add_t<addressBase, valuelist_mul_t<4, listRegisters>>;

      __COMPILER_BARRIER();
      Registers::_Set<listAddresses, listValues>();
      __COMPILER_BARRIER();
    }
  }

    // 32 = 2^5 interrupts per control register (enable/disable)
  static constexpr uint32_t valueShift = 5;

  static constexpr uint32_t addressBase = 0xE000E100;
  static constexpr uint32_t addressISER = addressBase;
  static constexpr uint32_t addressICER = addressBase + 0x80;

  template<typename listRegisters, typename listISR, typename Result = trait::Valuelist<>>
  struct valuelistISR{
    static constexpr uint32_t numberRegister = trait::front_v<listRegisters>;
    static constexpr uint32_t isr = trait::front_v<listISR>;
    static constexpr uint32_t numberRegisterFront = isr >> valueShift;
    static constexpr uint32_t mask = (numberRegisterFront == numberRegister) ? 1 << (isr & 31) : 0;
    using restListRegisters = trait::pop_front_t<listRegisters>;
    using restListISR = trait::pop_front_t<listISR>;
    static constexpr uint32_t value = mask | valuelistISR<listRegisters, restListISR, Result>::value;
    using result = trait::push_back_value_t<Result, value>;
    using type = typename valuelistISR<restListRegisters, listISR, result>::type;
  };

  template<typename listRegisters, typename Result>
  struct valuelistISR<listRegisters, trait::Valuelist<>, Result>{
    static constexpr uint32_t value = 0;
    using type = Result;
  };

  template<typename listISR, typename Result>
  struct valuelistISR<trait::Valuelist<>, listISR, Result>{
    static constexpr uint32_t value = 0;
    using type = Result;
  };

};

} // !namespace controller

#endif // !_INTERRUPT_HPP