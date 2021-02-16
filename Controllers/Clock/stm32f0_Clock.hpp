//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Hardware-dependent driver for Clock Control. 
//                STM32F0xx series. 
//  TODO:         
//----------------------------------------------------------------------------------


#ifndef _STM32F0_CLOCK_HPP
#define _STM32F0_CLOCK_HPP

#include <cstdint>
#include "../Common/Compiler/Compiler.h"
#include "../Interfaces/IPower.hpp"
#include "../Pin/stm32f0_Pin.hpp"
//#include "../Pinlist/stm32f0_Pinlist.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

namespace configuration::clock{

/*!
  @brief Configuration for MCO(Microcontroller clock output)
*/ 
enum class mco : uint32_t{

  /*! @brief No clock output at pin*/ 
  MCO_DISABLED = 0UL,

  /*! @brief Internal RC 14 MHz (HSI14) oscillator clock selected*/ 
  MCO_HSI14 = 0x1000000,

  /*! @brief Internal low speed (LSI) oscillator clock selected*/ 
  MCO_LSI = 0x2000000,

  /*! @brief External low speed (LSE) oscillator clock selected*/ 
  MCO_LSE = 0x3000000,

  /*! @brief System clock selected*/ 
  MCO_SYSTEM = 0x4000000,

  /*! @brief Internal RC 8 MHz (HSI) oscillator clock selected*/ 
  MCO_HSI = 0x5000000,

  /*! @brief External 4-32 MHz (HSE) oscillator clock selected*/ 
  MCO_HSE = 0x6000000,

  /*! @brief PLL clock selected*/ 
  MCO_PLL = 0x7000000,

  /*! @brief PLL clock selected with divider by 2*/ 
  MCO_PLL_DIV_2 = 0x87000000
};

/*!
  @brief Source of controller clock
*/ 
enum class source{

  /*! @brief Internal RC 8 MHz (HSI) oscillator clock selected*/ 
  HSI = 0,

  /*! @brief External 4-32 MHz (HSE) oscillator clock selected*/ 
  HSE = 1
};

} // !namespace configuration::clock

/*!
  @brief Clock Driver for STM32F1 series
*/ 
class Clock: private hardware::Registers{

public:

  /*!
    @brief Set controller clock.
    AHB, APB, TIM, PCLK, HCKL are set to System-clock value
    @tparam <value> new value for system clock (in Hz)
    @tparam <source> if true, then controller is driven by external source(HSE), otherwise - by internal(HSI). Default value is HSE
    @tparam <sourceValue> HSE value (in Hz). Skip it in case of HSI
  */
  template<uint32_t value = 48000000, 
           configuration::clock::source source = configuration::clock::source::HSE, 
           uint32_t sourceValue = 8000000>
  static bool Set(){
    
    using namespace configuration::clock;
    uint32_t constexpr valueSource = source == source::HSI ? 8000000 : sourceValue;
    uint32_t constexpr valueCFGR = _GetValueCFGR<value, source, valueSource>();
    uint32_t constexpr valueFlagSource = source == source::HSE ? mask::CR::HSEON : mask::CR::HSION;
    uint32_t constexpr valueFlagReady = source == source::HSE ? mask::CR::HSERDY : mask::CR::HSIRDY;
    uint32_t constexpr valueFlagClearSource = source == source::HSE ? mask::CR::HSION : mask::CR::HSERDY;

      //Turn ON selected source
    Registers::_Set<address::RCC_CR, valueFlagSource>();
    if (!_IsValueSet<address::RCC_CR, valueFlagReady>()) return false;

      //Set selected source as System source
    Registers::_Set<address::RCC_CFGR, (uint32_t)source, mask::CFGR::SW>();
    if (!_IsValueSet<address::RCC_CFGR, (uint32_t)source << 2, mask::CFGR::SWS>()) return false;

      //Turn off current source and PLL
    Registers::_Clear<address::RCC_CR, mask::CR::PLLON | valueFlagClearSource>();
    
    Registers::_Write<address::FLASH_ACR, mask::FLASH::ACR::PRFTBE | mask::FLASH::ACR::LATENCY_SELECTED<value>>();
    Registers::_Write<address::RCC_CFGR, valueCFGR>();
    if constexpr (value > valueSource){
      Registers::_Set<address::RCC_CR, mask::CR::PLLON>();
      if (!_IsValueSet<address::RCC_CR, mask::CR::PLLRDY>()) return false;
      Registers::_Write<address::RCC_CFGR, valueCFGR | mask::CFGR::SW_PLL>();
      if (!_IsValueSet<address::RCC_CFGR, mask::CFGR::SWS_PLL, mask::CFGR::SWS>()) return false;
    }

    clockSystem = value;
    return true;
  }

