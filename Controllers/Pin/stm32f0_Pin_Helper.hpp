//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Helper class for STM32F0 Pin
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _STM32F0_PIN_HELPER_HPP
#define _STM32F0_PIN_HELPER_HPP

#include <cstdint>
#include "../Common/Compiler/Compiler.h"
#include "../../Utils/type_traits_custom.hpp"
#include "../Interfaces/IPin.hpp"
#include "../Common/Core/Interrupt.hpp"
#include "../Power/stm32f0_Power.hpp"
#include "../External_Event/stm32f0_External_Event.hpp"

/*!
  @brief Configuration for controllers devices
*/
namespace controller::configuration{

/*!
  @brief Configurations of Pin
*/ 
enum class pin {

  /*! @brief Float input*/ 
  Input_Float = 0x00,

  /*! @brief Digital input with internal pull-up*/ 
  Input_PullUp = 0x01,

  /*! @brief Digital input with internal pull-down*/ 
  Input_PullDown = 0x02,

  /*! @brief Analog input (Low Power)*/ 
  Input_Analog = 0xC0,

  /*! @brief Input event with Rising trigger*/
  Event_Rising = 0x500,

  /*! @brief Input event with Rising trigger and pull down*/ 
  Event_Rising_PD = 0x502,

  /*! @brief Input event with Falling trigger*/ 
  Event_Falling = 0x600,

  /*! @brief Input event with Falling trigger and pull up*/
  Event_Falling_PU = 0x601,

  /*! @brief Input event with Rising and Falling trigger*/
  Event_Rising_Falling = 0x700,

    /*! @brief External interrupt with Rising trigger*/
  Interrupt_Rising = 0x900,

  /*! @brief External interrupt with Rising trigger and pull down*/ 
  Interrupt_Rising_PD = 0x902,

  /*! @brief External interrupt with Falling trigger*/ 
  Interrupt_Falling = 0xA00,

  /*! @brief External interrupt with Falling trigger and pull up*/
  Interrupt_Falling_PU = 0xA01,

  /*! @brief External interrupt with Rising and Falling trigger*/
  Interrupt_Rising_Falling = 0xB00,

  /*! @brief Open-drain output with Low slew rate*/ 
  Output_OpenDrain_Low = 0x50,

  /*! @brief Open-drain output with Medium slew rate*/ 
  Output_OpenDrain_Medium = 0x54,

  /*! @brief Open-drain output with High slew rate*/ 
  Output_OpenDrain_High = 0x5C,

  /*! @brief Open-drain output with pull-up and Low slew rate*/ 
  Output_OpenDrain_PU_Low = 0x51,

  /*! @brief Open-drain output with pull-up and Medium slew rate*/ 
  Output_OpenDrain_PU_Medium = 0x55,

  /*! @brief Open-drain output with pull-up and High slew rate*/ 
  Output_OpenDrain_PU_High = 0x5D,

  /*! @brief Digital output with Low slew rate and Low-state pin*/ 
  Output_Low_Low = 0x40,

  /*! @brief Digital output with Medium rate and Low-state pin*/ 
  Output_Low_Medium = 0x44,

  /*! @brief Digital output with High slew rate and Low-state pin*/ 
  Output_Low_High = 0x4C,

  /*! @brief Digital output with Low slew rate and High-state pin*/ 
  Output_High_Low = 0x1040,

  /*! @brief Digital output with Medium rate and High-state pin*/ 
  Output_High_Medium = 0x1044,

  /*! @brief Digital output with High slew rate and High-state pin*/ 
  Output_High_High = 0x104C,

  /*! @brief Push-pull output for alternative function with Low slew rate(USART...)*/ 
  Alternative_Output_Low = 0x80,

  /*! @brief Push-pull output for alternative function with Medium slew rate(USART...)*/ 
  Alternative_Output_Medium = 0x84,

  /*! @brief Push-pull output for alternative function with High slew rate(USART...)*/ 
  Alternative_Output_High= 0x8C,

  /*! @brief Open drain output for alternative function with Low slew rate(I2C...)*/ 
  Alternative_OpenDrain_Low = 0x90,

  /*! @brief Open drain output for alternative function with Medium slew rate(I2C...)*/ 
  Alternative_OpenDrain_Medium = 0x94,

