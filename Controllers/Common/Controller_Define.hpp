//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Define for controllers
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _CONTROLLER_DEFINE_HPP
#define _CONTROLLER_DEFINE_HPP

#if defined (STM32F10X_LD)    || defined (STM32F10X_LD_VL) || defined (STM32F10X_MD)    ||\
    defined (STM32F10X_MD_VL) || defined (STM32F10X_HD)    || defined (STM32F10X_HD_VL) ||\
    defined (STM32F10X_XL)    || defined (STM32F10X_CL)
  #define STM32F1
  #define CORTEX_M3
#elif defined (STM32F030x8) || defined (STM32F0x1)
  #define STM32F0
  #define CORTEX_M0
#else 
  #error "File: controller_define.hpp.\
    Controller define should match one of the listed in this file. E.g.: if controller = stm32f103c8, then #define STM32F10X_MD"
#endif

#endif // !_CONTROLLER_DEFINE_HPP