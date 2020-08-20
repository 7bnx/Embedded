//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Hardware-dependent library for list of Pins 
//  TODO:          
//----------------------------------------------------------------------------------

#ifndef _STM32F1_PINLIST_HPP
#define _STM32F1_PINLIST_HPP

#include <cstdint>
#include "stm32f10x.h"
#include "pinlist_traits.hpp"


namespace stm32f1::hardware{

/*!
  @brief Operate with list of pin
  @tparam <typename... Pins> list of single pins or/and Pinlist.
*/ 
template<typename... Pins>
class _PinlistImplementation{
public:

  /*!
    @brief List of pins
  */ 
  using pins = utils::expand_t<typename Pins::pins...>;

  /*!
    @brief List of unique ports in list. E.g.: utils::Typelist<utils::ctv<GPIOA_BASE>, utils::ctv<GPIOC_BASE>>
  */
  using ports = pinlist_traits::make_port_list_t<pins>;
  
  /*!
    @brief For Power class usage
  */ 

  using power = pinlist_traits::make_power_list_t<pins>;
  /*!
    @brief Number of pins in list
  */ 
  static constexpr size_t size = utils::size_of_list_v<pins>;

  /*!
    @brief Initialize list of pins
  */ 
  static void Init(){
    if constexpr(sizeof...(Pins))
      Config<pins, ports>();
  }

  /*!
    @brief Reset pins to default state
  */ 
  static void Reset(){
    if constexpr (sizeof...(Pins) == 0){
      SetValueToPort<0U, GPIOA_BASE, GPIOB_BASE, GPIOC_BASE, GPIOD_BASE, 
                         GPIOE_BASE, GPIOE_BASE, GPIOF_BASE, GPIOG_BASE>();
      return;
    }
    using resetPins = pinlist_traits::set_config_to_pins_t<controller::pinConfiguration::Input_Analog, pins>;
    Config<resetPins, ports>();
  }

  /*!
    @brief Set pins in list to low power mode
  */ 
  static void ToLowPowerMode(){ Reset(); }

  /*!
    @brief Set all pins in list to High-level
  */ 
  static void High(){ SetOutput<true, pins, ports>(); }

  /*!
    @brief Set all pins in list to Low-level
  */
  static void Low(){ SetOutput<false, pins, ports>();}

  /*!
    @brief Write value to pins in list
  */
  static void Write(uint32_t value){ WriteHelper<ports>(value); }

  /*!
    @brief Read the state of pins in list
  */
  static uint32_t Read(){ return ReadHelper<ports>(); }

private:

  _PinlistImplementation() = delete;

  template<typename Ports>
  static void WriteHelper(uint32_t value){
    static constexpr auto base = utils::front_t<Ports>::value;
    static constexpr auto GPIOx  = []() constexpr { return (GPIO_TypeDef*)(base);};
    
    using pinlistOfPort = pinlist_traits::make_port_pinlist_t<base, pins>;
    using positionlistOfPort = pinlist_traits::make_port_positionlist_t<base, size - 1U, pins>;

    GPIOx()->BSRR = (portConfigurationMask<base, pins>::value << 16U) | newValue<
                                                                                true, 
                                                                                utils::front_v<positionlistOfPort>, 
                                                                                utils::front_t<pinlistOfPort>, 
                                                                                utils::Typelist<>, 
                                                                                utils::Valuelist<>, 
                                                                                positionlistOfPort, 
                                                                                pinlistOfPort
                                                                              >(value);

    using next = utils::pop_front_t<Ports>;
    if constexpr(!utils::is_empty_v<next>)
      WriteHelper<next>(value);
  }

  template<typename Ports>
  static uint32_t ReadHelper(){
    static constexpr auto base = utils::front_t<Ports>::value;
    static constexpr auto GPIOx  = []() constexpr { return (GPIO_TypeDef*)(base);};

    using pinlistOfPort = pinlist_traits::make_port_pinlist_t<base, pins>;
    using positionlistOfPort = pinlist_traits::make_port_positionlist_t<base, size - 1, pins>;

    uint32_t value = newValue<
                              false, 
                              utils::front_v<positionlistOfPort>, 
                              utils::front_t<pinlistOfPort>, 
                              utils::Typelist<>, 
                              utils::Valuelist<>, 
                              positionlistOfPort, 
                              pinlistOfPort
                            >(GPIOx()->IDR);
  
    using next = utils::pop_front_t<Ports>;
    if constexpr (!utils::is_empty_v<next>)
      return value | ReadHelper<next>();
    return value;
  }