  /*! @brief Open drain output for alternative function with High slew rate(I2C...)*/ 
  Alternative_OpenDrain_High= 0x9C,

  /*! @brief Open drain output for alternative function with Pull Up and Low slew rate(I2C...)*/ 
  Alternative_OpenDrain_PU_Low = 0x91,

  /*! @brief Open drain output for alternative function with Pull Up and Medium slew rate(I2C...)*/ 
  Alternative_OpenDrain_PU_Medium = 0x95,

  /*! @brief Open drain output for alternative function with Pull Up and High slew rate(I2C...)*/ 
  Alternative_OpenDrain_PU_High= 0x9D,

  /*! @brief Default configuration*/ 
  Default = 0x00,

  /*! @brief Low Power configuration*/ 
  Low_Power = 0xC0
};

} // ! namespace controller::configuration

/*!
  @brief Helper for pin
*/
namespace helper::pin{

/*!
  @brief Pin Helper for STM32F0 series. Don't use it Directly
  @tparam <Pin> selected pin
  @tparam <portID> number of port. E.g.: GPIOA = 0
  @tparam <numberPin> number of pin. From 0 to 15
  @tparam <config> pin configuration
  @tparam <remapValue> value to remap pin fo alternative function
*/ 
template<typename Pin, uint32_t portID, uint32_t numberPin, 
         controller::configuration::pin config, uint32_t remapValue>
class Helper{

  Helper() = delete;

  template<typename>
  friend class controller::interfaces::IPower;

  template<typename>
  friend class controller::ExternalEvent;

  template<typename, typename...>
  friend class helper::pinlist::Helper;

  friend controller::Interrupt;

protected:

  static constexpr uint32_t port = portID;
  static constexpr uint8_t number = numberPin;
  static constexpr auto configuration = config;
  static constexpr uint32_t remap = remapValue;

  using unique = trait::Valuelist<port, number>;

  struct address{
    static constexpr uint32_t 
      base = 0x48000000 + 0x400*port,
      base_EXTI = 0x40010400,
      base_SYSCFG = 0x40010000,
      IDR = base + 16,
      ODR = base + 20,
      BSRR = base + 24,
      LCKR = base + 28,
      BRR = base + 40,
      EXTICR1 = base_SYSCFG + 8,
      EXTICR2 = base_SYSCFG + 12,
      EXTICR3 = base_SYSCFG + 16,
      EXTICR4 = base_SYSCFG + 20,
      EXTI_IMR = base_EXTI,
      EXTI_EMR = base_EXTI + 4,
      EXTI_RTSR = base_EXTI + 8,
      EXTI_FTSR = base_EXTI + 12,
      EXTI_SWIER = base_EXTI + 16,
      EXTI_PR = base_EXTI + 20;
  };

  static constexpr uint32_t addressWrite = address::BSRR;
  static constexpr uint32_t addressRead = address::IDR;

  struct mask{
    static constexpr uint32_t pin = 1 << number;
    struct configuration{
      static constexpr uint32_t
        PUSH_PULL = 0xF,
        IMR = 0x800,
        EMR = 0x400,
        FTSR = 0x200,
        RTSR = 0x100;
    };
    static constexpr uint32_t
      MODER = 3,
      OTYPER = 1,
      OSPEEDR = 3,
      PUPDR = 3,
      ODR = 1,
      AFRL = 0xF,
      AFRH = 0xF,
      EXTICR = 0xF;
  };

  struct position{
    struct configuration{
      static constexpr uint32_t
        ODR = 12,
        IMR = 11,
        EMR = 10,
        FTSR = 9,
        RTSR = 8,
        MODER = 6,
        OTYPER = 4,
        OSPEEDR = 2,
        PUPDR = 0,
        PUSH_PULL = 4,
        POWER_ENABLE = 17;
    };
  };  

#define ADDRESSES_CONFIGURATION(ID, port)\
struct Port##ID{\
  static constexpr uint32_t\
    base = 0x48000000 + 0x400*port,\
    MODER = base,\
    OTYPER = base + 4,\
    OSPEEDR = base + 8,\
    PUPDR = base + 12,\
    ODR = base + 20,\
    AFRL = base + 32,\
    AFRH = base + 36;\
};

