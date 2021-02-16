//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Driver for Real Time Clock. STM32F1-series
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _STM32F1_RTC_HPP
#define _STM32F1_RTC_HPP

#include <cstddef>
#include "../Common/Compiler/Compiler.h"
#include "../Common/Core/Registers.hpp"
#include "../Common/Core/Interrupt.hpp"
#include "../Power/stm32f1_Power.hpp"
#include "../Interfaces/IRTC.hpp"
#include "../../Utils/type_traits_custom.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief Configuration for RTC
*/
namespace configuration::rtc{

/*!
  @brief Source of RTC
*/ 
enum class source{

  /*! @brief External OSC with 32768 Hz*/
  SOURCE_EXTERNAL = 0x100,
  
  /*! @brief Internal OSC with 40000 Hz*/
  SOURCE_INTERNAL = 0x200
};

/*!
  @brief Interrupts
*/ 
enum class isr{

  /*! @brief Without interrupts. Use Handler for callbacks*/
  ISR_DISABLE = 0,

  /*! @brief Only Alarm interrupt. For second callback use Handler*/
  ISR_ALARM = 6,

  /*! @brief Only Second interrupt. For alarm callback use Handler*/
  ISR_SECOND = 5,

  /*! @brief Both Alarm and Second interrupts*/
  ISR_ALARM_SECOND = 7
};

} // !namespace configuration::rtc

template<configuration::rtc::source source = configuration::rtc::source::SOURCE_EXTERNAL,
         configuration::rtc::isr isr = configuration::rtc::isr::ISR_DISABLE>
class RTC: protected hardware::Registers, public interfaces::IRTC<RTC<source, isr>>{

  using base = interfaces::IRTC<RTC<source, isr>>;

  friend base;

  static constexpr bool isInterrupt = isr != configuration::rtc::isr::ISR_DISABLE;

  static inline size_t 
    counter = 0,
    counterPrev = 0,
    alarm = 0xFFFFFFFF;

  __FORCE_INLINE static bool _Init(const utils::TimeDate& timedate = base::timeDateDefault){
    using namespace configuration::rtc;

    Registers::_Set<address::PWR_CR, mask::PWR_CR::DBP>();
    bool isEnabled = false;
    if (!_IsEnabled()){
      static constexpr uint32_t 
        valueBDCR = mask::BDCR::RTCEN | (uint32_t)source | (source == source::SOURCE_EXTERNAL ? mask::BDCR::LSEON : 0);

      Registers::_Write<address::BDCR, mask::BDCR::BDRST>();
      Registers::_Write<address::BDCR, valueBDCR>();
        
      if constexpr (source == source::SOURCE_INTERNAL)
        Registers::_Write<address::CSR, mask::CSR::LSION>();

      base::timedate = timedate;

      _WaitSourceReady();

      _CalculateCounter();

      _UpdateRegisters<address::PRLL, address::PRLH, address::CNTL,    address::CNTH, address::ALRL,  address::ALRH>
                      (value::PRLL,   value::PRLH,   counter & 0xFFFF, counter >> 16, alarm & 0xFFFF, alarm >> 16);
    } else{
      while(!Registers::_Read<address::CRL, mask::CRL::RSF>());
      Registers::_Clear<address::CRL, mask::CRL::RSF>();
      _CalculateTimeDate();
      isEnabled = true;
    }
    if constexpr (isInterrupt) Registers::_Write<address::CRH, (uint16_t)isr>();
    return isEnabled;
  }

  __FORCE_INLINE static bool _Set(const utils::TimeDate& timedate){
    _CalculateTimeDate();
    if(base::timedate == timedate) return false;
    base::timedate = timedate;

    _CalculateCounter();
    if (counter > alarm){
      alarm = 0xFFFFFFFF;
      base::timedateAlarm = utils::TimeDate();
      _UpdateRegisters<address::CNTL,    address::CNTH, address::ALRL,  address::ALRH>
                      (counter & 0xFFFF, counter >> 16, alarm & 0xFFFF, alarm >> 16);
    } else{
      _UpdateRegisters<address::CNTL,    address::CNTH>
                      (counter & 0xFFFF, counter >> 16);
    }
    
    if (base::CallbackSet) base::CallbackSet();

    return true;
  }

  __FORCE_INLINE static bool _Set(const utils::Time& time){ return _Set(utils::TimeDate{ base::timedate.date, time }); }

  __FORCE_INLINE static bool _Set(const utils::Date& date){ return _Set(utils::TimeDate{ date, base::timedate.time }); }

  __FORCE_INLINE static bool _SetAlarm(const utils::TimeDate& timedate){
    _CalculateTimeDate();
    if(base::timedate > timedate || base::timedateAlarm == timedate) return false;
    base::timedateAlarm = timedate;
    alarm = timedate.DifferenceInSec(base::timeDateDefault);
    _UpdateRegisters<address::ALRL,    address::ALRH>
                    (alarm & 0xFFFF, alarm >> 16);
    return true;
  }

  __FORCE_INLINE static bool _SetAlarm(const utils::Time& time){ return _SetAlarm(utils::TimeDate{ base::timedateAlarm.date, time }); }

  __FORCE_INLINE static bool _SetAlarm(const utils::Date& date){ return _SetAlarm(utils::TimeDate{ date, base::timedateAlarm.time }); }

