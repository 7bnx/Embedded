//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/
//  Description:  Simple library to handle few Led operations.
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _LED_H
#define _LED_H

#include <cstdint>

/*!
  @file
  @brief Led driver. Header.
*/

/*!
  @brief Namespace for Led.
*/
namespace nLed{

/*!
  @brief Class of Led Object.
  @tparam Pin hardware-dependent output. Must implements field void Pin::Set(bool state)
  @tparam isOnHigh  if true, Led starts emitting, when Pin goes high
*/  
template<typename Pin, bool isOnHigh = true>
class Led{
public:    
	/*!
    @brief Led Handler. Put it in your infinity loop, or task. Each call increments Led's state counter
  */
	static void Handler();
  /*!
    @brief Turns Led to "Blink mode". Led immediately goes to ON
    @param [in] ticksOn period, when Led is ON
    @param [in] ticksOff period, when Led is OFF
    @param [in] qtyBlinks number of times the Led will be blinking. = 0 - infinity blinking cycle
  */
  static void Blink(uint32_t ticksOn, uint32_t ticksOff, uint32_t qtyBlinks = 0);
  /*!
    @brief Turns Led to "BlinkRow mode". It makes pause between blinking sequence.   \n
    E.g: Led::BlinkRow(1,2,2,3,2) -> |--|-----|--|--  -> Led turns off
    @param [in] ticksOn period, when Led is ON
    @param [in] ticksOff period, when Led is OFF
    @param [in] qtyBlinks number of times the Led will be blinking. = 0 - Led goes to "Blink mode
    @param [in] ticksPause period between blinking sequence
    @param [in] qtyBlinksRow number of times the blinking sequence. = 0 infinity cycle
  */
  static void BlinkRow(uint32_t ticksOn, uint32_t ticksOff, uint32_t qtyBlinks, 
                       uint32_t ticksPause = 0, uint32_t qtyBlinksRow = 0);
  /*!
    @brief Turns Led to new state. Resets Blink(...) and BlinkRow(...) parameters
  */
  static void Set(bool isLedOn);
  /*!
    @brief Turns Led to permanent ON. Resets Blink(...) and BlinkRow(...) parameters
  */
  static void On();
  /*!
    @brief Turns Led's to permanent OFF. Resets Blink(...) and BlinkRow(...) parameters
  */
  static void Off();
  /*!
    @brief Changes emitting state of Led. Resets Blink(...) and BlinkRow(...) parameters
  */  
  static void Toggle();
  /*!
    @brief Returns current emitting state of Led
  */
  __attribute__ ((always_inline)) inline 
  static bool IsOn(){return isOn;}
  /*!
    @brief Enables or disables Led. When disabled, another methods take no effect
  */
  __attribute__ ((always_inline)) inline 
  static void Enable(bool state = true){isEnabled = state;}
  /*!
    @brief Returns the current state of Led
  */
  __attribute__ ((always_inline)) inline 
  static bool IsEnable(){return isEnabled;}
	/*!
    @brief Use in conditional statements. E.g.: if (led) {...}
		@return emitting state
  */
  __attribute__ ((always_inline)) inline 
  operator bool() const {return isOn;};
  /*!
    @brief Assigning the the new state to the Led
    @param [in] state new emitting state
  */
  __attribute__ ((always_inline)) inline
  const bool operator=(bool state){Set(state); return state;}

private:  
  static inline bool isEnabled = true; 
  static inline bool isOn = false;
  static inline uint32_t ticksOn = 0;
  static inline uint32_t ticksOff = 0;
  static inline uint32_t ticksPause = 0;
  static inline uint32_t ticksBlinkCurrent = 0;
  static inline uint32_t ticksPauseCurrent = 0;
  static inline uint32_t ticksToNextState = 0;
  static inline uint32_t qtyBlinks = 0;
  static inline uint32_t qtyBlinksRow = 0;
  static inline uint32_t qtyBlinksCurrent = 0;
  static inline uint32_t qtyBlinksRowCurrent = 0;
  static inline bool constexpr pinStateToSetOn = isOnHigh ? true : false;

  static void SetLedHelper(bool isLedOn, bool pinSate);
  static void SetParam(uint32_t ticksOn = 0, uint32_t ticksOff = 0, uint32_t qtyBlinks = 0, 
                uint32_t ticksPause = 0, uint32_t qtyBlinksRow = 0);
  static void BlinkHandler();
	
};

}  // !nLed

#include "Led.cpp"

#endif // !_LED_H
//----------------------------------------------------------------------------------