//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Hardware-dependent driver for Power Enabling(Clock). STM32F1-series
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _STM32F1_POWER_HPP
#define _STM32F1_POWER_HPP

#include "../Common/Compiler/Compiler.h"
#include "../Common/Core/Registers.hpp"
#include "../Interfaces/IPower.hpp"

/*!
  @brief Controller's peripherals
*/
namespace controller{

/*!
  @brief Power managment for controller
*/
class Power: public interfaces::IPower<Power>, public hardware::Registers{

  Power() = delete;

public:

  /*!
    @brief Creates custom 'power' list from values. Peripheral driver should implement 'power' trait.
      E.g.: using power = Power::fromValues<1, 512, 8>::power; 
    @tparam <valueAHB=0> value for AHBENR register
    @tparam <valueAPB1=0> value for APB1ENR register
    @tparam <valueAPB2=0> value for APB2ENR register
  */
  template<uint32_t valueAHBENR = 0, uint32_t valueAPB1ENR = 0, uint32_t valueAPB2ENR = 0>
  struct fromValues{
    fromValues() = delete;
    struct initialization{
      initialization() = delete;
      using power = trait::Valuelist<valueAHBENR, valueAPB1ENR, valueAPB2ENR>;
    };
  };

private: 

  struct address{
    static constexpr uint32_t
      AHBENR  = 0x40021014,
      APB2ENR = 0x40021018,
      APB1ENR = 0x4002101C;
  };
  
  using AddressesList = trait::Valuelist<address::AHBENR, address::APB1ENR, address::APB2ENR>;

  using defaultEnable = fromValues<0x14U, 0U, 0U>::initialization::power;
  using defaultDisable = fromValues<0x557, 0x3AFEC9FD, 0x38FFFD>::initialization::power;

  template<typename EnableList, typename DisableList>
  __FORCE_INLINE static void _Set(){
    Registers::_Set<AddressesList, EnableList, DisableList>();
  }

  template<typename EnableList>
  __FORCE_INLINE static void _Write(){
    Registers::_Write<AddressesList, EnableList>();
  }

  friend class IPower<Power>;

};

} // !namespace controller

#endif // !_STM32F1_POWER_HPP