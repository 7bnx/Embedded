//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Includes periphery source of controller
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _CONTROLLER_PERIPHERY_HPP
#define _CONTROLLER_PERIPHERY_HPP

#include "controller_define.hpp"

#ifdef STM32F1

  #if __has_include("stm32f10x.h")
    #include "stm32f10x.h"
  #endif
  #if __has_include("stm32f1_Power.hpp")
    #include "stm32f1_Power.hpp"
  #endif
  #if __has_include("stm32f1_Pin.hpp")
    #include "stm32f1_Pin.hpp"
  #endif
  #if __has_include("stm32f1_Clock.hpp")
    #include "stm32f1_Clock.hpp"
  #endif
  #if __has_include("stm32f1_Pinlist.hpp")
    #include "stm32f1_Pinlist.hpp"
  #endif
  #if __has_include("stm32f1_UART.hpp")
    #include "stm32f1_UART.hpp"
  #endif
  #if __has_include("stm32f1_SPI.hpp")
    #include "stm32f1_SPI.hpp"
  #endif

#endif // !STM32F1

#include "systick.hpp"
#include "interrupt.hpp"
#include "connection.hpp"

#endif // !_CONTROLLER_PERIPHERY_HPP