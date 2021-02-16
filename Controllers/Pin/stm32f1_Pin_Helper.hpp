//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Helper class for STM32F1 Pin
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _STM32F1_PIN_HELPER_HPP
#define _STM32F1_PIN_HELPER_HPP

#include <cstdint>
#include "../Common/Compiler/Compiler.h"
#include "../../Utils/type_traits_custom.hpp"
#include "../Interfaces/IPin.hpp"
#include "../Power/stm32f1_Power.hpp"
#include "../External_Event/stm32f1_External_Event.hpp"
#include "../Common/Core/Interrupt.hpp"

/*!
  @brief Configuration for controllers devices
*/
namespace controller::configuration{

/*!
  @brief Configurations of Pin
*/ 
enum class pin {

  /*! @brief Float input*/ 
  Input_Float = 0x04,

  /*! @brief Digital input with internal pull-down*/ 
  Input_PD = 0x18,

  /*! @brief Digital input with internal pull-up*/ 
  Input_PU = 0x38,

  /*! @brief Analog input (Low Power)*/ 
  Input_Analog = 0x00,

  /*! @brief Open-drain output with 2MHz fronts*/ 
  Output_OpenDrain_2MHz = 0x06,

  /*! @brief Open-drain output with 10MHz fronts*/ 
  Output_OpenDrain_10MHz = 0x05,

  /*! @brief Open-drain output with 50MHz fronts*/ 
  Output_OpenDrain_50MHz = 0x07,

  /*! @brief Digital output with 2MHz fronts. Set to Low level*/ 
  Output_Low_2MHz = 0x12,

  /*! @brief Digital output with 10MHz fronts. Set to Low level*/ 
  Output_Low_10MHz = 0x11,

  /*! @brief Digital output with 50MHz fronts. Set to Low level*/ 
  Output_Low_50MHz = 0x13,

  /*! @brief Digital output with 2MHz fronts. Set to High level*/ 
  Output_High_2MHz = 0x32,

  /*! @brief Digital output with 10MHz fronts. Set to High level*/ 
  Output_High_10MHz = 0x31,

  /*! @brief Digital output with 50MHz fronts. Set to High level*/ 
  Output_High_50MHz = 0x33,

  /*! @brief Input event with Rising trigger*/ 
  Event_Rising = 0x144,

  /*! @brief Input event with Rising trigger and Pull Down*/ 
  Event_Rising_PD = 0x158,

  /*! @brief Input event with Falling trigger*/ 
  Event_Falling = 0x184,

  /*! @brief Input event with Falling trigger and Pull Up*/ 
  Event_Falling_PU = 0x1B8,

  /*! @brief Input event with Rising and Falling trigger*/ 
  Event_Rising_Falling = 0x1C4,

  /*! @brief Input interrupt with Rising trigger*/ 
  Interrupt_Rising = 0x244,

  /*! @brief Input interrupt with Rising trigger and Pull Down*/ 
  Interrupt_Rising_PD = 0x258,

  /*! @brief Input interrupt with Falling trigger*/ 
  Interrupt_Falling = 0x284,

  /*! @brief Input interrupt with Falling trigger and Pull Up*/ 
  Interrupt_Falling_PU = 0x2B8,

  /*! @brief Input interrupt with Rising and Falling trigger*/ 
  Interrupt_Rising_Falling = 0x2C4,

  /*! @brief Push-pull output for alternative function (USART...)*/ 
  Alternative_PushPull = 0x0B,

  /*! @brief Open-drain output for alternative function (I2C...)*/ 
  Alternative_OpenDrain = 0x0F,

  /*! @brief Default configuration*/ 
  Default = 0x04,

  /*! @brief Low Power configuration*/ 
  Low_Power = 0x00
};

} // ! namespace controller::configuration

