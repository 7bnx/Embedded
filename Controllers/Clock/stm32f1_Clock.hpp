//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Hardware-dependent driver for Clock Control. 
//                STM32F1xx series. Exclude Connectivity line
//  TODO:         Add support for Connectivity Line
//----------------------------------------------------------------------------------


#ifndef _STM32F1_CLOCK_HPP
#define _STM32F1_CLOCK_HPP

#include <cstdint>
#include "stm32f1_Pin.hpp"
#include "stm32f1_Power.hpp"
#include "type_traits_custom.hpp"

#ifdef STM32F1

namespace controller{

/*!
  @brief Configuration for MCO(Microcontroller clock output)
*/ 
enum class mcoSource{

  /*! @brief No clock output at pin*/ 
  NO_CLOCK = 0UL,

  /*! @brief System clock (SYSCLK) selected*/ 
  SYS_CLOCK = 0x4000000UL,

  /*! @brief HSI clock selected.*/ 
  HSI_CLOCK = 0x5000000UL,

  /*! @brief HSE clock selected*/ 
  HSE_CLOCK = 0x6000000UL,

  /*! @brief PLL clock divided by 2 selected*/ 
  PLL_DIV2_CLOCK = 0x7000000
};
#endif //! STM32F1
} // !namespace controller

namespace stm32f1::hardware{

/*!
  @brief Clock Driver for STM32F1 series. Don't use it Directly
*/ 
class _ClockImplementation{
public:

  /*!
    @brief Set controller clock.    \n 
    ADC-clock is set to value/2.    \n
    USB-clock is set to 48MHz in case of value = 72MHz, otherwise to value.    \n
    AHB-clock is set to value.
    APB1-clock is set to 36MHz in case of value\2 >= 36MHz, othervise to value    \n
    APB2-clock is set to value.
    @tparam <value> new value for system clock (in Hz)
		@tparam <isExternalSrc> if true, then controller is driven by external source(HSE), otherwise - by internal(HSI). Default value is HSE
    @tparam <srcValue> HSE value (in Hz). Skip it in case of HSI
  */
  template<uint32_t value = 72000000, bool isExternalSrc = true, uint32_t srcValue = 8000000>
  static bool Set(){

    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    RCC->CFGR = 0;
    RCC->CIR = RCCIRQFLAGSRESETVALUE;
    
    if constexpr(isExternalSrc){
      RCC->CR = RCC_CR_HSION | RCC_CR_HSEON | pllOnValueCR<value, srcValue> | (RCC->CR & (RCC_CR_HSITRIM | RCC_CR_HSICAL));
      uint32_t i = 0;
      for (; !(RCC->CR & RCC_CR_HSERDY) && i < HSEONTIMEOUT ;i++);
      if (i > HSEONTIMEOUT) return false;
    } else RCC->CR = RCC_CR_HSION  | pllOnValueCR<value, srcValue> | (RCC->CR & (RCC_CR_HSITRIM | RCC_CR_HSICAL));
    
    FLASH->ACR = (FLASH->ACR &(~FLASH_ACR_LATENCY)) | FLASH_ACR_PRFTBE | flashLatency<value>;
    if constexpr (value > srcValue){
      RCC->CFGR = cfgrValueConfigWithPLL<value, isExternalSrc, srcValue>();

      while (!(RCC->CR & RCC_CR_PLLRDY));
      RCC->CFGR = cfgrValueConfigWithPLL<value, isExternalSrc, srcValue>() | RCC_CFGR_SW_PLL;
    } else RCC->CFGR = cfgrValueConfig<value, isExternalSrc, srcValue>();

    if constexpr(srcToCheck<value, isExternalSrc, srcValue> == RCC_CFGR_SWS_PLL  ||
                srcToCheck<value, isExternalSrc, srcValue> == RCC_CFGR_SWS_HSE)
      while ((RCC->CFGR & RCC_CFGR_SWS )!= srcToCheck<value, isExternalSrc, srcValue>);
    if constexpr (isExternalSrc) RCC->CR &=~ RCC_CR_HSION;

    clockSystem = value;
    clockAHB = clockSystem; // !This expression is correct, if RCC_CFGR_HPRE is set to default (SYSCLK not divided)
    clockAPB1 = CalcAPB<cfgrValueConfig<value, isExternalSrc, srcValue>(), RCC_CFGR_PPRE1, APB1PRESCSTARTBIT, 1>();
    clockAPB1TIM = CalcAPB<cfgrValueConfig<value, isExternalSrc, srcValue>(), RCC_CFGR_PPRE1, APB1PRESCSTARTBIT, 2>();
    clockAPB2 = clockAHB;
    clockAPB2TIM = clockAHB;
    clockADC = clockAHB / ADCDIVIDER[0];
    clockUSB = cfgrvValuePreUSB<value> ? clockAHB : (clockAHB * 2) / 3;
    return true;
  }
  