  static constexpr uint32_t irq = number < 2 ? 5 : 
                                  number < 4 ? 6 : 7;
  
  static constexpr bool isInterruptEvent = uint32_t(configuration) & mask::configuration::IMR ||
                                           uint32_t(configuration) & mask::configuration::EMR;

  static constexpr bool isInterrupt = uint32_t(configuration) & mask::configuration::IMR;

  struct initialization{
   using power = typename controller::Power::fromValues<
      1U << (port + position::configuration::POWER_ENABLE), 
      0U, 
      isInterruptEvent ? 1U : 0U
    >::initialization::power;
    using pins = trait::Typelist<Pin>;
    using interrupts = std::conditional_t<isInterrupt, trait::Valuelist<irq>, trait::Valuelist<>>;
  };

  template<typename portList = trait::Valuelist<port>, typename numberList = trait::Valuelist<number>, 
           typename configList = trait::Valuelist<configuration>, typename remapList = trait::Valuelist<remap>>
  struct _configuration{

    using nextPort = trait::pop_front_t<portList>;
    using nextNumberList = trait::pop_front_t<numberList>;
    using nextConfigList = trait::pop_front_t<configList>;
    using nextRemapList = trait::pop_front_t<remapList>;
    using next = _configuration<nextPort, nextNumberList, nextConfigList, nextRemapList>;

    static constexpr auto port = trait::front_v<portList>;
    static constexpr auto pin = trait::front_v<numberList>;
    static constexpr auto conf = trait::front_v<configList>;
    static constexpr auto remap = trait::front_v<remapList>;

    static constexpr uint32_t 
      valueIMR  = ((uint32_t(conf) & mask::configuration::IMR) ? 1 << pin : 0) | next::valueIMR,
      valueEMR  = ((uint32_t(conf) & mask::configuration::EMR) ? 1 << pin : 0) | next::valueEMR,
      valueRTSR = ((uint32_t(conf) & mask::configuration::RTSR) ? 1 << pin : 0) | next::valueRTSR,
      valueFTSR = ((uint32_t(conf) & mask::configuration::FTSR) ? 1 << pin : 0) | next::valueFTSR;
    
    static constexpr uint32_t 
      maskIMR  = (uint32_t(conf) & (mask::configuration::IMR | mask::configuration::EMR) ? 1 << pin : 0) | next::maskIMR,
      maskEMR  = maskIMR,
      maskRTSR = maskIMR,
      maskFTSR = maskIMR;

    template<uint32_t numberReg>
    static constexpr bool isEXTICR_Configuration = 
      (numberReg*4U - pin - 1) < 4 && (uint32_t(conf) & mask::configuration::IMR || uint32_t(conf) & mask::configuration::EMR);

    template<uint32_t numberReg>
    static constexpr uint32_t valueEXTICR = isEXTICR_Configuration<numberReg> ? port << (pin & 3)*4 : 0;

    template<uint32_t numberReg>
    static constexpr uint32_t maskEXTICR = isEXTICR_Configuration<numberReg> ? mask::EXTICR << (pin & 3)*4 : 0;

    static constexpr uint32_t 
      valueEXTICR1 = valueEXTICR<1> | next::valueEXTICR1,
      valueEXTICR2 = valueEXTICR<2> | next::valueEXTICR2,
      valueEXTICR3 = valueEXTICR<3> | next::valueEXTICR3,
      valueEXTICR4 = valueEXTICR<4> | next::valueEXTICR4;

    static constexpr uint32_t 
      maskEXTICR1 = maskEXTICR<1> | next::maskEXTICR1,
      maskEXTICR2 = maskEXTICR<2> | next::maskEXTICR2,
      maskEXTICR3 = maskEXTICR<3> | next::maskEXTICR3,
      maskEXTICR4 = maskEXTICR<4> | next::maskEXTICR4;

