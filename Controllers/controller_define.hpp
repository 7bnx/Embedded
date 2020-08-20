//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Includes for controller
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _CONTROLLER_DEFINE_HPP
#define _CONTROLLER_DEFINE_HPP

#if defined (STM32F10X_LD)    || defined (STM32F10X_LD_VL) || defined (STM32F10X_MD)    ||\
    defined (STM32F10X_MD_VL) || defined (STM32F10X_HD)    || defined (STM32F10X_HD_VL) ||\
    defined (STM32F10X_XL)    || defined (STM32F10X_CL)
  #define STM32F1
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
  using namespace stm32f1;
#endif

#endif //!_CONTROLLER_DEFINE_HPP