  /*!
    @brief Set controller clock.    \n 
    ADC-clock is set to value/2.    \n
    USB-clock is set to 48MHz in case of value = 72MHz, otherwise to value.    \n
    AHB-clock is set to value.
    APB1-clock is set to 36MHz in case of value\2 >= 36MHz, othervise to value    \n
    APB2-clock is set to value.
    @param [in] value new value for system clock (in Hz)
		@param [in] isExternalSrc if true, then controller is driven by external source(HSE), otherwise - by internal(HSI). Default value is HSE
    @param [in] srcValue HSE value (in Hz). Skip it in case of HSI
  */
  static bool Set(uint32_t value = 72000000, bool isExternalSrc = true, uint32_t srcValue = 8000000){
    SwitchToHSI();
    if (isExternalSrc){
      if(!EnableHSE()) return false;
    }

    uint32_t flashLatency = FLASH_ACR_LATENCY_0;
    if (value >= 48000000) flashLatency = FLASH_ACR_LATENCY_2;
    else if (value >= 24000000) flashLatency = FLASH_ACR_LATENCY_1;
    FLASH->ACR = (FLASH->ACR &(~FLASH_ACR_LATENCY)) | FLASH_ACR_PRFTBE | flashLatency;

    uint32_t cfgrValue = value == 72000000 ? 0 : RCC_CFGR_USBPRE;
    if (value >= (MAXSYSVALUE/2)) cfgrValue |= RCC_CFGR_PPRE1_DIV2;

    uint32_t srcToCheck = static_cast<uint32_t>(isExternalSrc) << 2UL;
    if (value > srcValue){
      uint32_t mul = value / srcValue;
      uint32_t div = value % srcValue;
      if (!isExternalSrc || (isExternalSrc && div)){
        mul*=2UL;
        cfgrValue |= RCC_CFGR_PLLXTPRE_HSE_Div2;
      }

      if (div) mul++;

      if (mul > PLLMULMAX || mul < 2) return false;

      cfgrValue |= (mul-2) << PLLMULSTARTBIT;

      if (isExternalSrc) cfgrValue |= RCC_CFGR_PLLSRC_HSE;

      RCC->CFGR |= cfgrValue;
      RCC->CR |= RCC_CR_PLLON;

      while (!(RCC->CR & RCC_CR_PLLRDY));
      cfgrValue |= RCC_CFGR_SW_PLL;
      srcToCheck = RCC_CFGR_SWS_PLL;
    } else cfgrValue |= static_cast<uint32_t>(isExternalSrc);

    RCC->CFGR |= cfgrValue;

    while ((RCC->CFGR & RCC_CFGR_SWS )!= srcToCheck);

    if (isExternalSrc)RCC->CR &=~ RCC_CR_HSION;

    clockSystem = value;
    clockAHB = clockSystem; // !This expression is correct, if RCC_CFGR_HPRE is set to default (SYSCLK not divided)
    clockAPB1 = CalcAPB(cfgrValue, RCC_CFGR_PPRE1, APB1PRESCSTARTBIT, 1);
    clockAPB1TIM = CalcAPB(cfgrValue, RCC_CFGR_PPRE1, APB1PRESCSTARTBIT, 2);
    clockAPB2 = clockAHB;
    clockAPB2TIM = clockAHB;
    clockADC = clockAHB / ADCDIVIDER[0];
    clockUSB = cfgrValue & RCC_CFGR_USBPRE ? clockAHB : (clockAHB * 2) / 3;
    return true;
  }

