#ifndef _BUTTON_CPP
#define _BUTTON_CPP

#include "Button.hpp"

#define PARAMETERS typename Pin, bool isActiveLow, \
									 uint32_t ticksDebounce, uint32_t ticksLongPress
#define CLASS Button<Pin, isActiveLow, ticksDebounce, ticksLongPress>

namespace device{

/*!
  @brief Handles the button. Put it in your infinity loop, or task
*/
template<PARAMETERS>
void CLASS::Handler(){
  if (isEnabled){
		bool currentState = Pin::Get() ^ isActiveLow;
		if (currentState) HandlerPress();
		else HandlerRelease();
	}
}    

/*!
  @brief Enables or disables Button. When disabled, Events don't execute
*/  
template<PARAMETERS>
void CLASS::Enable(bool state){
  isEnabled = state;
  if (state) {
    ticksPressed = 0;
    ticksPressedLong = 0;
    isPressed = false;
    isPressedLong = false;
  }
}  

/*!
  @brief Clears Events functions
*/
template<PARAMETERS>
void CLASS::ResetCallbacks(){
  CallbackPressed.Reset();
  CallbackPressedLong.Reset();
  CallbackReleased.Reset();
}

template<PARAMETERS>
void CLASS::HandlerPress(){
  if(!isPressed && (ticksPressed++ >= ticksDebounce)){
    CallbackPressed();
		isPressed = true;
  }
  if (!isPressedLong && isPressed && (ticksPressedLong++ >= ticksLongPress)){
    isPressedLong = true;
		CallbackPressedLong();
	}
}

template<PARAMETERS>
void CLASS::HandlerRelease(){
  if (ticksPressed) ticksPressed--;
  else if (isPressed) {
    ticksPressedLong = 0;
    isPressed = false;
    isPressedLong = false;
		CallbackReleased();
  }
}

} // !device

#undef PARAMETERS
#undef CLASS

#endif // !_BUTTON_CPP