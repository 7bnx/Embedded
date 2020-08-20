#ifndef _LED_CPP
#define _LED_CPP

#include "Led.hpp"

#define PARAMETERS typename Pin, bool isOnHigh
#define CLASS Led<Pin, isOnHigh>

namespace device{

/*!
  @brief Led Handler. Put it in your infinity loop, or task. Each call increments Led's state counter
*/
template<PARAMETERS>
void CLASS::Handler(){
  if (ticksOn){
  /*If Blink or BlinkRow is active -> :*/
    if (qtyBlinksCurrent >= qtyBlinks && qtyBlinks){
    /*If counter of blink reaches the qty of blinks ->*/
      ticksPauseCurrent++;
      if (ticksPauseCurrent >= ticksPause){
      /*If counter of pause between blinks sequences reaches the limit ->*/
        qtyBlinksRowCurrent++;
        ticksPauseCurrent = 0;
        qtyBlinksCurrent = 0;
        if ((qtyBlinksRowCurrent >= qtyBlinksRow && 
           ticksPause && qtyBlinksRow) || !ticksPause) ticksOn = 0;       
      }
    } else BlinkHandler();
  }
}

/*!
  @brief Turns Led to "Blink mode". Led immediately goes to ON
  @param [in] ticksOn period, when Led is ON
  @param [in] ticksOff period, when Led is OFF
  @param [in] qtyBlinks number of times the Led will be blinking. = 0 - infinity blinking cycle
*/
template<PARAMETERS>
void CLASS::Blink(uint32_t ticksOn, uint32_t ticksOff, uint32_t qtyBlinks){
  BlinkRow(ticksOn,ticksOff, qtyBlinks);
}

/*!
  @brief Turns Led to "BlinkRow mode". It makes pause between blinking sequence.    \n
  E.g: Led::BlinkRow(1,2,2,3,2) -> |--|-----|--|--  -> Led turns off
  @param [in] ticksOn period, when Led is ON
  @param [in] ticksOff period, when Led is OFF
  @param [in] qtyBlinks number of times the Led will be blinking. = 0 - Led goes to "Blink mode
  @param [in] ticksPause period between blinking sequence
  @param [in] qtyBlinksRow number of times the blinking sequence. = 0 infinity cycle
*/
template<PARAMETERS>
void CLASS::BlinkRow(uint32_t ticksOn, uint32_t ticksOff, 
                     uint32_t qtyBlinks, uint32_t ticksPause, uint32_t qtyBlinksRow){
  if (isEnabled && ticksOn){
    Pin::Set(pinStateToSetOn);
    isOn = true;
    SetParam(ticksOn, ticksOff, qtyBlinks, ticksPause, qtyBlinksRow);
  }
}

/*!
  @brief Turns Led to new state. Resets Blink(...) and BlinkRow(...) parameters
*/
template<PARAMETERS>
void CLASS::Set(bool state){
  bool pinState = state ? pinStateToSetOn : !pinStateToSetOn;
  SetLedHelper(state, pinState);
}

/*!
  @brief Turns Led to permanent ON. Resets Blink(...) and BlinkRow(...) parameters
*/
template<PARAMETERS>
void CLASS::On(){
  SetLedHelper(true, pinStateToSetOn);
}

/*!
  @brief Turns Led's to permanent OFF. Resets Blink(...) and BlinkRow(...) parameters
*/
template<PARAMETERS>
void CLASS::Off(){
  SetLedHelper(false, !pinStateToSetOn);
}

/*!
  @brief Changes emitting state of Led. Resets Blink(...) and BlinkRow(...) parameters
*/ 
template<PARAMETERS>
void CLASS::Toggle(){
  Set(!isOn);
}

template<PARAMETERS>
void CLASS::SetLedHelper(bool isLedOn, bool pinState) {
  if (isEnabled) {
    Pin::Set(pinState);
    isOn = isLedOn;
  }
}

template<PARAMETERS>
void CLASS::SetParam(uint32_t _ticksOn, uint32_t _ticksOff, 
                    uint32_t _qtyBlinks, uint32_t _ticksPause, uint32_t _qtyBlinksRow){
  ticksOn = _ticksOn;
  ticksOff = _ticksOff;
  qtyBlinks = _qtyBlinks;
  ticksPause = _ticksPause;
  qtyBlinksRow = _qtyBlinksRow;
  ticksToNextState = _ticksOn;
  ticksBlinkCurrent = 0;
  ticksPauseCurrent = 0;
  qtyBlinksCurrent = 0;
  qtyBlinksRowCurrent = 0;
}

template<PARAMETERS>
void CLASS:: BlinkHandler(){
  ticksBlinkCurrent++;
  if (ticksBlinkCurrent >= ticksToNextState){
		ticksToNextState = isOn ? ticksOff : ticksOn;
    if (isOn)qtyBlinksCurrent++;
		isOn = !isOn;
    Pin::Set(isOn ? pinStateToSetOn : !pinStateToSetOn);
    ticksBlinkCurrent = 0;
  }
}

} // !device

#undef PARAMETERS
#undef CLASS

#endif // !_LED_CPP