/*!
  @brief Helper for pin
*/
namespace helper::pin{

/*!
  @brief Pin Driver for STM32F1 series. Don't use it Directly
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

  static constexpr uint32_t irq = number < 5  ? number + 6 : 
                                  number < 10 ? 23 : 40;

  using unique = trait::Valuelist<port, number>;

  struct address{
    static constexpr uint32_t 
      base = 0x40010800 + 0x400*port,
      base_EXTI = 0x40010400,
      base_AFIO = 0x40010000,
      IDR = base + 8,
      ODR = base + 12,
      BSRR = base + 16,
      BRR = base + 20,
      MAPR = base_AFIO,
      MAPR2 = base_AFIO + 4,
      EXTICR1 = base_AFIO + 8,
      EXTICR2 = base_AFIO + 12,
      EXTICR3 = base_AFIO + 16,
      EXTICR4 = base_AFIO + 20,
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
    static constexpr uint32_t
      CR = 0xF,
      ODR = 1,
      EXTICR = 0xF;
    struct configuration{
      static constexpr uint32_t
        IMR = 1 << 9,
        EMR = 1 << 8,
        FTSR = 1 << 7,
        RTSR = 1 << 6,
        REMAP_VALUE = 0xF,
        REMAP_REGISTER = 3,
        REMAP_POSITION = 0xFF00;
    };
  };

  struct position{
    struct configuration{
      static constexpr uint32_t
        CR = 0,
        PULL = 4,
        PULL_UP_DOWN = 5,
        REMAP_VALUE = 0,
        REMAP_REGISTER = 4,
        REMAP_POSITION = 8;
    };
  };

  static constexpr bool isInterrupt = uint32_t(configuration) & mask::configuration::IMR;

  static constexpr bool isAFIO = (uint32_t(configuration) & (mask::configuration::IMR | mask::configuration::EMR)) || remap;

  struct initialization{
   using power = typename controller::Power::fromValues<
      0U, 
      0U, 
      (1 << (2 + port)) | (isAFIO ? 1 : 0)
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

    static constexpr bool isAlternative = conf == controller::configuration::pin::Alternative_PushPull || 
                                          conf == controller::configuration::pin::Alternative_OpenDrain;

    template<uint32_t number>
    static constexpr uint32_t valueMAPR = 
      isAlternative && (((remap >> position::configuration::REMAP_REGISTER) & mask::configuration::REMAP_REGISTER) == number) ? 
      (remap & mask::configuration::REMAP_VALUE) << ((remap >> position::configuration::REMAP_POSITION) & mask::configuration::REMAP_POSITION) : 0;

    static constexpr uint32_t 
      valueMAPR1 = valueMAPR<1> | next::valueMAPR1,
      valueMAPR2 = valueMAPR<2> | next::valueMAPR2;

#define ADDRESSES_CONFIGURATION(ID, port)\
struct Port##ID{\
  static constexpr uint32_t\
    base = 0x40010800 + 0x400*port,\
    CRL = base,\
    CRH = base + 4,\
    ODR = base + 12;\
};

#define VALUES_CONFIGURATION(ID, portNumber)\
  static constexpr uint32_t\
    valueCRL_##ID = (port != portNumber || pin > 7 ? 0 :\
      ((((uint32_t)conf >> position::configuration::CR) & mask::CR) << (pin * 4))) | next::valueCRL_##ID,\
    valueCRH_##ID = (port != portNumber || pin < 8 ? 0 :\
      ((((uint32_t)conf >> position::configuration::CR) & mask::CR) << ((pin - 8) * 4))) | next::valueCRH_##ID,\
    valueODR_##ID = (port != portNumber  || !((uint32_t)config >> position::configuration::PULL & mask::ODR) ? 0 :\
      ((((uint32_t)conf >> position::configuration::PULL_UP_DOWN) & mask::ODR) << pin)) | next::valueODR_##ID;

#define MASKS_CONFIGURATION(ID, portNumber)\
  static constexpr uint32_t\
    maskCRL_##ID = (port != portNumber || pin > 7 ? 0 :\
      (mask::CR << (pin * 4))) | next::maskCRL_##ID,\
    maskCRH_##ID = (port != portNumber || pin < 8 ? 0 :\
      (mask::CR << ((pin - 8) * 4))) | next::maskCRH_##ID,\
    maskODR_##ID = (port != portNumber || !((uint32_t)conf >> position::configuration::PULL & mask::ODR) ? 0 :\
      (mask::ODR << pin)) | next::maskODR_##ID;

#define VALUES_MASKS_CONFIGURATION_DEFAULT(ID)\
  static constexpr uint32_t\
    valueCRL_##ID = 0, maskCRL_##ID = 0,\
    valueCRH_##ID = 0, maskCRH_##ID = 0,\
    valueODR_##ID = 0, maskODR_##ID = 0;

#define ADDRESSES_PORT(ID) Port##ID::CRL, Port##ID::CRH, Port##ID::ODR

#define VALUES_PORT(ID) valueCRL_##ID, valueCRH_##ID, valueODR_##ID

#define MASKS_PORT(ID) maskCRL_##ID, maskCRH_##ID, maskODR_##ID

    ADDRESSES_CONFIGURATION(A,0);
    ADDRESSES_CONFIGURATION(B,1);
    ADDRESSES_CONFIGURATION(C,2);
    ADDRESSES_CONFIGURATION(D,3);
    ADDRESSES_CONFIGURATION(E,4);
    ADDRESSES_CONFIGURATION(F,5);
    ADDRESSES_CONFIGURATION(G,6);

    VALUES_CONFIGURATION(A, 0);
    VALUES_CONFIGURATION(B, 1);
    VALUES_CONFIGURATION(C, 2);
    VALUES_CONFIGURATION(D, 3);
    VALUES_CONFIGURATION(E, 4);
    VALUES_CONFIGURATION(F, 5);
    VALUES_CONFIGURATION(G, 6);

    MASKS_CONFIGURATION(A, 0);
    MASKS_CONFIGURATION(B, 1);
    MASKS_CONFIGURATION(C, 2);
    MASKS_CONFIGURATION(D, 3);
    MASKS_CONFIGURATION(E, 4);
    MASKS_CONFIGURATION(F, 5);
    MASKS_CONFIGURATION(G, 6);

    using addresses = trait::Valuelist<address::MAPR, address::MAPR2,
                                       ADDRESSES_PORT(A), ADDRESSES_PORT(B), ADDRESSES_PORT(C),
                                       ADDRESSES_PORT(D), ADDRESSES_PORT(E), ADDRESSES_PORT(F), ADDRESSES_PORT(G),
                                       address::EXTI_IMR, address::EXTI_EMR, address::EXTI_RTSR, address::EXTI_FTSR,
                                       address::EXTICR1, address::EXTICR2, address::EXTICR3, address::EXTICR4>;

    using values = trait::Valuelist<valueMAPR1, valueMAPR2,
                                    VALUES_PORT(A), VALUES_PORT(B), VALUES_PORT(C),
                                    VALUES_PORT(D), VALUES_PORT(E), VALUES_PORT(F), VALUES_PORT(G),
                                    valueIMR, valueEMR, valueRTSR, valueFTSR,
                                    valueEXTICR1, valueEXTICR2, valueEXTICR3, valueEXTICR4>;

    using masks = trait::Valuelist<0U, 0U,
                                   MASKS_PORT(A), MASKS_PORT(B), MASKS_PORT(C),
                                   MASKS_PORT(D), MASKS_PORT(E), MASKS_PORT(F), MASKS_PORT(G),
                                   maskIMR, maskEMR, maskRTSR, maskFTSR,
                                   maskEXTICR1, maskEXTICR2, maskEXTICR3, maskEXTICR4>;

  };

  template<typename numberList, typename configList, typename remapList>
  struct _configuration<trait::Valuelist<>, numberList, configList, remapList>{

    static constexpr uint32_t 
      valueMAPR1 = 0,
      valueMAPR2 = 0;

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
    VALUES_MASKS_CONFIGURATION_DEFAULT(G);

  };

};

} // !namespace helper::pin

#endif // !_STM32F1_PIN_HELPER_HPP