    static constexpr bool 
      isAlternative = (((uint32_t)conf >> position::configuration::MODER) & mask::MODER) == 2,
      isPushPull = (((uint32_t)conf >> position::configuration::PUSH_PULL) & mask::configuration::PUSH_PULL) == 4,
      isInput = 
        (((uint32_t)conf >> position::configuration::MODER) & mask::MODER) == 3 || 
        (((uint32_t)conf >> position::configuration::MODER) & mask::MODER) == 0;
       
#define VALUES_CONFIGURATION(ID, portNumber)\
  static constexpr uint32_t\
    valueMODER_##ID = (port != portNumber ? 0 :\
      ((((uint32_t)conf >> position::configuration::MODER) & mask::MODER) << (pin * 2))) | next::valueMODER_##ID,\
    valueOTYPER_##ID = (port != portNumber ? 0 :\
      ((((uint32_t)conf >> position::configuration::OTYPER) & mask::OTYPER) << pin)) | next::valueOTYPER_##ID,\
    valueOSPEEDR_##ID = (port != portNumber ? 0 :\
      ((((uint32_t)conf >> position::configuration::OSPEEDR) & mask::OSPEEDR) << (pin * 2))) | next::valueOSPEEDR_##ID,\
    valuePUPDR_##ID = (port != portNumber || isPushPull ? 0 :\
      ((((uint32_t)conf >> position::configuration::PUPDR) & mask::PUPDR) << (pin * 2))) | next::valuePUPDR_##ID,\
    valueODR_##ID = (port != portNumber || !isPushPull ? 0 :\
      ((((uint32_t)conf >> position::configuration::ODR) & mask::ODR) << pin)) | next::valueODR_##ID,\
    valueAFRL_##ID = (port != portNumber || pin > 7  || !isAlternative ? 0 :\
      ((remap & mask::AFRL) << (pin * 4))) | next::valueAFRL_##ID,\
    valueAFRH_##ID = (port != portNumber || pin < 8  || !isAlternative ? 0 :\
      ((remap & mask::AFRH) << ((pin - 7) * 4))) | next::valueAFRH_##ID;

#define MASKS_CONFIGURATION(ID, portNumber)\
  static constexpr uint32_t\
    maskMODER_##ID = (port != portNumber ? 0 :\
      (mask::MODER << (pin * 2))) | next::maskMODER_##ID,\
    maskOTYPER_##ID = (port != portNumber || isInput ? 0 :\
      (mask::OTYPER << pin)) | next::maskOTYPER_##ID,\
    maskOSPEEDR_##ID = (port != portNumber || isInput ? 0 :\
      (mask::OSPEEDR << (pin * 2))) | next::maskOSPEEDR_##ID,\
    maskPUPDR_##ID = (port != portNumber || isPushPull? 0:\
      (mask::PUPDR << (pin * 2))) | next::maskPUPDR_##ID,\
    maskODR_##ID = (port != portNumber || !isPushPull ? 0:\
      (mask::ODR << pin)) | next::maskODR_##ID,\
    maskAFRL_##ID = (port != portNumber || pin > 7 || !isAlternative ? 0:\
      (mask::AFRL << (pin * 4))) | next::maskAFRL_##ID,\
    maskAFRH_##ID = (port != portNumber || pin < 8 || !isAlternative ? 0:\
      (mask::AFRH << ((pin - 7) * 4))) | next::maskAFRH_##ID;

#define VALUES_MASKS_CONFIGURATION_DEFAULT(ID)\
  static constexpr uint32_t\
    valueMODER_##ID = 0, maskMODER_##ID = 0,\
    valueOTYPER_##ID = 0, maskOTYPER_##ID = 0,\
    valueOSPEEDR_##ID = 0, maskOSPEEDR_##ID = 0,\
    valuePUPDR_##ID = 0, maskPUPDR_##ID = 0,\
    valueODR_##ID = 0, maskODR_##ID = 0,\
    valueAFRL_##ID = 0, maskAFRL_##ID = 0,\
    valueAFRH_##ID = 0, maskAFRH_##ID = 0;

#define ADDRESSES_PORT(ID) Port##ID::AFRL, Port##ID::AFRH,\
                           Port##ID::OTYPER, Port##ID::OSPEEDR,\
                           Port##ID::PUPDR, Port##ID::MODER, Port##ID::ODR

#define VALUES_PORT(ID) valueAFRL_##ID, valueAFRH_##ID, valueOTYPER_##ID, valueOSPEEDR_##ID, valuePUPDR_##ID, valueMODER_##ID, valueODR_##ID

