//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Handle HMI-Button.
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _BUTTON_H
#define _BUTTON_H

#include <cstdint>
#include "Callback.hpp"

/*!
  @file
  @brief Button driver.
*/

/*!
  @brief Namespace for devices.
*/
namespace device{

/*!
  @brief Class of Button Object.
  @tparam Pin hardware-dependent input. Must implements field 'bool Pin::Get()'
  @tparam isActiveLow if true, button goes pressed, when Pin input-state is LOW
  @tparam ticksDebounce number of ticks to filter input state
  @tparam ticksLongPress number of ticks to activate the Long Press Event
*/
template<typename Pin, bool isActiveLow = false,
         uint32_t ticksDebounce = 30, uint32_t ticksLongPress = 1500>
class Button{
public:

  /*!
    @brief Handles the button. Put it in your infinity loop, or task
  */
  static void Handler();

  /*!
    @brief Enables or disables Button. When disabled, Events don't execute
  */
  static void Enable(bool state = true);

  /*!
    @brief Returns the enable state of Button
  */
  __attribute__ ((always_inline)) inline
  static bool IsEnabled(){return isEnabled;}

  /*!
    @brief Returns the pressed state of Button
  */
  __attribute__ ((always_inline)) inline
  static bool IsPressed(){return isPressed;}

  /*!
    @brief Returns the long presseed state of Button
  */
  __attribute__ ((always_inline)) inline
  static bool IsPressedLong(){return isPressedLong;}

  /*!
      @brief Use in conditional statements. E.g.: if (button) {...}
      @return press state of Button
  */
  __attribute__ ((always_inline)) inline
  operator bool(){return isPressed;}

  /*!
    @brief Clears Events functions.
  */
  static void ResetCallbacks();

  /*!
    @brief Executes when the button is pressed.  \n
    Usage: Like lambda with optional 1 variable in capture list, or regular function
  */
  static inline utils::Callback CallbackPressed;

  /*!
    @brief Executes when the button is long pressed.   \n
    Usage: Like lambda with optional 1 variable in capture list, or regular function
  */
  static inline utils::Callback CallbackPressedLong;

  /*!
    @brief Executes when the button is released.    \n
    Usage: Like lambda with optional 1 variable in capture list, or regular function
  */
  static inline utils::Callback CallbackReleased;

private:
  static inline bool isEnabled = true;
  static inline bool isPressed = false;
  static inline bool isPressedLong = false;
  static inline uint32_t ticksPressed = 0;
  static inline uint32_t ticksPressedLong = 0;

  static void HandlerPress();
  static void HandlerRelease();
};

} // !device

#include "Button.cpp"

#endif // !_BUTTON_H