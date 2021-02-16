//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  List of pins. Common Interface
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _IPINLIST_HPP
#define _IPINLIST_HPP

#include "../Common/Core/Interface.hpp"
#include "../Common/Compiler/Compiler.h"
#include "../Pinlist/Pinlist_Helper.hpp"

/*!
  @brief Controller's peripherals interfaces
*/
namespace controller::interfaces{

/*!
  @brief Interface of Pinlist
  @tparam <adapter> hardware implementation of Pinlist
  @tparam <Pins...> single pins or perepheral devices
*/  
template<typename adapter, typename... Pins>
class IPinlist : public helper::pinlist::Helper<adapter, Pins...>{

using Helper = helper::pinlist::Helper<adapter, Pins...>;

public:

  /*!
    @brief Number of pins in the list
  */ 
  static constexpr auto size = Helper::size;
  
  /*!
    @brief Interface of connection
  */ 
  static constexpr auto interface = controller::interface::PARALLEL;

  /*!
    @brief Initialization of pins in list
  */
  __FORCE_INLINE static void Init(){
    Helper::_Init();
  }

  /*!
    @brief Set all pins of the list to reset state
  */ 
  __FORCE_INLINE static void DeInit(){
    Helper::_DeInit(); 
  }

  /*!
    @brief Set all pins of the list to low power mode
  */ 
  __FORCE_INLINE static void LowPower(){ 
   Helper::_LowPower();
  }

  /*!
    @brief Write value to pins in list
    @param [in] value. Little endian
  */
  __FORCE_INLINE static void Write(uint32_t value){ Helper::_WriteRunTime(value);}

  /*!
    @brief Write value to pins in list
    @param [in] value. Little endian
  */
  template<uint32_t value>
  __FORCE_INLINE static void Write(){ 
      Helper::template _WriteCompileTime<value>();
    }

  /*!
    @brief Set all pins of the list to High-level
  */ 
  static void High(){ 
    Helper::template _WriteCompileTime<~0>();
  }

  /*!
    @brief Set all pins of the list to Low-level
  */
  static void Low(){
    Helper::template _WriteCompileTime<0>(); 
  }

  /*!
    @brief Write state to pin in the list
    @tparam <number> number of pin
    @param [in] state to write
  */
  template<size_t number>
  static void SetPin(bool state){
    Helper::template get_pin_t<number>::Set(state);
  }

  /*!
    @brief Read value of the Pinlist. Little endian
  */
  static uint32_t Read(){ return Helper::_Read();}

  /*!
    @brief Read state of pin in the list
    @tparam <number> number of pin
  */
  template<size_t number>
  static bool ReadPin(){
    return typename Helper::template get_pin_t<number>::Get();
  }

  /*!
    @brief Get pin from list by number
    @tparam <number> number of pin
  */
  template<size_t number>
  using get_pin = typename Helper::template get_pin_t<number>;

  /*!
    @brief Get pinlist without poped pins from front
    @tparam <number> number to pop
  */
  template<size_t number>
  using pop_front_pins = typename Helper::template pop_front_pins_t<number>;

  /*!
    @brief Get pinlist without poped pins from back
    @tparam <number> number to pop
  */
  template<size_t number>
  using pop_back_pins = typename Helper::template pop_back_pins_t<number>;

  /*!
    @brief Set mode to all pins in list
  */
  struct mode{

    /*!
      @brief Set config of source to all pins in list
      @tparam <sourcePin> apply configuration from sourcePin to pins in list
    */
    template<typename sourcePin>
    using same = typename Helper::set<sourcePin::mode::get>;

    /*!
      @brief Set config to all pins in list
      @tparam <config> apply config to pins in list
    */
    template<decltype(get_pin<0>::mode::get) config>
    using set = typename Helper::set<config>;
  };

  /*!
    @brief Generate custom Pinlist
  */
  struct generate{
    /*!
      @brief Complement all ports, used in Pinlist with rest pins
      @tparam <config> configuration for rest pins
    */
    template<decltype(get_pin<0>::mode::get) config>
    using complement = typename Helper::generate::set<config>;

    /*!
      @brief Generate row of pins from pinlist[0] - with 'number' and configuration till numberPinEnd.
      E.g.: Pinlist<PA_1, PB_13>::generate::row<15> = Pinlist<PA_1, PB_13, PB_15, PB_15>;
            Pinlist<PA_1, PB_13>::generate::row<11> = Pinlist<PA_1, PB_13, PB_12, PB_11>; 
      @tparam <numberPinEnd> the number of end pin in row
    */
    template<std::size_t numberPinEnd>
    using row = typename Helper::generate::row<numberPinEnd>;
  };

}; 

} // !namespace controller::common

#endif // !_IPINLIST_HPP