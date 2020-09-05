//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  List of pins. Common Interface
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _PINLIST_HPP
#define _PINLIST_HPP

#include <cstdint>
#include <cstddef>
#include "interface.hpp"


/*!
  @brief Controller's common interfaces
*/
namespace controller::common{

/*!
  @brief Interface of Pinlist
  @tparam <implementation> hardware implementation of Pinlist
  @tparam <Pins...> single pins or perepheral devices
*/  
template<typename implementation, typename... Pins>
class Pinlist{

public:

  /*!
    @brief Interface
  */
  static constexpr auto interface = controller::interface::parallel;

  /*!
    @brief Initialization of pins in list
  */
  static void Init(){ implementation::_Init(); }

  /*!
    @brief Write value to pins in list
    @param [in] value. Little endian
  */
  static void Write(uint32_t value){ implementation::_Write(value); }

  /*!
    @brief Write value to single pin of the list
    @tparam <pinNumber> number of pin
    @param [in] value to write
  */
  template<size_t pinNumber>
  static void Write(bool value){ implementation::template _WritePin<pinNumber>(value); }

  /*!
    @brief Read value of the Pinlist. Little endian
  */
  static uint32_t Read(){ return implementation::_Read(); }

  /*!
    @brief Set all pins of the list to High-level
  */ 
  static void High(){ implementation::_High(); }

  /*!
    @brief Set all pins of the list to Low-level
  */
  static void Low(){ implementation::_Low();}

  /*!
    @brief Set all pins of the list to low power mode
  */ 
  static void ToLowPowerMode(){ implementation::_ToLowPowerMode(); }

  /*!
    @brief Set all pins of the list to reset state
  */ 
  static void Reset(){ implementation::_Reset(); }

  /*!
    @brief Number of pins in the list
  */ 
  static constexpr size_t size = utils::size_of_list_v<typename implementation::pins>;

  /*!
    @brief Write value to pins in list
  */
  void operator=(uint32_t value){ implementation::_Write(value);}

  /*!
    @brief Read the value of pins in list
  */
  operator uint32_t(){ return implementation::_Read(); }

private:

  friend implementation;

};

} // !namespace controller::common

#endif // !_PINLIST_HPP