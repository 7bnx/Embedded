#ifndef _LED_CPP
#define _LED_CPP

#include "Led.hpp"

#define PARAMETERS typename Pin, bool isOnHigh
#define CLASS Led<Pin, isOnHigh>

namespace device{


/*!
  @brief Led Handler. Call it in super loop, or task. Each call increments Led's state counter
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
        ticksPauseCurrent = qtyBlinksCurrent = 0; 
        qtyBlinksRowCurrent++;  
        if ((qtyBlinksRowCurrent >= qtyBlinksRow && qtyBlinksRow) || (!ticksPause && !qtyBlinksRow))
          ticksOn = 0;
      }
    } else _BlinkHandler();
  }
}

/*!
  @brief Turns Led to "Blink mode". Led immediately goes to ON
  @param [in] ticksOn period, when Led is ON
  @param [in] ticksOff period, when Led is OFF
  @param [in] qtyBlinks number of times the Led will be blinking. = 0 - infinity blinking cycle
*/
template<PARAMETERS>
void CLASS::Blink(size_t ticksOn, size_t ticksOff, size_t qtyBlinks){
  BlinkRow(ticksOn, ticksOff, qtyBlinks);
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
void CLASS::BlinkRow(size_t ticksOn, size_t ticksOff, size_t qtyBlinks, size_t ticksPause, size_t qtyBlinksRow){
  if (isEnabled){
    _SetEmmitState(true, pinStateEmmit);
    _SetParameters(ticksOn, ticksOff, qtyBlinks, ticksPause, qtyBlinksRow);
  }
}

/*!
  @brief Turns Led to new state. Resets Blink(...) and BlinkRow(...) parameters
*/
template<PARAMETERS>
void CLASS::Set(bool state){
  if (isEnabled){
    ticksOn = 0;
    _SetEmmitState(state, !state ^ pinStateEmmit);
  }
}

/*!
  @brief Turns Led to permanent ON. Resets Blink(...) and BlinkRow(...) parameters
*/
template<PARAMETERS>
void CLASS::On(){
  Set(true);
}

/*!
  @brief Turns Led's to permanent OFF. Resets Blink(...) and BlinkRow(...) parameters
*/
template<PARAMETERS>
void CLASS::Off(){
  Set(false);
}

/*!
  @brief Changes emitting state of Led. Resets Blink(...) and BlinkRow(...) parameters
*/ 
template<PARAMETERS>
void CLASS::Toggle(){
  Set(!isOn);
}

template<PARAMETERS>
void CLASS::_SetEmmitState(bool isLedOn, bool pinState) {
  Pin::Set(pinState);
  isOn = isLedOn;
}

template<PARAMETERS>
void CLASS::_SetParameters(size_t _ticksOn, size_t _ticksOff, size_t _qtyBlinks, size_t _ticksPause, size_t _qtyBlinksRow){
  ticksOn = _ticksOn;
  ticksOff = _ticksOff;
  ticksPause = _ticksPause;
  ticksToNextState = _ticksOn;
  ticksPauseCurrent = 0;
  qtyBlinks = _qtyBlinks;
  qtyBlinksRow = _qtyBlinksRow;
  qtyBlinksCurrent = 0;
  qtyBlinksRowCurrent = 0;
}

template<PARAMETERS>
void CLASS::_BlinkHandler(){
  if (!ticksToNextState--){
    if (isOn){
      ticksToNextState = ticksOff;
      qtyBlinksCurrent++;
    } else  ticksToNextState = ticksOn;
    _SetEmmitState(!isOn, (bool)(isOn ^ pinStateEmmit));
  }
}

} // ! namespace device

#undef PARAMETERS
#undef CLASS

#endif // !_LED_CPP