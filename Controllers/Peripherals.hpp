//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Includes periphery source of controller
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _PERIPHERALS_HPP
#define _PERIPHERALS_HPP

#include "Common/Controller_Define.hpp"

#ifdef STM32F1
  #if __has_include("Power/stm32f1_Power.hpp")
    #include "Power/stm32f1_Power.hpp"
  #endif
  #if __has_include("Pin/stm32f1_Pin.hpp")
    #include "Pin/stm32f1_Pin.hpp"
  #endif
  #if __has_include("Clock/stm32f1_Clock.hpp")
    #include "Clock/stm32f1_Clock.hpp"
  #endif
  #if __has_include("Pinlist/stm32f1_Pinlist.hpp")
    #include "Pinlist/stm32f1_Pinlist.hpp"
  #endif
  #if __has_include("UART/stm32f1_UART.hpp")
    #include "UART/stm32f1_UART.hpp"
  #endif
  #if __has_include("SPI/stm32f1_SPI.hpp")
    #include "SPI/stm32f1_SPI.hpp"
  #endif
  #if __has_include("External_Event/stm32f1_External_Event.hpp")
    #include "External_Event/stm32f1_External_Event.hpp"
  #endif
  #if __has_include("RTC/stm32f1_RTC.hpp")
    #include "RTC/stm32f1_RTC.hpp"
  #endif
// !STM32F1

#elif defined (STM32F0)
  #if __has_include("Power/stm32f0_Power.hpp")
    #include "Power/stm32f0_Power.hpp"
  #endif
  #if __has_include("Clock/stm32f0_Clock.hpp")
    #include "Clock/stm32f0_Clock.hpp"
  #endif
  #if __has_include("Pin/stm32f0_Pin.hpp")
    #include "Pin/stm32f0_Pin.hpp"
  #endif
  #if __has_include("External_Event/stm32f0_External_Event.hpp")
    #include "External_Event/stm32f0_External_Event.hpp"
  #endif
  #if __has_include("Pinlist/stm32f0_Pinlist.hpp")
    #include "Pinlist/stm32f0_Pinlist.hpp"
  #endif
#endif // !STM32F0

#include "Common/Core/Systick.hpp"
#include "Common/Core/Interrupt.hpp"

#endif // !_PERIPHERALS_HPP