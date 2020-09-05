//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Power enabling for controllers(Clock)
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _POWER_HPP
#define _POWER_HPP

#include <cstdint>
#include "type_traits_custom.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller::common{

/*!
  @brief Enable or Disable peripheral Power. Inherited from controller-specific interface
*/
template<typename implementation>  
class Power{

public:

  /*!
    @brief Enable or Disable peripheral Power
    @tparam <periphBase> comma-separated list of base addresses. E.g.: <GPIOA_BASE, SPI1_BASE>
  */
  template<uint32_t... periphBase>
  static void Set(bool state){ implementation:: template _Set<periphBase...>(state); }

  /*!
    @brief Enable or Disable peripheral Power
    @tparam <List> comma-separated list of peripheral devices. E.g.: <spi1, pin>
  */
  template<typename... List>
  static void Set(bool state){ implementation:: template _Set<List...>(state); }

  /*!
    @brief Enable peripheral Power
    @tparam <periphBase> comma-separated list of base addresses. E.g.: <GPIOA_BASE, SPI1_BASE>
  */
  template<uint32_t... periphBase>
  static void Enable(){ implementation:: template _Set<periphBase...>(true); }

  /*!
    @brief Enable peripheral Power
    @tparam <List> comma-separated list of peripheral devices. E.g.: <spi1, pin>
  */
  template<typename... List>
  static void Enable(){ implementation:: template _Set<List...>(true); }

  /*!
    @brief Disable peripheral Power
    @tparam <periphBase> comma-separated list of base addresses. E.g.: <GPIOA_BASE, SPI1_BASE>
  */
  template<uint32_t... periphBase>
  static void Disable(){ implementation:: template _Set<periphBase...>(false); }

  /*!
    @brief Disable peripheral Power
    @tparam <List> comma-separated list of peripheral devices. E.g.: <spi1, pin>
  */
  template<typename... List>
  static void Disable(){ implementation:: template _Set<List...>(false); }

private:

  Power() = delete;
  friend implementation;
};

} // !namespace controller

#endif // !_POWER_HPP