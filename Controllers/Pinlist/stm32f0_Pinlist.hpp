//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Adapter for Pinlist. STM32F0-series
//  TODO:          
//----------------------------------------------------------------------------------

#ifndef _STM32F0_PINLIST_HPP
#define _STM32F0_PINLIST_HPP

#include "../Common/Compiler/Compiler.h"
#include "../Common/Core/Registers.hpp"
#include "../Interfaces/IPinlist.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief Operations for list of pins
  @tparam <typename... Pins> list of single pin, pins or/and Pinlists.
*/ 
template<typename... Pins>
class Pinlist: public interfaces::IPinlist<Pinlist<Pins...>, Pins...>, public hardware::Registers{

  using Registers = controller::hardware::Registers;

  Pinlist() = delete;

  static constexpr size_t pinsInPort = 16;
 
  template<typename, typename...>
  friend class helper::pinlist::Helper;

  template<typename addresses, typename values, typename masks>
  __FORCE_INLINE static void _ConfigureWrite(){
    Registers::_Write<addresses, values, masks>();
  }

  template<typename addresses, typename values, typename masks>
  __FORCE_INLINE static void _ConfigureSet(){
    Registers::_Set<addresses, values, masks>();
  }

  template<uint32_t address, uint32_t mask, auto countPins>
  __FORCE_INLINE static uint32_t _Read(){
    return Registers::_Read<address, mask>();
  }

  template<uint32_t address, uint32_t value, uint32_t mask, auto countPins>
  __FORCE_INLINE static void _Write(){
    Registers::_Write<address, (mask << pinsInPort) | (value & mask)>();
  }
  
  template<uint32_t address, uint32_t mask, auto countPins>
  __FORCE_INLINE static void _Write(uint32_t value){
    Registers::_Write<address>((mask << pinsInPort) | (value & mask));
  }

  template<typename... newPins>
  using newList = Pinlist<newPins...>;

};
 
}// !namespace controller

#endif // !_STM32F0_PINLIST_HPP