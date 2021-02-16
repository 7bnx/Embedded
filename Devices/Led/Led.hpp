//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Single Led driver
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _LED_HPP
#define _LED_HPP

#include <cstddef>
#include "Compiler.h"

/*!
  @file
  @brief Led driver
*/

/*!
  @brief Namespace for devices
*/
namespace device{

/*!
  @brief Class of Led Object.
  @tparam <Pin> hardware-dependent output. Must implements field void Pin::Set(bool state)
  @tparam <isOnHigh> if true, Led starts emitting, when Pin goes high
*/  
template<typename Pin, bool isOnHigh = true>
class Led{

public:

  /*!
    @brief Led Handler. Call it in super loop, or task. Each call increments Led's state counter
  */
  static void Handler();
  
  /*!
    @brief Turns Led to "Blink mode". Led immediately goes to ON
    @param [in] ticksOn period, when Led is ON
    @param [in] ticksOff period, when Led is OFF
    @param [in] qtyBlinks number of times the Led will be blinking. = 0 - infinity blinking cycle
  */
  __FORCE_INLINE static void Blink(size_t ticksOn, size_t ticksOff, size_t qtyBlinks = 0);

  /*!
    @brief Turns Led to "BlinkRow mode". It makes pause between blinking sequence.   \n
    E.g: Led::BlinkRow(1,2,2,3,2) -> |--|-----|--|--  -> Led turns off
    @param [in] ticksOn period, when Led is ON
    @param [in] ticksOff period, when Led is OFF
    @param [in] qtyBlinks number of times the Led will be blinking. = 0 - Led goes to "Blink mode
    @param [in] ticksPause period between blinking sequence
    @param [in] qtyBlinksRow number of times the blinking sequence. = 0 infinity cycle
  */
  static void BlinkRow(size_t ticksOn, size_t ticksOff, size_t qtyBlinks, 
                       size_t ticksPause = 0, size_t qtyBlinksRow = 0);

  /*!
    @brief Turns Led to new state. Resets Blink(...) and BlinkRow(...) parameters
  */
  static void Set(bool isLedOn);

  /*!
    @brief Turns Led to permanent ON. Resets Blink and BlinkRow parameters
  */
  static void On();

  /*!
    @brief Turns Led's to permanent OFF. Resets Blink and BlinkRow parameters
  */
  static void Off();

  /*!
    @brief Changes emitting state of Led. Resets Blink and BlinkRow parameters
  */  
  __FORCE_INLINE static void Toggle();

  /*!
    @brief Returns current emitting state of Led
  */
  __FORCE_INLINE static bool IsOn(){ return isOn; }

  /*!
    @brief Enables or disables Led. When disabled, another methods take no effect
  */
  __FORCE_INLINE static void Enable(bool state = true){ isEnabled = state; ticksOn = 0; }

  /*!
    @brief Returns the current state of Led
  */
  __FORCE_INLINE static bool IsEnable(){ return isEnabled; }

  /*!
    @brief Use in conditional statements. E.g.: if (led) {...}
    @return emitting state
  */
  __FORCE_INLINE operator bool() const { return isOn; }
  
  /*!
    @brief Assigning the the new state to the Led
    @param [in] state new emitting state
  */
  __FORCE_INLINE bool operator=(bool state){ Set(state); return state; }

private:  
  static inline bool isEnabled = true; 
  static inline bool isOn = false;
  static inline size_t ticksOn = 0;
  static inline size_t ticksOff = 0;
  static inline size_t ticksPause = 0;
  static inline size_t ticksToNextState = 0;
  static inline size_t ticksPauseCurrent = 0;
  static inline size_t qtyBlinks = 0;
  static inline size_t qtyBlinksRow = 0;
  static inline size_t qtyBlinksCurrent = 0;
  static inline size_t qtyBlinksRowCurrent = 0;
  static inline bool constexpr pinStateEmmit = isOnHigh ? true : false;

  static void _SetEmmitState(bool isLedOn, bool pinSate);
  static void _SetParameters(size_t ticksOn = 0, size_t ticksOff = 0, size_t qtyBlinks = 0, 
                size_t ticksPause = 0, size_t qtyBlinksRow = 0);
  __FORCE_INLINE static void _BlinkHandler();
  
};

}  // !namespace device

#include "Led.cpp"

#endif // !_LED_HPP