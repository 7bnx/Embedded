#ifndef _BUTTON_CPP
#define _BUTTON_CPP

#include "Button.hpp"

#define PARAMETERS typename Pin, bool isActiveLow, \
                   size_t ticksDebounce, size_t ticksLongPress
#define CLASS Button<Pin, isActiveLow, ticksDebounce, ticksLongPress>

namespace device{

/*!
  @brief Button Handler. Call it in super loop, or task. Each call increments Buttons's state counter
*/
template<PARAMETERS>
void CLASS::Handler(){
  if (isEnabled){
    bool currentState = Pin::Get() ^ isActiveLow;
    if (currentState) _HandlerPress();
    else _HandlerRelease();
  }
}    

/*!
  @brief Enables or disables Button
*/  
template<PARAMETERS>
void CLASS::Enable(bool state){
  if (!state) {
    ticksPressed = 0;
    ticksPressedLong = 0;
    isPressed = false;
    isPressedLong = false;
  }
  isEnabled = state;
}  

/*!
  @brief Clears Callbacks functions
*/
template<PARAMETERS>
void CLASS::ResetCallbacks(){
  CallbackPressed = nullptr;
  CallbackPressedLong = nullptr;
  CallbackReleased = nullptr;
}

template<PARAMETERS>
void CLASS::_HandlerPress(){

  if (ticksPressed < ticksDebounce){
    ticksPressed++;
  } else{
    if (!isPressed){
      isPressed = true;
      if (CallbackPressed)
        CallbackPressed();
    }
  }

  if (ticksPressedLong < ticksLongPress){
    ticksPressedLong++;
  } else{
    if (!isPressedLong){
      isPressedLong = true;
      if (CallbackPressedLong)
        CallbackPressedLong();
    }
  }
}

template<PARAMETERS>
void CLASS::_HandlerRelease(){
  if (ticksPressed) ticksPressed--;
  else if (isPressed) {
    ticksPressedLong = 0;
    isPressed = isPressedLong = false;
    if (CallbackReleased)
      CallbackReleased();
  }
}

} // !device

#undef PARAMETERS
#undef CLASS

#endif // !_BUTTON_CPP