  template<typename pins, typename ports>
  static void Config(){
    
    static constexpr auto base = utils::front_t<ports>::value;
    static constexpr auto GPIOx  = []() constexpr { return (GPIO_TypeDef*)(base);};

    if constexpr (constexpr auto value = portConfigurationClearLowValue<base, pins>; value)
      GPIOx()->CRL =  (GPIOx()->CRL & (~value)) | portConfigurationLowValue<base, pins>;
    if constexpr (constexpr auto value = portConfigurationClearHighValue<base, pins>; value)
      GPIOx()->CRH =  (GPIOx()->CRH & (~value)) | portConfigurationHighValue<base, pins>;
    
    if constexpr (constexpr auto value = portConfigurationODR<base, pins>::value; value)
      GPIOx()->ODR = (GPIOx()->ODR & (~portConfigurationMask<base, pins>::value))|(value);
    else GPIOx()->ODR &= ~portConfigurationMask<base, pins>::value;

    if constexpr (constexpr auto valueEMR = portConfigurationEMR<base,pins>, 
                                 valueIMR = portConfigurationIMR<base,pins>,
                                 valueFTSR = portConfigurationFTSR<base,pins>,
                                 valueRTSR = portConfigurationRTSR<base,pins>;
                  valueEMR || valueIMR || valueFTSR || valueRTSR){
      EXTI->EMR  = (EXTI->EMR & (~valueIMR))|(valueEMR);
      EXTI->IMR  = (EXTI->IMR & (~valueEMR))|(valueIMR);
      EXTI->FTSR = (EXTI->FTSR & (!valueRTSR)) | valueFTSR;
      EXTI->RTSR = (EXTI->RTSR & (!valueFTSR)) | valueRTSR;
    }

    using next = utils::pop_front_t<ports>;
    if constexpr(!utils::is_empty_v<next>)
      Config<pins, next>();
  }

  template<bool isHigh, uint8_t pinNum, controller::pinConfiguration config>
  static constexpr uint32_t GetPinConfigurationValue(){
    if constexpr (pinNum >= 8U && !isHigh) return 0U;
    else if constexpr (pinNum < 8U && isHigh) return 0U;
    return (static_cast<uint32_t>(config) & static_cast<uint32_t>(controller::pinConfiguration::ClearMask)) << 
           ((pinNum >= 8 ? pinNum - 8 : pinNum)*4U);
  }

  template<auto value, auto... base>
  static void SetValueToPort(){
    (..., [](){
      static constexpr auto GPIOx  = []() constexpr { return (GPIO_TypeDef*)(base);};
      GPIOx()->CRL = static_cast<uint32_t>(value);
      GPIOx()->CRH = static_cast<uint32_t>(value);
    }());
  }

  template<auto base, bool isHigh, bool isReset, typename...>
  struct portConfigurationValue{
    static constexpr auto value = 0U;
  };

  template<auto base, bool isHigh, bool isReset, typename Head, typename... Tail>
  struct portConfigurationValue<base, isHigh, isReset, Head, Tail...>{
    static constexpr auto number = Head::number;
    static constexpr auto config = isReset ? controller::pinConfiguration::ClearMask : Head::configuration;
    static constexpr auto baseAddress = Head::baseAddress;
    static constexpr auto value = ((baseAddress == base) ? GetPinConfigurationValue<isHigh, number, config>() : 0U) | 
                                  portConfigurationValue<base, isHigh, isReset, Tail...>::value;
  };

  template<auto base, bool isHigh, bool isReset, typename... List>
  struct portConfigurationValue<base, isHigh, isReset, utils::Typelist<List...>>{
    static constexpr auto value = portConfigurationValue<base, isHigh, isReset, List...>::value;
  };

  template<auto base, typename...>
  struct portConfigurationMask{
    static constexpr auto value = 0U;
  };

  template<auto base, typename Head, typename... Tail>
  struct portConfigurationMask<base, Head, Tail...>{
    static constexpr auto number = Head::number;
    static constexpr auto baseAddress = Head::baseAddress;
    static constexpr auto value = ((baseAddress == base) ? Head::mask : 0U) | portConfigurationMask<base, Tail...>::value;
  };

