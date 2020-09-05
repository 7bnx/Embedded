//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Interrupt interface
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _INTERRUPT_HPP
#define _INTERRUPT_HPP

#include "type_traits_custom.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{


/*!
  @brief Enable or Disable list of IRQn
*/
class Interrupt{

public:

  /*!
    @brief Enable list of IRQn
    @tparam <pereph...> list of perepheral devices
  */
  template<typename... pereph>
  __attribute__ ((always_inline)) inline
  static void Enable(){
    Set<0, true, typename pereph::interrupts...>();
  }

  /*!
    @brief Enable list of IRQn
    @tparam <IRQn...pereph> list of IRQ numbers
  */
  template<IRQn... pereph>
  __attribute__ ((always_inline)) inline
  static void Enable(){
    Set<0, true, utils::ctv<pereph>...>();
  }


  /*!
    @brief Enable list of IRQn
    @tparam <pereph...> list of perepheral devices
  */
  template<typename... pereph>
  __attribute__ ((always_inline)) inline
  static void Disable(){
    Set<0, false, pereph...>();
  }

  /*!
    @brief Disable list of IRQn
    @tparam <IRQn...pereph> list of IRQ numbers
  */
  template<IRQn... pereph>
  __attribute__ ((always_inline)) inline
  static void Disable(){
    Set<0, false, utils::ctv<pereph>...>();
  }

private:

  template<auto regNumber, typename...>
  struct irqRegisterMask{
    static constexpr auto value = 0U;
  };

  template<auto regNumber, typename Head, typename... Tail>
  struct irqRegisterMask<regNumber, Head, Tail...>{
    static constexpr auto irq = Head::value;
    static constexpr auto value = (((irq >> 5U) == regNumber) ? (uint32_t)(1UL << (((uint32_t)irq) & 0x1FUL)) : 0U) | 
                                  irqRegisterMask<regNumber, Tail...>::value;
  };

  template<auto regNumber, typename... List>
  struct irqRegisterMask<regNumber, utils::Typelist<List...>>{
    static constexpr auto value = irqRegisterMask<regNumber, List...>::value;
  };

  template<auto regNumber, bool state, typename... pereph>
  __attribute__ ((always_inline)) inline
  static void Set(){
    __COMPILER_BARRIER();
    using list = utils::expand_t<pereph...>;
    if constexpr (constexpr uint32_t value = irqRegisterMask<regNumber, list>::value; value){
      if constexpr(state) NVIC->ISER[regNumber] = value;
      else NVIC->ICER[regNumber] = value;
    }
    if constexpr(regNumber + 1 < 16)
      Set<regNumber + 1, state, pereph...>();

    __COMPILER_BARRIER();
  }

};

} // !namespace controller

#endif // !_INTERRUPT_HPP