  /*!
    @brief Enable or disable MCO
    @tparam <source> of MCO. Set to mco::NO_CLOCK to disable MCO
  */
  template<configuration::clock::mco source = configuration::clock::mco::MCO_DISABLED>
  __FORCE_INLINE static void MCO(){
    using namespace configuration::clock;
    static constexpr uint32_t mask = mask::CFGR::PLLNODIV | mask::CFGR::MCO;
    static constexpr uint32_t value = (uint32_t)source;

    Registers::_Set<address::RCC_CFGR, value, mask>();
  }

  /*!
    @brief Returns value of System-clock(SYSCLCK) in Hz
  */
  __FORCE_INLINE static uint32_t Get(){ return clockSystem; }

private:

  Clock() = delete;

  template<typename>
  friend class interfaces::IPower;
  
  template<typename...>
  friend class helper::pinlist::Helper;

  static inline uint32_t clockSystem = 0;

  struct constant{
    static constexpr uint32_t 
      timeout = 500,
      clockMax = 48000000;
  };

  struct address{
    static constexpr uint32_t 
      base_RCC = 0x40021000,
      base_Flash = 0x40022000,
      RCC_CR = base_RCC,
      RCC_CFGR = base_RCC + 4,
      RCC_CIR = base_RCC + 8,
      FLASH_ACR = base_Flash;
  };

  struct mask{
    struct CR{
      static constexpr uint32_t 
        HSION = 1,
        HSIRDY = 2,
        HSEON = 0x10000,
        HSERDY = 0x20000,
        PLLON = 0x1000000,
        PLLRDY = 0x2000000;
    };
    struct CFGR{
      static constexpr uint32_t 
        SW = 3,
        SW_PLL = 2,
        SWS = 12,
        SWS_PLL = 8,
        PLLSRC = 0x10000,
        MCO = 0xF << 24,
        PLLNODIV = 1 << 31;
    };
    struct FLASH{
      struct ACR{
        static constexpr uint32_t 
          LATENCY = 7,
          PRFTBE = 16;
        template<uint32_t value>
        static constexpr uint32_t LATENCY_SELECTED = value > 24000000 ? 1 : 0;
      };
    };
  };

  struct position{
    struct CFGR{
      static constexpr uint32_t PLLMULL = 18;
    };
  };

  template<uint32_t value, configuration::clock::source source, uint32_t valueSource>
  static constexpr uint32_t _GetValueCFGR(){
    using namespace configuration::clock;
    if (value > valueSource){
      uint32_t valueCFGR = 0;
      uint32_t mul = value / valueSource;
      if (source == source::HSE) valueCFGR |= mask::CFGR::PLLSRC;
      else mul *= 2;
      valueCFGR |= (mul - 2) << position::CFGR::PLLMULL;
      return valueCFGR;
    }
     else return (uint32_t)source;
  }

  template<uint32_t address, uint32_t value, uint32_t mask = value>
  static bool _IsValueSet(){
    for(size_t i = 0; i < constant::timeout; ++i){
      if (Registers::_Read<address, mask>() == value)
        return true;
    }
    return false;
  }

  using pin = Pin::PA_8::mode::set<configuration::pin::Alternative_Output_High>;

  struct initialization{
    using power = typename controller::Power::fromPeripherals<pin>::power;
    using pins = trait::Typelist<pin>;
    using interrupts = trait::Valuelist<>;
  };

};

} // !namespace controller

#endif //!_STM32F0_CLOCK_HPP