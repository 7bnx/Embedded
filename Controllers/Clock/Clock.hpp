//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Clock configuration common interface
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _CLOCK_HPP
#define _CLOCK_HPP

#include <cstdint>
#include "type_traits_custom.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller::common{

/*!
  @brief Set and get system and bus clock. Inherited from controller-specific interface
*/ 
template<typename implementation>
class Clock{
  
public:
  template<uint32_t value = 72000000, bool isExternalSrc = true, uint32_t srcValue = 8000000>
  static bool Set(){ return implementation::template _Set<value, isExternalSrc, srcValue>(); }
  static bool Set(uint32_t value = 72000000, bool isExternalSrc = true, uint32_t srcValue = 8000000){ 
    return implementation::_Set(value, isExternalSrc, srcValue); 
  }
  static uint32_t System(){ return implementation::_System(); }
private:
  Clock() = delete;
  friend implementation;
};

} // !namespace controller

#endif //!_CLOCK_HPP