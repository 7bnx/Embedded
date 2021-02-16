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
#include "../Common/Compiler/Compiler.h"
#include "../Interfaces/IPower.hpp"
#include "../Pin/stm32f1_Pin.hpp"
#include "../Pinlist/stm32f1_Pinlist.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

namespace configuration::clock{

/*!
  @brief Configuration for MCO(Microcontroller clock output)
*/ 
enum class mco{

  /*! @brief No clock output at pin*/ 
  NO_CLOCK = 0UL,

  /*! @brief System clock (SYSCLK) selected*/ 
  SYS_CLOCK = 0x4000000UL,

  /*! @brief HSI clock selected.*/ 
  HSI_CLOCK = 0x5000000UL,

  /*! @brief HSE clock selected*/ 
  HSE_CLOCK = 0x6000000UL,

  /*! @brief PLL clock divided by 2 selected*/ 
  PLL_DIV2_CLOCK = 0x7000000UL
};

enum class source{
  HSI = 0,
  HSE = 1
};

} // !namespace configuration::clock

/*!
  @brief Clock Driver for STM32F1 series
*/ 
class Clock: private hardware::Registers{

public:

  /*!
    @brief Set controller clock
    ADC-clock is set to value/2.
    USB-clock is set to 48MHz in case of value = 72MHz, otherwise to value.
    AHB-clock is set to value.
    APB1-clock is set to 36MHz in case of value\2 >= 36MHz, othervise to value.
    APB2-clock is set to value.
    @tparam <value> new value for system clock (in Hz)
    @tparam <source> if true, then controller is driven by external source(HSE), otherwise - by internal(HSI). Default value is HSE
    @tparam <sourceValue> HSE value (in Hz). Skip it in case of HSI
  */
  template<uint32_t value = 72000000, 
           configuration::clock::source source = configuration::clock::source::HSE, 
           uint32_t sourceValue = 8000000>
  static bool Set(){
    
    using namespace configuration::clock;
    uint32_t constexpr valueSource = source == source::HSI ? 8000000 : sourceValue;
    uint32_t constexpr valueCFGR = _GetValueCFGR<value, source, valueSource>();
    uint32_t constexpr valueFlagSource = source == source::HSE ? valueOnHSE : valueOnHSI;
    uint32_t constexpr valueFlagReady = source == source::HSE ? valueFlagReadyHSE : valueFlagReadyHSI;
    uint32_t constexpr valueFlagClearSource = source == source::HSE ? valueOnHSI : valueOnHSE;

    Registers::_Set<addressCR, valueFlagSource>();
    if (!_IsValueSet<addressCR, valueFlagReady>()) return false;
    Registers::_Set<addressCFGR, (uint32_t)source, valueSW_Mask>();
    if (!_IsValueSet<addressCFGR, (uint32_t)source << 2, valueMaskSWS>()) return false;
    Registers::_Clear<addressCR, valueOnPLL | valueFlagClearSource>();
    
    Registers::_Write<addressACR, valueOnPrefetchBuffer | valueLatency<value>>();
    Registers::_Write<addressCFGR, valueCFGR>();
    if constexpr (value > valueSource){
      Registers::_Set<addressCR, valueOnPLL>();
      if (!_IsValueSet<addressCR, valueFlagReadyPLL>()) return false;
      Registers::_Write<addressCFGR, valueCFGR | valueSW_PLL>();
      if (!_IsValueSet<addressCFGR, valueSWS_PLL, valueMaskSWS>()) return false;
    }

    valueSystem = value;
    valueAHB = valueSystem; // !This value is correct, if RCC_CFGR_HPRE is set to default (SYSCLK not divided)
    valueAPB1 = _GetValueAPB<valueCFGR, valueMaskPrescalerAPB1, startBitAPB1, 1>();
    valueAPB1TIM = _GetValueAPB<valueCFGR, valueMaskPrescalerAPB1, startBitAPB1, 2>();
    valueAPB2 = valueAHB;
    valueAPB2TIM = valueAHB;
    valueADC = valueAHB / valuePrescalerADC;
    valueUSB = valuePrescalerUSB<value> ? valueAHB : (valueAHB * 2) / 3;
   
    return true;
  }

  /*!
    @brief Enable or disable MCO
    @tparam <source> of MCO. Set to mco::NO_CLOCK to disable MCO
  */
  template<configuration::clock::mco source = configuration::clock::mco::NO_CLOCK>
  __FORCE_INLINE static void MCO(){
    Registers::_Set<addressCFGR, (uint32_t)source, valueMaskMCO>();
  }

  /*!
    @brief Returns value of System-clock(SYSCLCK) in Hz
  */
  __FORCE_INLINE static uint32_t Get(){return valueSystem;}

protected:

  /*!
    @brief Returns value of AHB in Hz
  */
  __FORCE_INLINE static uint32_t AHB(){return valueAHB;}

  /*!
    @brief Returns value of APB1 in Hz
  */
  __FORCE_INLINE static uint32_t APB1(){return valueAPB1;}

  /*!
    @brief Returns value of APB2 in Hz
  */
  __FORCE_INLINE static uint32_t APB2(){return valueAPB2;}

  /*!
    @brief Returns value of ADC in Hz
  */
  __FORCE_INLINE static uint32_t ADC(){return valueADC;}

  /*!
    @brief Returns value of USB in Hz
  */
  __FORCE_INLINE static uint32_t USB(){return valueUSB;}

  /*!
    @brief Returns value for APB1 Timers in Hz
  */
  __FORCE_INLINE static uint32_t APB1TIM(){return valueAPB1TIM;}

  /*!
    @brief Returns value for APB2 Timers in Hz
  */
  __FORCE_INLINE static uint32_t APB2TIM(){return valueAPB2TIM;}

private:

