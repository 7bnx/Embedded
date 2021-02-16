//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Hardware-dependent library for Pin driver of stm32f0
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _STM32F0_PIN_HPP
#define _STM32F0_PIN_HPP

#include "../Common/Compiler/Compiler.h"
#include "stm32f0_Pin_Helper.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

namespace wrapper{

struct Pins{

/*!
  @brief Pin Driver for STM32F0 series. Don't use it Directly
  @tparam <portID> number of port. E.g.: GPIOA = 0
  @tparam <number> number of pin. From 0 to 15
  @tparam <config> pin configuration
  @tparam <remapValue> value to remap pin fo alternative function
*/
template<uint32_t portID, uint32_t number, configuration::pin config = configuration::pin::Default, uint32_t remapValue = 0>
class Pin: public interfaces::IPin<Pin<portID, number, config, remapValue>>, protected controller::hardware::Registers,
           public helper::pin::Helper<Pin<portID, number, config, remapValue>, portID, number, config, remapValue>{

  using Helper = helper::pin::Helper<Pin<portID, number, config>, portID, number, config, remapValue>;

  template<typename, typename...>
  friend class interfaces::IPin;

  template<configuration::pin configuration, uint32_t remap = remapValue>
  using setMode= Pin<portID, number, configuration, remap>;

  template<uint32_t port, uint8_t pin, configuration::pin conf = configuration::pin::Default, uint32_t remap = remapValue>
  using newPin = Pin<port, pin, conf, remap>;

  __FORCE_INLINE static void _Init(){
    using addresses = typename Helper::_configuration<>::addresses;
    using values = typename Helper::_configuration<>::values;
    using masks = typename Helper::_configuration<>::masks;
    Registers::_Set<addresses, values, masks>();
  }

  __FORCE_INLINE static void _DeInit(){
    Pin<portID, number, configuration::pin::Default>::Init();
  }

  __FORCE_INLINE static void _LowPower(){
    Pin<portID, number, configuration::pin::Low_Power>::Init();
  }

  __FORCE_INLINE static void _High(){
    Registers::_Write<Helper::address::BSRR, Helper::mask::pin, Helper::mask::pin>(); 
  }

  __FORCE_INLINE static void _Low(){ 
    Registers::_Write<Helper::address::BRR, Helper::mask::pin, Helper::mask::pin>(); 
  }

  __FORCE_INLINE static bool _Get(){ 
      return static_cast<bool>(Registers::_Read<Helper::address::IDR, Helper::mask::pin>());
    }

  __FORCE_INLINE static void _Set(bool state){
      auto value = (0x10000U | static_cast<uint32_t>(state)) << number;
      Registers::_Write<Helper::address::BSRR, (Helper::mask::pin << 16) | Helper::mask::pin>(value); 
    }

  __FORCE_INLINE static void _Toggle(){
    auto value = (Helper::mask::pin << 16) | ~Registers::_Read<Helper::address::ODR, Helper::mask::pin>();
    Registers::_Write<Helper::address::BSRR, Helper::mask::pin | (Helper::mask::pin << 16)>(value); 
  }
  
};

};

} // !namespace wrapper

/*!
  @brief Pin Driver for STM32F1 series
*/
struct Pin : 
public constructor::Ports::PortA_16<wrapper::Pins>,
public constructor::Ports::PortB_16<wrapper::Pins>,
public constructor::Ports::PortC_16<wrapper::Pins>,
public constructor::Ports::PortD_16<wrapper::Pins>,
public constructor::Ports::PortE_16<wrapper::Pins>,
public constructor::Ports::PortF_16<wrapper::Pins>{};

} //!namespace controller

#endif //!_STM32F0_PIN_HPP