  template<auto base, typename... List>
  struct portConfigurationMask<base, utils::Typelist<List...>>{
    static constexpr auto value = portConfigurationMask<base, List...>::value;
  };

  template<auto base, typename...>
  struct portConfigurationODR{
    static constexpr auto value = 0U;
  };

  template<auto base, typename Head, typename... Tail>
  struct portConfigurationODR<base, Head, Tail...>{
    static constexpr auto baseAddress = Head::baseAddress;
    static constexpr auto config = Head::configuration;
    static constexpr auto value = portConfigurationODR<base, Tail...>::value | ((baseAddress != base) ? 0U : 
      []()constexpr -> uint32_t{if constexpr (config == controller::pinConfiguration::Output_High_2MHz ||
                                              config == controller::pinConfiguration::Output_High_10MHz || 
                                              config == controller::pinConfiguration::Output_High_50MHz || 
                                              config == controller::pinConfiguration::Input_PullUp ||
                                              config == controller::pinConfiguration::Interrupt_Falling || 
                                              config == controller::pinConfiguration::Event_Falling)
                                              {return Head::mask;} 
                                              return 0U; }());
  };

  template<auto base, typename... List>
  struct portConfigurationODR<base, utils::Typelist<List...>>{
    static constexpr auto value = portConfigurationODR<base, List...>::value;
  };

    template<auto base, controller::pinConfiguration one, controller::pinConfiguration two, typename...>
  struct portConfigurationEXTI{
    static constexpr auto value = 0U;
  };

  template<auto base, controller::pinConfiguration one, controller::pinConfiguration two, typename Head, typename... Tail>
  struct portConfigurationEXTI<base, one, two, Head, Tail...>{
    static constexpr auto mask = Head::mask;
    static constexpr auto config = Head::configuration;
    static constexpr auto baseAddress = Head::baseAddress;
    static constexpr auto value = ((baseAddress != base) ? 0U : (config == one || config == two ? mask: 0U)) | 
                                    portConfigurationEXTI<base, one, two, Tail...>::value;
  };

  template<auto base, controller::pinConfiguration one, controller::pinConfiguration two, typename... List>
  struct portConfigurationEXTI<base, one, two, utils::Typelist<List...>>{
    static constexpr auto value = portConfigurationEXTI<base, one, two, List...>::value;
  };

  template<auto base, typename... List>
  static constexpr uint32_t portConfigurationEMR = 
    portConfigurationEXTI<base, controller::pinConfiguration::Event_Falling, controller::pinConfiguration::Event_Rising, List...>::value;

  template<auto base, typename... List>
  static constexpr uint32_t portConfigurationIMR = 
    portConfigurationEXTI<base, controller::pinConfiguration::Interrupt_Falling, controller::pinConfiguration::Interrupt_Rising, List...>::value;

  template<auto base, typename... List>
  static constexpr uint32_t portConfigurationFTSR = 
    portConfigurationEXTI<base, controller::pinConfiguration::Event_Falling, controller::pinConfiguration::Interrupt_Falling, List...>::value;

  template<auto base, typename... List>
  static constexpr uint32_t portConfigurationRTSR = 
    portConfigurationEXTI<base, controller::pinConfiguration::Event_Rising, controller::pinConfiguration::Interrupt_Rising, List...>::value;

  template<auto base, typename... List>
  static constexpr uint32_t portConfigurationLowValue = portConfigurationValue<base, false, false, List...>::value;

  template<auto base, typename... List>
  static constexpr uint32_t portConfigurationHighValue = portConfigurationValue<base, true, false, List...>::value;

  template<auto base, typename... List>
  static constexpr uint32_t portConfigurationClearLowValue = portConfigurationValue<base, false, true, List...>::value;

  template<auto base, typename... List>
  static constexpr uint32_t portConfigurationClearHighValue = portConfigurationValue<base, true, true, List...>::value;

  template<uint8_t pinNum, controller::pinConfiguration config,
           controller::pinConfiguration one, controller::pinConfiguration second>
  constexpr uint32_t GetExternalValue(){
    if constexpr (config == one || config == second) return (1U << pinNum); 
    return 0U;
  }