#define MASKS_PORT(ID) maskAFRL_##ID, maskAFRH_##ID, maskOTYPER_##ID, maskOSPEEDR_##ID, maskPUPDR_##ID, maskMODER_##ID, maskODR_##ID

    ADDRESSES_CONFIGURATION(A,0);
    ADDRESSES_CONFIGURATION(B,1);
    ADDRESSES_CONFIGURATION(C,2);
    ADDRESSES_CONFIGURATION(D,3);
    ADDRESSES_CONFIGURATION(E,4);
    ADDRESSES_CONFIGURATION(F,5);

    VALUES_CONFIGURATION(A, 0);
    VALUES_CONFIGURATION(B, 1);
    VALUES_CONFIGURATION(C, 2);
    VALUES_CONFIGURATION(D, 3);
    VALUES_CONFIGURATION(E, 4);
    VALUES_CONFIGURATION(F, 5);

    MASKS_CONFIGURATION(A, 0);
    MASKS_CONFIGURATION(B, 1);
    MASKS_CONFIGURATION(C, 2);
    MASKS_CONFIGURATION(D, 3);
    MASKS_CONFIGURATION(E, 4);
    MASKS_CONFIGURATION(F, 5);

    using addresses = trait::Valuelist<ADDRESSES_PORT(A), ADDRESSES_PORT(B), ADDRESSES_PORT(C),
                                       ADDRESSES_PORT(D), ADDRESSES_PORT(E), ADDRESSES_PORT(F),
                                       address::EXTI_IMR, address::EXTI_EMR, address::EXTI_RTSR, address::EXTI_FTSR,
                                       address::EXTICR1, address::EXTICR2, address::EXTICR3, address::EXTICR4>;

    using values = trait::Valuelist<VALUES_PORT(A), VALUES_PORT(B), VALUES_PORT(C),
                                    VALUES_PORT(D), VALUES_PORT(E), VALUES_PORT(F),
                                    valueIMR, valueEMR, valueRTSR, valueFTSR,
                                    valueEXTICR1, valueEXTICR2, valueEXTICR3, valueEXTICR4>;

    using masks = trait::Valuelist<MASKS_PORT(A), MASKS_PORT(B), MASKS_PORT(C),
                                   MASKS_PORT(D), MASKS_PORT(E), MASKS_PORT(F),
                                   maskIMR, maskEMR, maskRTSR, maskFTSR,
                                   maskEXTICR1, maskEXTICR2, maskEXTICR3, maskEXTICR4>;
  };

  template<typename numberList, typename configList, typename remapList>
  struct _configuration<trait::Valuelist<>, numberList, configList, remapList>{

    static constexpr uint32_t 
      valueIMR  = 0,
      valueEMR  = 0,
      valueRTSR = 0,
      valueFTSR = 0;

    static constexpr uint32_t 
      maskIMR  = 0,
      maskEMR  = 0,
      maskRTSR = 0,
      maskFTSR = 0;

    static constexpr uint32_t 
      valueEXTICR1 = 0,
      valueEXTICR2 = 0,
      valueEXTICR3 = 0,
      valueEXTICR4 = 0;

    static constexpr uint32_t 
      maskEXTICR1 = 0,
      maskEXTICR2 = 0,
      maskEXTICR3 = 0,
      maskEXTICR4 = 0;

    VALUES_MASKS_CONFIGURATION_DEFAULT(A);
    VALUES_MASKS_CONFIGURATION_DEFAULT(B);
    VALUES_MASKS_CONFIGURATION_DEFAULT(C);
    VALUES_MASKS_CONFIGURATION_DEFAULT(D);
    VALUES_MASKS_CONFIGURATION_DEFAULT(E);
    VALUES_MASKS_CONFIGURATION_DEFAULT(F);
  };

};

#undef ADDRESSES_CONFIGURATION
#undef VALUES_CONFIGURATION
#undef MASKS_CONFIGURATION
#undef ADDRESSES_PORT
#undef VALUES_PORT
#undef MASKS_PORT
#undef VALUES_MASKS_CONFIGURATION_DEFAULT

} // !namespace helper::pin

#endif // !_STM32F0_PIN_HELPER_HPP