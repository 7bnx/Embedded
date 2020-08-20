//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Handle list of pins
//  TODO:         
//----------------------------------------------------------------------------------
#ifndef _PINLIST_HPP
#define _PINLIST_HPP

#include <cstdint>
#include "controller_define.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief Operate with list of pin
  @tparam <typename... Pins> list of single pins ot/and Pinlist
*/  
template<typename... Pins>
class Pinlist: public hardware::_PinlistImplementation<Pins...>{
public:

  /*!
    @brief Write value to pins in list
  */
  void operator=(uint32_t value){hardware::_PinlistImplementation<Pins...>::Write(value);}

  /*!
    @brief Read the state of pins in list
  */
  operator uint32_t(){ return hardware::_PinlistImplementation<Pins...>::Read(); }
  
private:
  
};

} // !namespace controller

#endif //! _PINLIST_HPP