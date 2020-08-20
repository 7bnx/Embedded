//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Traits for list of pins 
//  TODO:         Add description 
//----------------------------------------------------------------------------------

#ifndef _PINLIST_TRAITS_HPP
#define _PINLIST_TRAITS_HPP

#include <cstddef>
#include "type_traits_custom.hpp"
#include "pin_configuration.hpp"

/*!
  @brief Namespace for pinlist traits
*/
namespace pinlist_traits{

  // Make unique ports

  template<typename...>
  struct make_list_ports;

  template<typename Head, typename... Tail>
  struct make_list_ports<utils::Typelist<Head, Tail...>>{
    static constexpr auto frontBase = Head::baseAddress;
    using next = utils::Typelist<Tail...>;
    using rest = typename make_list_ports<next>::type;
    using type = utils::push_front_value_t<rest, frontBase>;
  };

  template<>
  struct make_list_ports<utils::Typelist<>>{
    using type = utils::Valuelist<>;
  };

  template<typename Ports>
  using make_unique_ports_t = utils::make_unique_t<typename make_list_ports<Ports>::type>;

  // End of  make unique ports


  // Make list of pins numbers

  template<typename...>
  struct make_list_pin_numbers;

  template<typename Head, typename... Tail>
  struct make_list_pin_numbers<utils::Typelist<Head, Tail...>>{
    static constexpr auto frontNumber = Head::number;
    using next = utils::Typelist<Tail...>;
    using rest = typename make_list_pin_numbers<next>::type;
    using type = utils::push_front_value_t<rest, frontNumber>;
  };

  template<>
  struct make_list_pin_numbers<utils::Typelist<>>{
    using type = utils::Valuelist<>;
  };

  template<typename List>
  using make_list_pin_numbers_t = typename make_list_pin_numbers<List>::type;

  // End of list of pins numbers


  // Check if pins are unique

  template<typename...>
  static constexpr auto is_pins_unique_v = std::true_type{};

  template <typename Head, typename... Tail>
  static constexpr auto is_pins_unique_v<Head, Tail...> =
    std::bool_constant<(((Head::baseAddress == Tail::baseAddress ? Head::number != Tail::number: true))&& ...) && is_pins_unique_v<Tail...>>{};

  template<typename... list>
  static constexpr auto is_pins_unique_v<utils::Typelist<list...>> = is_pins_unique_v<list...>;

  // End of pins unique



  // Set config to pins

  template<auto config, typename...>
  struct set_config_to_pins;


  template<auto config, typename Head, typename... Tail>
  struct set_config_to_pins<config, utils::Typelist<Head, Tail...>>{

    static constexpr auto frontNumber = Head::number;
    using newPin = controller::abstarct::Pin<Head::baseAddress, Head::number, config>;
    using next = utils::Typelist<Tail...>;
    using rest = typename set_config_to_pins<config, next>::type;
    using type = utils::push_front_t<rest, newPin>;
  };

  template<auto config>
  struct set_config_to_pins<config, utils::Typelist<>>{
    using type = utils::Typelist<>;
  };

  template<auto config, typename List>
  using set_config_to_pins_t = typename set_config_to_pins<config, List>::type;

  // End of set config to pins


  // Pinlist generator

  template<auto gpioBase, auto config, auto startPin, auto endPin>
  struct generate_pinlist{
    using pin = controller::abstarct::Pin<gpioBase, startPin, config>;
    using rest = typename generate_pinlist<gpioBase, config, startPin + 1, endPin>::type;
    using type = std::conditional_t<startPin < endPin, utils::push_front_t<rest, pin>, rest>;
  };

  template<auto gpioBase, auto config, auto startPin>
  struct generate_pinlist<gpioBase, config, startPin, startPin>{
    using pin = controller::abstarct::Pin<gpioBase, startPin, config>;
    using type = utils::Typelist<pin>;
  };

  // End of Pinlist generator


  // Creates list of Pins, belongs to port

  template<auto Port, typename Pins, bool empty = utils::is_empty_v<Pins>>
  struct make_port_pinlist;