  Clock() = delete;

  template<typename>
  friend class interfaces::IPower;
  
  template<typename...>
  friend class helper::pinlist::Helper;

  static constexpr uint32_t addressBase = 0x40021000;
  static constexpr uint32_t addressCR = addressBase;
  static constexpr uint32_t addressCFGR = addressBase + 4;
  static constexpr uint32_t addressCIR = addressBase + 8;
  static constexpr uint32_t addressFlashBase = 0x40022000;
  static constexpr uint32_t addressACR = addressFlashBase;

  static inline uint32_t valueSystem = 0;
  static inline uint32_t valueAHB = 0;
  static inline uint32_t valueAPB1 = 0;
  static inline uint32_t valueAPB2 = 0;
  static inline uint32_t valueUSB = 0;
  static inline uint32_t valueADC = 0;
  static inline uint32_t valueAPB1TIM = 0;
  static inline uint32_t valueAPB2TIM = 0;

  static constexpr uint32_t startBitPLLMUL = 18;
  static constexpr uint32_t valueMax = 72000000;

  static constexpr uint8_t valuePrescaler2APB = 4;
  static constexpr uint8_t startBitAPB1 = 8;

  static constexpr uint32_t valuePrescalerADC = 2;
  static constexpr size_t valueTimeout = 500;

  static constexpr uint32_t valueMaskMCO = 0xF000000;

  static constexpr uint32_t valueLatency_0 = 0;
  static constexpr uint32_t valueLatency_1 = 1;
  static constexpr uint32_t valueLatency_2 = 2;

  static constexpr uint32_t valuePrescalerHCKL_2 = 0x400;

  static constexpr uint32_t valueOnHSI = 1;
  static constexpr uint32_t valueOnHSE = 0x10000;
  static constexpr uint32_t valueOnPLL = 0x1000000;

  static constexpr uint32_t valuePrescalerPLLFromHSE_2 = 0x20000;

  static constexpr uint32_t valueSourcePLLFromHSE = 0x10000;

  static constexpr uint32_t valuePrescallerUSB = 0x400000;

  static constexpr uint32_t valueSW_PLL = 2;
  static constexpr uint32_t valueSW_Mask = 3;

  static constexpr uint32_t valueFlagReadyHSI = 2;
  static constexpr uint32_t valueFlagReadyHSE = 0x20000;
  static constexpr uint32_t valueFlagReadyPLL = 0x2000000;

  static constexpr uint32_t valueOnPrefetchBuffer = 0x2000000;

  static constexpr uint32_t valueMaskHSITRIM = 0xF8;
  static constexpr uint32_t valueMaskHSICAL = 0xFF00;

    //(System Clock Switch Status)
  static constexpr uint32_t valueSWS_HSI = 0;
  static constexpr uint32_t valueSWS_HSE = 4;
  static constexpr uint32_t valueSWS_PLL = 8;
  static constexpr uint32_t valueMaskSWS = 12;

  static constexpr uint32_t valueMaskPrescalerAPB1 = 0x700;

  template<uint32_t value>
  static constexpr uint32_t valueLatency = value >= 48000000 ? valueLatency_2 :
                                           value >= 24000000 ? valueLatency_1 :
                                                               valueLatency_0;

  template<uint32_t value, configuration::clock::source source, uint32_t srcValue>
  static constexpr uint32_t _GetMul(){
    using namespace configuration::clock;
    uint32_t mul = value / srcValue;
    uint32_t div = value % srcValue;
    if (source == source::HSI || (source == source::HSE && div)) mul*=2;
    if (div) mul++;
    return mul;
  }

  template<uint32_t value, configuration::clock::source source, uint32_t valueSource>
  static constexpr uint32_t _GetValueCFGR(){
    using namespace configuration::clock;
    uint32_t valueCFGR = 0;
    if (value >= (valueMax/2)) valueCFGR |= valuePrescalerHCKL_2;
    if (value > valueSource){
      if (source == source::HSI || (source == source::HSE && (value % valueSource))) 
        valueCFGR |= valuePrescalerPLLFromHSE_2;

      valueCFGR |= (_GetMul<value, source, valueSource>()-2) << startBitPLLMUL;

      if (source == source::HSE) valueCFGR |= valueSourcePLLFromHSE;
    } else valueCFGR |= (uint32_t)source;
    return valueCFGR | valuePrescalerUSB<value>;
  }

  template<uint32_t valueCFGR, uint32_t bitMask, uint32_t startBit, uint32_t mulTim>
  static constexpr uint32_t _GetValueAPB(){
    uint32_t prescallerAPB = (valueCFGR & bitMask) >> startBit;
    if (prescallerAPB >= valuePrescaler2APB){
      prescallerAPB &=~ valuePrescaler2APB;
      return (valueAHB >> (prescallerAPB + 1)) * mulTim;
    } else{
      return valueAHB;
    }
  }

  template<uint32_t address, uint32_t value, uint32_t mask = value>
  static bool _IsValueSet(){
    for(size_t i = 0; i < valueTimeout; ++i){
      if (Registers::_Read<address, mask>() == value)
        return true;
    }
    return false;
  }

  template<uint32_t value>
  static constexpr uint32_t valuePrescalerUSB =  value == 72000000 ? 0 : valuePrescallerUSB;

  using pin = Pin::PA_8::mode::set<configuration::pin::Alternative_PushPull>;

  struct initialization{
    using power = typename controller::Power::fromPeripherals<pin>::power;
    using pins = trait::Typelist<pin>;
    using interrupts = trait::Valuelist<>;
  };

};

} // !namespace controller

#endif //!_STM32F1_CLOCK_HPP