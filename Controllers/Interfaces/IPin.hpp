//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Pin common interface
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _IPIN_HPP
#define _IPIN_HPP

#include "../Common/Core/Registers.hpp"
#include "../Common/Core/Interface.hpp"
#include "../Interfaces/IPower.hpp"
#include "../Interfaces/IPinlist.hpp"
#include "../Pin/Ports.hpp"

/*!
  @brief Controller's peripherals interfaces
*/
namespace controller::interfaces{

/*!
  @brief Interface for Pin. Static class. CRT pattern
  @tparam <adapter> class of specific controller
*/
template<typename adapter>
class IPin{
public:

  /*!
    @brief Interface
  */
  static constexpr auto interface = controller::interface::SERIAL;

  /*!
    @brief Initialization of pin
  */
  static void Init(){ adapter::_Init(); }

  /*!
    @brief Reset pin to default configuration
  */ 
  static void DeInit(){ adapter::_DeInit(); }

  /*!
    @brief Configure pin to low power mode
  */
  static void LowPower(){ adapter::_LowPower(); }

  /*!
    @brief Set pin output to High level
  */ 
  static void High(){ adapter::_High(); }

  /*!
    @brief Set pin output to Low level
  */
  static void Low(){ adapter::_Low(); }

  /*!
    @brief Set output level for pin
  */
  static void Set(bool state){ adapter::_Set(state); }

  /*!
    @brief Toggle current state of pin
  */
  static void Toggle(){ adapter::_Toggle(); }

  /*!
    @brief Get current input pin level
  */
  static bool Get(){ return adapter::_Get(); }

  /*!
    @brief Set, change or get mode of pin
  */
  class mode{
    private:
    static constexpr auto remapDefault = adapter::remap;

    public:
    
    /*!
    @brief Get configuration of pin
    */
    static constexpr auto get = adapter::configuration;

    /*!
    @brief Set configuration of pin
    @tparam <config> configuration
    @tparam <remap> *optional, remap value for alternative functions
    */
    template<decltype(get) config, decltype(remapDefault) remap = remapDefault>
    using set = typename adapter::setMode<config, remap>;

    /*!
    @brief Get configuration from source pin and set it to destination pin
    @tparam <sourcePin> source configuration
    */
    template<typename sourcePin>
    using same = typename adapter::setMode<sourcePin::configuration, sourcePin::remap>;

    /*!
    @brief Reconfigre all parameters of pin
    @tparam <portID> identification of port. Starts from 0. E.g.: PORTA = 0, PORTC = 3...
    @tparam <pinNumber> number of pin
    @tparam <config> configuration
    @tparam <remap> *optional, remap value for alternative functions
    */
    template<uint32_t portID, uint8_t pinNumber, decltype(get) config, decltype(remapDefault) remap = remapDefault>
    using reconfigure = typename adapter::newPin<portID, pinNumber, config, remap>;

  };

};

} //!namespace controller

#endif //!_IPIN_HPP