  __FORCE_INLINE static const utils::TimeDate& _Get(){ _CalculateTimeDate(); return base::timedate; }

  __FORCE_INLINE static const utils::TimeDate& _GetAlarm(){ return base::timedateAlarm; }

  __FORCE_INLINE static bool _IsEnabled(){ return Registers::_Read<address::BDCR, mask::BDCR::RTCEN>(); }

  __FORCE_INLINE static void _ISR(){ if constexpr (isInterrupt) _Handler(); }

  __FORCE_INLINE static void _Handler(){
    using namespace configuration::rtc;
    uint32_t flags = Registers::_Read<address::CRL>();
    Registers::_Clear<address::CRL>(flags);

    if (flags & mask::CRL::OWF){}

    if constexpr (isr == isr::ISR_ALARM || isr == isr::ISR_ALARM_SECOND || isr == isr::ISR_DISABLE)
      if (flags & mask::CRL::ALRF && base::CallbackAlarm) base::CallbackAlarm();

    if constexpr (isr == isr::ISR_SECOND || isr == isr::ISR_ALARM_SECOND || isr == isr::ISR_DISABLE)
      if (flags & mask::CRL::SECF && base::CallbackPeriod) base::CallbackPeriod();
  }

  __FORCE_INLINE static void _WaitSourceReady(){
    if constexpr (source == configuration::rtc::source::SOURCE_EXTERNAL)
      while(!Registers::_Read<address::BDCR, mask::BDCR::LSERDY>());
    else
      while(!Registers::_Read<address::CSR, mask::CSR::LSIRDY>());
  }

  static void _CalculateTimeDate(){
    counter = Registers::_Read<address::CNTH, uint16_t>() << 16U | Registers::_Read<address::CNTL, uint16_t>();
    if (counter != counterPrev){
      size_t addValue = counter - counterPrev;
      base::timedate.AddSec(addValue);
      counterPrev = counter;
    }
  }

  static void _CalculateCounter(){ counterPrev = counter = base::timedate.DifferenceInSec(base::timeDateDefault); }

  template<uint32_t... address, typename ... Types>
  static void _UpdateRegisters(Types ... value){
    _EnableUpdate();
    Registers::_Write<address...>(value...);
    _DisableUpdate();
  }

  __FORCE_INLINE static void _EnableUpdate(){
    while (!Registers::_Read<address::CRL, mask::CRL::RTOFF, uint16_t>());
    Registers::_Set<address::CRL, mask::CRL::CNF>();
  }

  __FORCE_INLINE static void _DisableUpdate(){
    Registers::_Clear<address::CRL, mask::CRL::CNF>();
    while (!Registers::_Read<address::CRL, mask::CRL::RTOFF, uint16_t>());
  }

  template<typename>
  friend class controller::interfaces::IPower;

  friend controller::Interrupt;

  struct address{
    static constexpr uint32_t base = 0x40002800,
      CRH  = base,
      CRL  = base + 4,
      PRLH = base + 8,
      PRLL = base + 12,
      DIVH = base + 16,
      DIVL = base + 20,
      CNTH = base + 24,
      CNTL = base + 28,
      ALRH = base + 32,
      ALRL = base + 36,
      BDCR = 0x40021020,
      CSR  = 0x40021024,
      PWR_CR  = 0x40007000;
  };

  struct mask{
    static constexpr uint32_t 
      PRL = 0xFFFF;
    struct APB1{
      static constexpr uint32_t 
        BKPEN = 1 << 27,
        PWREN = 1 << 28;
    };
    struct CRH{
      static constexpr uint32_t
        SECIE = 1, // Second interrupt enable
        ALRIE = 2, // Alarm interrupt enable
        OWIE = 4; // Overflow interrupt enable
    };
    struct CRL{
      static constexpr uint32_t
        SECF = 1, // Second interrupt flag
        ALRF = 2, // Alarm interrupt flag
        OWF = 4, // Overflow interrupt flag
        RSF  = 8, // Registers synchronized flag
        CNF = 16, // Configuration flag
        RTOFF = 32; // RTC operation OFF
    };
    struct CSR{
      static constexpr uint32_t
        LSION = 1, // Internal Low Speed oscillator enable
        LSIRDY = 2; // Internal Low Speed oscillator ready
    };
    struct BDCR{
      static constexpr uint32_t
        LSEON = 1, // External Low Speed oscillator enable
        LSERDY = 2, // External Low Speed oscillator ready
        RTCEN = 0x8000, // RTC clock enable
        BDRST = 0x10000; // Backup domain software reset
    };
    struct PWR_CR{
      static constexpr uint32_t
        DBP = 1 << 8;
    };
  };

  struct value{
    static constexpr uint32_t
      lsi = 40000,
      lse = 32768,
      prescaller = (source == configuration::rtc::source::SOURCE_EXTERNAL ? lse : lsi) - 1,
      PRLL = prescaller & mask::PRL,
      PRLH = (prescaller >> 16 ) & mask::PRL;
  };

  static constexpr uint32_t irq = 3;

  using power = typename Power::fromValues< 0U, mask::APB1::BKPEN | mask::APB1::PWREN, 0U >::initialization::power;
  using pins = trait::Typelist<>;
  using interrupts = std::conditional_t<isInterrupt, trait::Valuelist<irq>, trait::Valuelist<>>;

};

} //! namespace controller

#endif // !_STM32F1_RTC_HPP