  template<auto Port, typename Pins>
  struct make_port_pinlist<Port, Pins, false>{
    static constexpr auto frontBaseAddress = utils::front_t<Pins>::baseAddress;
    using frontPin = utils::front_t<Pins>;
    using rest = typename make_port_pinlist<Port, utils::pop_front_t<Pins>>::type;
    using type = std::conditional_t<Port == frontBaseAddress, utils::push_front_t<rest, frontPin>, rest>;
  };

  template<auto Port>
  struct make_port_pinlist<Port, utils::Typelist<>, true>{
    using type = utils::Typelist<>;
  };

  template<auto Port, typename Pins>
  using make_port_pinlist_t = typename make_port_pinlist<Port, Pins>::type;

  //End of make_port_pinlist


  // Creates list of Pins positions, belongs to port

  template<auto Port, auto endPosition, typename Pins, bool empty = utils::is_empty_v<Pins>>
  struct make_port_positionlist;

  template<auto Port, auto endPosition, typename Pins>
  struct make_port_positionlist<Port, endPosition, Pins, false>{
    static constexpr auto frontBaseAddress = utils::front_t<Pins>::baseAddress;
    using frontPin = utils::front_t<Pins>;
    using rest = typename make_port_positionlist<Port, endPosition - 1, utils::pop_front_t<Pins>>::type;
    using type = std::conditional_t<Port == frontBaseAddress, utils::push_front_value_t<rest, endPosition>, rest>;
  };

  template<auto Port, auto endPosition>
  struct make_port_positionlist<Port, endPosition, utils::Typelist<>, true>{
    using type = utils::Valuelist<>;
  };

  template<auto Port, auto endPosition, typename Pins>
  using make_port_positionlist_t = typename make_port_positionlist<Port, endPosition, Pins>::type;

  //End of make_port_positionlist


  // Finding pins sequences

  template<auto frontPinPosition, typename frontPin, typename singlePins, typename singlePositions, typename positionList, typename Pins>
  struct reverseChainValue{
    using frontList = utils::front_t<Pins>;
    static constexpr bool isSequence = frontList::number - frontPin::number == frontPinPosition - utils::front_v<positionList>;
    using next = reverseChainValue<
                                    frontPinPosition, 
                                    frontPin, 
                                    singlePins, 
                                    singlePositions, 
                                    utils::pop_front_t<positionList>, 
                                    utils::pop_front_t<Pins>
                                  >;
    using newPositionList = std::conditional_t< !isSequence, 
                                                utils::push_front_value_t<typename next::newPositionList, utils::front_v<positionList>>, 
                                                typename next::newPositionList
                                              >;
    using rest = std::conditional_t<!(isSequence), utils::push_front_t<typename next::rest, frontList>, typename next::rest>;
    static constexpr uint32_t mask = ((isSequence ? 1U : 0U) << (utils::front_v<positionList>)) | next::mask;
    static constexpr uint32_t maskRead = (isSequence ? 1U : 0U) << (frontList::number) | next::maskRead;
    static constexpr uint32_t shift = 31U - frontList::number - utils::front_v<positionList>;
    static constexpr uint32_t pinsNumber = (isSequence ? 1U : 0U) + next::pinsNumber;
    using singlePinsListHelper = std::conditional_t<  (reverseChainValue<frontPinPosition, frontPin, singlePins, singlePositions, positionList, Pins>::pinsNumber > 1U), 
                                                      utils::Typelist<>, 
                                                      frontPin
                                                  >;
    using singlePinsList = utils::push_front_t<singlePins, singlePinsListHelper>;
    using singlePinsPositions = std::conditional_t< pinsNumber < 2, 
                                                    utils::push_front_value_t<singlePositions, utils::front_v<positionList>>, 
                                                    singlePositions
                                                  >;
  };

  template<auto frontPinPosition, typename frontPin, typename singlePins, typename singlePositions>
  struct reverseChainValue<frontPinPosition, frontPin, singlePins, singlePositions, utils::Valuelist<>, utils::Typelist<>>{
    static constexpr uint32_t mask = 1U << frontPinPosition;
    static constexpr uint32_t maskRead = 1U << frontPin::number;
    using rest = utils::Typelist<>;
    using newPositionList = utils::Valuelist<>;
    static constexpr uint32_t pinsNumber = 0U;
    using singlePinsPositions = singlePositions;
  };