  /*!
    @brief Enable or disable MCO
    @tparam <src> source of MCO. Set to mcoSrc::NO_CLOCK to disable MCO
		@tparam <configureOutPin> if true, then MCO-pin will be configured
  */
  template<controller::mcoSource src = controller::mcoSource::NO_CLOCK, bool configureOutPin = true>
  static void EnableMCO(){
    if constexpr(configureOutPin && src != controller::mcoSource::NO_CLOCK){

      _PowerImplementation::Enable<GPIOA_BASE, AFIO_BASE>();
      _PinImplementation<GPIOA_BASE, 8, controller::pinConfiguration::Alternative_PushPull>::Init();
    }
    RCC->CFGR = (RCC->CFGR & (~ RCC_CFGR_MCO)) | static_cast<uint32_t>(src);
  }

  /*!
    @brief Returns value of System-clock(SYSCLCK) in Hz
  */
  __attribute__ ((always_inline)) inline
  static uint32_t System(){return clockSystem;}

  /*!
    @brief Returns value of AHB in Hz
  */
  __attribute__ ((always_inline)) inline
  static uint32_t AHB(){return clockAHB;}

  /*!
    @brief Returns value of APB1 in Hz
  */
  __attribute__ ((always_inline)) inline
  static uint32_t APB1(){return clockAPB1;}

  /*!
    @brief Returns value of APB2 in Hz
  */
  __attribute__ ((always_inline)) inline
  static uint32_t APB2(){return clockAPB2;}

  /*!
    @brief Returns value of ADC in Hz
  */
  __attribute__ ((always_inline)) inline
  static uint32_t ADC(){return clockADC;}

  /*!
    @brief Returns value of USB in Hz
  */
  __attribute__ ((always_inline)) inline
  static uint32_t USB(){return clockUSB;}

  /*!
    @brief Returns value for APB1 Timers in Hz
  */
  __attribute__ ((always_inline)) inline
  static uint32_t APB1TIM(){return clockAPB1TIM;}

  /*!
    @brief Returns value for APB2 Timers in Hz
  */
  __attribute__ ((always_inline)) inline
  static uint32_t APB2TIM(){return clockAPB2TIM;}

private:

  _ClockImplementation() = delete;

  static inline uint32_t clockSystem = 0;
  static inline uint32_t clockAHB = 0;
  static inline uint32_t clockAPB1 = 0;
  static inline uint32_t clockAPB2 = 0;
  static inline uint32_t clockUSB = 0;
  static inline uint32_t clockADC = 0;
  static inline uint32_t clockAPB1TIM = 0;
  static inline uint32_t clockAPB2TIM = 0;

  static const uint8_t PLLMULOFFSET = 2;
  static const uint8_t PLLMULMAX = 16; /* Max multiplier of PLL*/
  static const uint8_t PLLMULSTARTBIT = 18;
  static const uint32_t MAXSYSVALUE = 72000000;

  static const uint8_t AHBPRESCSTARTBIT = 4;
  static const uint8_t AHBDIVIDEREN = 8;
  static constexpr uint8_t AHBDIVIDER[] = {1,2,3,4,6,7,8,9}; /*left shifted*/

  static const uint8_t APBDEVIDEREN = 4;
  static const uint8_t APB1PRESCSTARTBIT = 8;
  static const uint8_t APB2PRESCSTARTBIT = 11;

  static constexpr uint8_t ADCDIVIDER[] = {2, 4, 6, 8}; 
  static const uint8_t ADCPRESCSTARTBIT = 14;

  static const uint32_t HSEONTIMEOUT = 500;

  static const uint32_t RCCIRQFLAGSRESETVALUE = 0x009F1F9F;

  template<uint32_t value>
  static constexpr uint32_t flashLatency = value >= 48000000 ? FLASH_ACR_LATENCY_2 :
                                           value >= 24000000 ? FLASH_ACR_LATENCY_1 :
                                                               FLASH_ACR_LATENCY_0;

