//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Common interface for Real Time Clock
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _IRTC_HPP
#define _IRTC_HPP

#include "../Utils/TimeDate.hpp"

/*!
  @brief Controller's peripherals interfaces
*/
namespace controller::interfaces{
  
/*!
  @brief Interface for Real Time Clock. Static class. CRT pattern
  @tparam <adapter> class of specific controller
*/
template<typename adapter>  
class IRTC{

  IRTC() = delete;

public:

  /*!
    @brief Initialization of RTC
    @param [in] timedate time and date
  */
  static bool Init(const utils::TimeDate& timedate = utils::TimeDate()){ return adapter::_Init(timedate); }

  /*!
    @brief Set time and date to RTC. Calls Set callbacks
  */
  static bool Set(const utils::TimeDate& timedate){ return adapter::_Set(timedate); }

  /*!
    @brief Set time to RTC. Calls Set callbacks
  */
  static bool Set(const utils::Time& time){ return adapter::_Set(time); }

  /*!
    @brief Set time and date to RTC. Calls Set callbacks
  */
  static bool Set(const utils::Date& date){ return adapter::_Set(date); }

  /*!
    @brief Set time and date for Alarm
  */
  static bool SetAlarm(const utils::TimeDate& timedate){ return adapter::_SetAlarm(timedate); }

  /*!
    @brief Set time of Alarm
  */
  static bool SetAlarm(const utils::Time& time){ return adapter::_SetAlarm(time); }

  /*!
    @brief Set date of Alarm
  */
  static bool SetAlarm(const utils::Date& date){ return adapter::_SetAlarm(date); }

  /*!
    @brief Get time and date of RTC
  */
  static const utils::TimeDate& Get(){ return adapter::_Get(); }

  /*!
    @brief Get time and date of Alarm
  */
  static const utils::TimeDate& GetAlarm(){ return adapter::_GetAlarm(); }

  /*!
    @brief Get enable-status of RTC
  */
  static bool IsEnabled(){ return adapter::_IsEnabled(); }

  /*!
    @brief Interrupt service routine. Calls Period and Alarm callbacks
  */
  static void ISR(){ adapter::_ISR(); }

  /*!
    @brief Handler for superloop or task. Calls Period and Alarm callbacks
  */
  static void Handler(){ adapter::_Handler(); }

  static inline void (*CallbackPeriod)() = nullptr;
  static inline void (*CallbackAlarm)() = nullptr;
  static inline void (*CallbackSet)() = nullptr;

protected:

  static inline const utils::TimeDate timeDateDefault{2021, 1, 1, 5, false};
  static inline utils::TimeDate timedate = timeDateDefault;
  static inline utils::TimeDate timedateAlarm;

  struct initialization{
    using power = typename adapter::power;
    using pins = typename adapter::pins;
    using interrupts = typename adapter::interrupts;
  };

};

} // !namespace controller::interfaces

#endif // !_IRTC_HPP