  template<auto frontPinPosition, typename frontPin, typename singlePins, typename singlePositions, typename positionList, typename Pins>
  struct directChainValue{
    using frontList = utils::front_t<Pins>;
    static constexpr bool isSequence = frontList::number - frontPin::number == utils::front_v<positionList> - frontPinPosition;
    using next = directChainValue<
                                    frontPinPosition, 
                                    frontPin, 
                                    singlePins, 
                                    singlePositions, 
                                    utils::pop_front_t<positionList>, 
                                    utils::pop_front_t<Pins>
                                  >;
    using newPositionList = std::conditional_t< !isSequence, 
                                                utils::push_front_value_t<typename next::newPositionList, utils::front_v<positionList>>, 
                                                typename next::newPositionList
                                              >;
    using rest = std::conditional_t<  !isSequence, 
                                      utils::push_front_t<typename next::rest, frontList>, 
                                      typename next::rest
                                  >;
    static constexpr uint32_t maskRead = (isSequence ? 1U : 0U) << (frontList::number) | next::maskRead;
    static constexpr uint32_t mask = ((isSequence ? 1U : 0U) << (utils::front_v<positionList>)) | next::mask;
    static constexpr uint32_t shift = frontPin::number >= frontPinPosition ? frontPin::number - frontPinPosition : frontPin::number;
    static constexpr uint32_t pinsNumber = (isSequence ? 1U : 0U) + next::pinsNumber;
    using singlePinsListHelper = std::conditional_t< (directChainValue<frontPinPosition, frontPin, singlePins, singlePositions, positionList, Pins>::pinsNumber > 1U), 
                                                    utils::Typelist<>, 
                                                    frontPin
                                                  >;
    using singlePinsList = utils::push_front_t<singlePins, singlePinsListHelper>;
    using singlePinsPositions = std::conditional_t< pinsNumber < 2, 
                                                    utils::push_front_value_t<singlePositions, 
                                                    utils::front_v<positionList>>, 
                                                    singlePositions
                                                  >;
  };

  template<auto frontPinPosition, typename frontPin, typename singlePins, typename singlePositions>
  struct directChainValue<frontPinPosition, frontPin, singlePins, singlePositions, utils::Valuelist<>, utils::Typelist<>>{
    static constexpr uint32_t mask =  1U << frontPinPosition;
    static constexpr uint32_t maskRead = 1U << frontPin::number;
    using rest = utils::Typelist<>;
    using newPositionList = utils::Valuelist<>; 
    static constexpr uint32_t pinsNumber = 0U;
    using singlePinsPositions = singlePositions;
  };

  // End of Finding pins sequences


  // Make list of unique ports and power value

  template<typename pinsList, bool empty = utils::is_empty_v<pinsList>>
  struct port_power_list{
    using next = utils::pop_front_t<pinsList>;

    using frontPort = utils::ctv<utils::front_t<pinsList>::baseAddress>;
    using rest_port_list = typename port_power_list<next>::port_list;
    using port_list = utils::push_front_t<rest_port_list, frontPort>;

    static constexpr bool isAlternativeFunction = 
        utils::front_t<pinsList>::configuration == controller::pinConfiguration::Alternative_PushPull ||
        utils::front_t<pinsList>::configuration == controller::pinConfiguration::Alternative_OpenDrain ? true : false; 

    using rest_power_list = typename port_power_list<next>::power_list;
    using _power_list = utils::push_front_t<rest_power_list, frontPort>;
    using power_list = std::conditional_t<isAlternativeFunction, utils::push_front_t<_power_list, utils::ctv<AFIO_BASE>>, _power_list>;
  };

  template<typename pinsList>
  struct port_power_list<pinsList, true>{
    using port_list = utils::Typelist<>;
    using power_list = utils::Typelist<>;
  };

  template<typename pinsList>
  using make_power_list_t = utils::make_unique_t<typename port_power_list<pinsList>::power_list>;

  template<typename pinsList>
  using make_port_list_t = utils::make_unique_t<typename port_power_list<pinsList>::port_list>;


} //!namspace pinlist_traits

#endif //!_PINLIST_TRAITS_HPP