  template<uint32_t value, bool isExternalSrc, uint32_t srcValue>
  __attribute__ ((always_inline)) inline
  static constexpr uint32_t mul(){
    uint32_t mul = value / srcValue;
    uint32_t div = value % srcValue;
    if (!isExternalSrc || (isExternalSrc && div)) mul*=2UL;
    if (div) mul++;
    return mul;
  }

  template<uint32_t value, bool isExternalSrc, uint32_t srcValue>
  __attribute__ ((always_inline)) inline
  static constexpr uint32_t cfgrValueConfigWithPLL(){
    uint32_t cfgrValue = 0;
    if (value >= (MAXSYSVALUE/2)) cfgrValue |= RCC_CFGR_PPRE1_DIV2;
    if (value > srcValue){
      if (!isExternalSrc || (isExternalSrc && (value % srcValue))) 
        cfgrValue |= RCC_CFGR_PLLXTPRE_HSE_Div2;

      cfgrValue |= (mul<value, isExternalSrc, srcValue>()-2) << PLLMULSTARTBIT;

      if (isExternalSrc) cfgrValue |= RCC_CFGR_PLLSRC_HSE;
    }
    return cfgrValue | cfgrvValuePreUSB<value>;
  }

  template<uint32_t value, bool isExternalSrc, uint32_t srcValue>
  __attribute__ ((always_inline)) inline
  static constexpr uint32_t cfgrValueConfig(){
    uint32_t cfgrValue = 0;
    if (value >= (MAXSYSVALUE/2)) cfgrValue |= RCC_CFGR_PPRE1_DIV2;
    if (value > srcValue) cfgrValue |= RCC_CFGR_SW_PLL;
    else cfgrValue |= static_cast<uint32_t>(isExternalSrc);
    return cfgrValue | cfgrvValuePreUSB<value>;
  }

  template<uint32_t value, bool isExternalSrc, uint32_t srcValue>
  static constexpr uint32_t srcToCheck = value > srcValue ? RCC_CFGR_SWS_PLL : 
                                         isExternalSrc    ? RCC_CFGR_SWS_HSE :
                                                            RCC_CFGR_SWS_HSI;

  template<uint32_t cfgrValue, uint32_t bitMask, uint32_t startBit, uint32_t mulTim>
  __attribute__ ((always_inline)) inline
  static constexpr uint32_t CalcAPB(){
    uint32_t prescallerAPB = (cfgrValue & bitMask) >> startBit;
    if (prescallerAPB >= APBDEVIDEREN){
      prescallerAPB &=~APBDEVIDEREN;
      return (clockAHB >> (prescallerAPB + 1)) * mulTim;
    } else{
      return clockAHB;
    }
  }

  __attribute__ ((always_inline)) inline
  static uint32_t CalcAPB(uint32_t cfgrValue, uint32_t bitMask, uint32_t startBit, uint32_t mulTim){
    uint32_t prescallerAPB = (cfgrValue & bitMask) >> startBit;
    if (prescallerAPB >= APBDEVIDEREN){
      prescallerAPB &=~APBDEVIDEREN;
      return (clockAHB >> (prescallerAPB + 1)) * mulTim;
    } else{
      return clockAHB;
    }
  }

  template<uint32_t value, uint32_t srcValue>
  static constexpr uint32_t pllOnValueCR = value > srcValue ? RCC_CR_PLLON : 0U;

  template<uint32_t value>
  static constexpr uint32_t cfgrvValuePreUSB = value == 72000000 ? 0 : RCC_CFGR_USBPRE;

  static void SwitchToHSI(){
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    RCC->CFGR = 0;
    RCC->CIR = RCCIRQFLAGSRESETVALUE;
    RCC->CR = RCC_CR_HSION  | (RCC->CR & (RCC_CR_HSITRIM | RCC_CR_HSICAL));
  }
  
  static bool EnableHSE(){
    RCC->CR |= RCC_CR_HSEON;
    uint32_t i = 0;
    for (; !(RCC->CR & RCC_CR_HSERDY) && i < HSEONTIMEOUT; ++i);
    return (i <= HSEONTIMEOUT);
  }

};

} // !namespace stm32f1::hardware

#endif //!_STM32F1_CLOCK_HPP