//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Common interface for Power Enabling (Clock)
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _IPOWER_HPP
#define _IPOWER_HPP

#include "../Common/Compiler/Compiler.h"
#include "../../Utils/type_traits_custom.hpp"

/*!
  @brief Controller's peripherals interfaces
*/
namespace controller::interfaces{

/*!
  @brief Interface for Power(Clock control). Static class. CRT pattern
  @tparam <adapter> class of specific controller
*/
template<typename adapter>  
class IPower{

  IPower() = delete;

public:

  /*!
    @brief Enables peripherals Power(Clock)
    @tparam <Peripherals> list of peripherals with trait 'power'
  */
  template<typename... Peripherals>
  __FORCE_INLINE static void Enable(){
    using tEnableList = trait::lists_termwise_or_t<typename Peripherals::initialization::power...>;
    using tDisableList = typename adapter::fromValues<>::initialization::power;
    adapter:: template _Set<tEnableList, tDisableList>();
  }

  /*!
    @brief Enables peripherals Power(Clock). Makes 'reg = value', not 'reg |= value', 
    @tparam <Peripherals> list of peripherals with trait 'power'
  */
  template<typename... Peripherals>
  __FORCE_INLINE static void Enable_Init(){
    using tORedList = trait::lists_termwise_or_t<typename Peripherals::initialization::power...>;
    using tEnableList = trait::operation_non_zero_then_or_t<tORedList, typename adapter::defaultEnable>;
    adapter:: template _Write<tEnableList>();
  }

  /*!
    @brief Enables Power(Clock) except listed peripherals in 'ExceptList'. 
      If Enable = Exception = 1, then Enable = 0, otherwise depends on Enable.
    @tparam <EnableList> list to enable, with trait 'power'
    @tparam <ExceptList> list of exception, with trait 'power'
  */
  template<typename EnableList, typename ExceptList>
  __FORCE_INLINE static void EnableExcept(){
    using tXORedList = trait::lists_termwise_xor_t<typename EnableList::initialization::power, typename ExceptList::initialization::power>;
    using tEnableList = trait::lists_termwise_and_t<typename EnableList::initialization::power, tXORedList>;
    using tDisableList = typename adapter::fromValues<>::initialization::power;
    adapter:: template _Set<tEnableList, tDisableList>();
  }

  /*!
    @brief Disables peripherals Power(Clock)
    @tparam <Peripherals> list of peripherals with trait 'power'
  */
  template<typename... Peripherals>
  __FORCE_INLINE static void Disable(){
    using tDisableList = trait::lists_termwise_or_t<typename Peripherals::initialization::power...>;
    using tEnableList = typename adapter::fromValues<>::initialization::power;
    adapter:: template _Set<tEnableList, tDisableList>();
  }

  /*!
    @brief Disables All peripherals Power(Clock)
  */
  __FORCE_INLINE static void DisableAll(){
    using tEnableList = typename adapter::defaultEnable;
    using tDisableList = trait::lists_termwise_or_t<typename adapter::defaultEnable, typename adapter::defaultDisable>;
    adapter:: template _Write<tEnableList, tDisableList>();
  }

  /*!
    @brief Disables Power(Clock) except listed peripherals in 'ExceptList'. 
      If Disable = Exception = 1, then Disable = 0, otherwise depends on Disable.
    @tparam <DisableList> list to disable, with trait 'power'
    @tparam <ExceptList> list of exception, with trait 'power'
  */
  template<typename DisableList, typename ExceptList>
  __FORCE_INLINE static void DisableExcept(){
    using tXORedList = trait::lists_termwise_xor_t<typename DisableList::initialization::power, typename ExceptList::initialization::power>;
    using tDisableList = trait::lists_termwise_and_t<typename DisableList::power, tXORedList>;
    using tEnableList = typename adapter::fromValues<>::initialization::power;
    adapter:: template _Set<tEnableList, tDisableList>();
  }

  /*!
    @brief Disable and Enables Power(Clock) depends on values. 
      If Enable = Disable = 1, then Enable = Disable = 0, otherwise depends on values
    @tparam <EnableList> list to enable, with trait 'power'
    @tparam <DisableList> list to disable, with trait 'power'
  */
  template<typename EnableList, typename DisableList>
  __FORCE_INLINE static void Keep(){
    using tXORedList = trait::lists_termwise_xor_t<typename EnableList::initialization::power, typename DisableList::initialization::power>;
    using tANDList = trait::lists_termwise_and_t<typename EnableList::initialization::power, tXORedList>;
    using tEnableList = trait::operation_non_zero_then_or_t<tANDList, typename adapter::defaultEnable>;
    adapter:: template _Write<tEnableList>();
  }

  /*!
    @brief Creates custom 'power' list from peripherals. Peripheral driver should implement 'power' trait.
      E.g.: using power = Power::makeFromValues<1, 512, 8>::power; 
    @tparam <PeripheralsList> list of peripherals with trait 'power'
  */
 template<typename... PeripheralsList>
  struct fromPeripherals{
    fromPeripherals() = delete;
    using power = trait::lists_termwise_or_t<typename PeripheralsList::initialization::power...>;
    friend class IPower<adapter>;
  };

};

} // !namespace controller::interfaces

#endif // !_IPOWER_HPP