  template<bool isHigh, typename pins, typename power>
  static void SetOutput(){ 
    static constexpr auto base = utils::front_t<power>::value;
    static constexpr auto GPIOx  = []() constexpr { return (GPIO_TypeDef*)(base);};
    static constexpr auto value = portConfigurationMask<base, pins>::value;
    GPIOx()->BSRR = ((value << 16U) | (isHigh ? value : 0U));

    using next = utils::pop_front_t<power>;
    if constexpr (!utils::is_empty_v<next>)
      SetOutput<isHigh, pins, next>();

  }

  template<bool isWrite, typename positionList, typename Pinslist> 
  static uint32_t singleValue(uint32_t val){
    uint32_t value;
    if constexpr (isWrite) value = shiftedDirectValue<(1U << utils::front_v<positionList>), utils::front_t<Pinslist>::number, utils::front_v<positionList>>(val);
    else value = shiftedDirectValue<(1U << utils::front_t<Pinslist>::number), utils::front_v<positionList>, utils::front_t<Pinslist>::number>(val);
    if constexpr (utils::is_empty_v<utils::pop_front_t<positionList>>) 
      return value;
    else 
      return value | singleValue<isWrite, utils::pop_front_t<positionList>, utils::pop_front_t<Pinslist>>(val);
  }

  template<bool isWrite, auto frontPinPosition, typename frontPin, 
           typename NoneMask, typename singlePositions, typename positionList, typename Pinslist>
  static uint32_t directValue(uint32_t val){
    using current = pinlist_traits::directChainValue<frontPinPosition, frontPin, NoneMask, singlePositions, positionList, Pinslist>;
    constexpr uint32_t mask = isWrite ? current::mask : current::maskRead;
    constexpr uint32_t number = isWrite ? frontPin::number : frontPinPosition;
    constexpr uint32_t position = isWrite ? frontPinPosition : frontPin::number;
    uint32_t value = shiftedDirectValue<mask, number, position>(val);
    if constexpr (utils::is_empty_v<typename current::rest>)
      return value;
    else{
      return value | 
        directValue<
                    isWrite,
                    utils::front_v<typename current::newPositionList>, 
                    utils::front_t<typename current::rest>, 
                    typename current::singlePinsList, 
                    typename current::singlePinsPositions, 
                    typename current::newPositionList, 
                    typename current::rest
                  >(val);
    }
  }

  template<bool isWrite, auto frontPinPosition, typename frontPin, 
           typename NoneMask, typename singlePositions, typename positionList, typename Pinslist>
  static uint32_t newValue(uint32_t val){
    using current = pinlist_traits::reverseChainValue<frontPinPosition, frontPin, NoneMask, singlePositions, positionList, Pinslist>;
    constexpr uint32_t mask = isWrite ? current::mask : current::maskRead;
    uint32_t value = current::pinsNumber > 1 ? __RBIT(val & mask) >> current::shift : 0U;
    if constexpr (utils::is_empty_v<typename current::rest>){
      if constexpr (utils::is_empty_v<typename current::singlePinsList>) 
        return value;
      else 
        return value | 
          directValue<
                      isWrite,
                      utils::front_v<
                      typename current::singlePinsPositions>, 
                      utils::front_t<typename current::singlePinsList>, 
                      utils::Typelist<>, 
                      utils::Valuelist<>, 
                      typename current::singlePinsPositions, 
                      typename current::singlePinsList
                    >(val);
    }
    else
      return value | newValue<
                              isWrite,
                              utils::front_v<typename current::newPositionList>, 
                              utils::front_t<typename current::rest>, 
                              typename current::singlePinsList, 
                              typename current::singlePinsPositions, 
                              typename current::newPositionList, 
                              typename current::rest
                            >(val);
  }

  template<auto mask, auto number, auto position>
  static uint32_t shiftedDirectValue(uint32_t value){
    static constexpr bool isLeftShift = number > position ? true : false;
    static constexpr uint32_t shift = isLeftShift ? 
                     number - position : 
                     position - number;
    if constexpr (isLeftShift) return (value & mask) << shift;
    else return (value & mask) >> shift;
  }

  static_assert(pinlist_traits::is_pins_unique_v<pins>, 
    "Pin list has the repetitive numbers");
};
 
}// !namespace stm32f1::hardware

#endif //! _STM32F1_PINLIST_HPP