//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Single Button driver
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _BUTTON_H
#define _BUTTON_H

#include <cstddef>
#include "Compiler.h"

/*!
  @file
  @brief Button driver.
*/

/*!
  @brief Namespace for devices
*/
namespace device{

/*!
  @brief Class of Button Object
  @tparam <Pin> hardware-dependent input. Must implements field 'bool Pin::Get()'
  @tparam <isActiveLow> if true, button goes pressed, when Pin input-state is LOW
  @tparam <ticksDebounce> number of ticks to filter input state
  @tparam <ticksLongPress> number of ticks to activate the Long Press Event
*/
template<typename Pin, bool isActiveLow = false,
         size_t ticksDebounce = 30, size_t ticksLongPress = 1500>
class Button{
  
public:

  /*!
    @brief Button Handler. Call it in super loop, or task. Each call increments Buttons's state counter
  */
  static void Handler();

  /*!
    @brief Enables or disables Button
  */
  static void Enable(bool state = true);

  /*!
    @brief Returns the enable-state of Button
  */
  __FORCE_INLINE static bool IsEnabled(){return isEnabled;}

  /*!
    @brief Returns the pressed-state of Button
  */
  __FORCE_INLINE static bool IsPressed(){ return isPressed; }

  /*!
    @brief Returns the long presseed state of Button
  */
  __FORCE_INLINE static bool IsPressedLong(){ return isPressedLong; }

  /*!
    @brief Clears Callbacks functions
  */
  static void ResetCallbacks();

  /*!
      @brief Use in conditional statements. E.g.: if (button) {...}
      @return press-state of Button
  */
  __FORCE_INLINE operator bool() const { return isPressed; }

  /*!
    @brief Executes when the button is pressed
  */
  static inline void (*CallbackPressed)() = nullptr;

  /*!
    @brief Executes when the button is long pressed
  */
  static inline void (*CallbackPressedLong)() = nullptr;

  /*!
    @brief Executes when the button is released
  */
  static inline void (*CallbackReleased)() = nullptr;

private:
  static inline bool isEnabled = true;
  static inline bool isPressed = false;
  static inline bool isPressedLong = false;
  static inline size_t ticksPressed = 0;
  static inline size_t ticksPressedLong = 0;

  static void _HandlerPress();
  static void _HandlerRelease();
};

} // !device

#include "Button.cpp"

#endif // !_BUTTON_H