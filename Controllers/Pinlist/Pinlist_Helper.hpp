//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Helper class for interface of Pinlist
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _PINLIST_HELPER_HPP
#define _PINLIST_HELPER_HPP

#include "../Common/Compiler/Compiler.h"
#include "../../Utils/type_traits_custom.hpp"


// Forward declaraion
namespace controller{

class Interrupt;

namespace interfaces{

template<typename, typename...>
class IPinlist;

template<typename>
class IPower;

} // !namespace interfaces

} // !namespace controller

/*!
  @brief Helper for pinlist
*/
namespace helper::pinlist{

template<typename adapter, typename... Pins>
class Helper{

  Helper() = delete;

  template<typename>
  friend class controller::interfaces::IPower;

  template<typename, typename...>
  friend class controller::interfaces::IPinlist;

  friend controller::Interrupt;

protected:

template<auto Value>
struct return_value{ static constexpr auto value = Value; };

template<auto Value>
struct return_mask{ static constexpr auto value = 1U << Value; };

template<typename T>
struct numberHelper;

template<typename... T>
struct numberHelper<trait::Typelist<T...>>{ using type = trait::Valuelist<T::number...>; };

template<typename T>
struct portHelper;

template<typename... T>
struct portHelper<trait::Typelist<T...>>{ using type = trait::Valuelist<T::port...>; };

template<typename T>
struct configurationHelper;

template<typename... T>
struct configurationHelper<trait::Typelist<T...>>{ using type = trait::Valuelist<T::configuration...>; };

template<typename T>
struct remapHelper;

template<typename... T>
struct remapHelper<trait::Typelist<T...>>{ using type = trait::Valuelist<T::remap...>; };

template<typename T>
struct addressWriteHelper;

template<typename... T>
struct addressWriteHelper<trait::Typelist<T...>>{ 
  using type = trait::make_unique_t<trait::Valuelist<T::addressWrite...>>;
};

template<typename T>
struct addressReadHelper;

template<typename... T>
struct addressReadHelper<trait::Typelist<T...>>{ 
  using type = trait::make_unique_t<trait::Valuelist<T::addressRead...>>;
};

template<typename T>
struct uniqueHelper;

template<typename... T>
struct uniqueHelper<trait::Typelist<T...>>{ using type = trait::Typelist<typename T::unique...>;};

template<template<auto Value> typename modification, auto address, typename addresses, typename values, typename Result = trait::Valuelist<>>
struct get_mask{
  static constexpr auto firstAddresses = trait::front_v<addresses>;
  static constexpr auto firstValue = modification<trait::front_v<values>>::value;
  static constexpr auto isAddressMatch = address == firstAddresses;
  using result = std::conditional_t<isAddressMatch, trait::push_back_value_t<Result, firstValue>, Result>;
  using nextAddresses= trait::pop_front_t<addresses>;
  using nextValues= trait::pop_front_t<values>;
  static constexpr auto value = get_mask<modification, address, nextAddresses, nextValues, result>::value;
};

template<template<auto Value> typename modification, auto address, typename listValues, typename Result>
struct get_mask<modification, address, trait::Valuelist<>, listValues, Result>{
  static constexpr auto value = trait::valuelist_or_v<Result>;
};

template<template<auto Value> typename modification, typename addressesSorted, typename addresses, typename values, typename Result = trait::Valuelist<>>
struct masks{
  static constexpr auto firstAddressesSorted = trait::front_v<addressesSorted>;
  using result = trait::push_back_value_t<Result, get_mask<modification, firstAddressesSorted, addresses, values>::value>;
  using nextAddressesSorted = trait::pop_front_t<addressesSorted>; 
  using type = typename masks<modification, nextAddressesSorted, addresses, values, result>::type;
};

template<template<auto Value> typename modification, typename addresses, typename values, typename Result>
struct masks<modification, trait::Valuelist<>, addresses, values, Result>{
  using type = Result;
};

template<template<auto Value> typename modification, typename addressesSorted, typename addresses, typename values>
using masks_t = typename masks<modification, addressesSorted, addresses, values>::type;

// Make list of pins_positions/pins_numbers
                                                                                
template<auto Port, typename pins, bool isConfig, bool isNumber, auto position = trait::size_of_list_v<pins> - 1, bool empty = trait::is_empty_v<pins>>
struct make_pins{
  using frontPin = trait::front_t<pins>;
  static constexpr auto port = frontPin::port;
  static constexpr auto number = frontPin::number;
  static constexpr auto config = frontPin::configuration;
  static constexpr auto valueToStore = [](){if constexpr(isConfig) return config; else return isNumber ? number : position;}();
  using rest = typename make_pins<Port, trait::pop_front_t<pins>, isConfig, isNumber, position - 1>::type;
  using type = std::conditional_t<Port == port, trait::push_front_value_t<rest, valueToStore>, rest>;
};

template<auto Port, bool isConfig, bool isNumber, auto position>
struct make_pins<Port, trait::Typelist<>, isConfig, isNumber, position, true>{
  using type = trait::Valuelist<>;
};

template<auto Port>
using make_pins_numbers_t = typename make_pins<Port, trait::lists_expand_t<typename Pins::initialization::pins...>, false, true>::type;

template<auto Port>
using make_pins_positions_t = typename make_pins<Port, trait::lists_expand_t<typename Pins::initialization::pins...>, false, false>::type;

template<auto Port>
using make_pins_config_t = typename make_pins<Port, trait::lists_expand_t<typename Pins::initialization::pins...>, true, false>::type;

template<typename Ports, typename Result = trait::Typelist<>, bool empty = trait::is_empty_v<Ports>>
class make_pins_list_of_ports{
  static constexpr auto port = trait::front_v<Ports>;
  using next = trait::pop_front_t<Ports>;
  using list = make_pins_numbers_t<port>;
  using result = trait::push_back_t<Result,list>;
public:
  using type = typename make_pins_list_of_ports<next, result>::type;
};

template<typename Ports, typename Result>
class make_pins_list_of_ports<Ports, Result, true>{
public:  
  using type = Result;
};

//End of Make list of pins_positions/pins_numbers

template<bool isReverse, auto position, auto number, typename positionsList, typename numbersList,
         typename notChainedPositions = trait::Valuelist<>, typename notChainedNumbers = trait::Valuelist<>>
struct chain{
  static constexpr auto diffPosition = isReverse ? position - trait::front_v<positionsList> : trait::front_v<positionsList> - position;
  static constexpr bool isChain = trait::front_v<numbersList> - number == diffPosition;
  
  using next = chain< isReverse,
                      position, 
                      number, 
                      trait::pop_front_t<positionsList>, 
                      trait::pop_front_t<numbersList>,
                      notChainedPositions,
                      notChainedNumbers>;

  static constexpr auto pinsCount = (isChain ? 1U : 0U) + next::pinsCount;

    // Add pins position, if it is not in chain                                        
  using NotChainedPositions = std::conditional_t< pinsCount < 2, 
                                                  trait::push_back_value_t<notChainedPositions, position>, 
                                                  notChainedPositions>;
    // Add pins number, if it is not in chain   
  using NotChainedNumbers = std::conditional_t< pinsCount < 2,
                                                trait::push_back_value_t<notChainedNumbers, number>, 
                                                notChainedNumbers>;
    // Add pins position to next intertation
  using newPositions = std::conditional_t< !isChain, 
                                            trait::push_front_value_t<typename next::newPositions, trait::front_v<positionsList>>,  
                                            typename next::newPositions>;
    // Add pins numbers to next intertation
  using newNumbers = std::conditional_t< !isChain, 
                                          trait::push_front_value_t<typename next::newNumbers, trait::front_v<numbersList>>, 
                                          typename next::newNumbers>;

  static constexpr auto mask = ((isChain ? 1U : 0U) << (trait::front_v<positionsList>)) | next::mask;
  static constexpr auto maskRead = ((isChain ? 1U : 0U) << trait::front_v<numbersList>) | next::maskRead;
  
  static constexpr auto isLeftShift = number > position;
  static constexpr auto shift = isReverse ? 31U - trait::front_v<numbersList> - trait::front_v<positionsList> : 
                                            isLeftShift ? number - position : position - number;

};

template<bool isReverse, auto position, auto number, typename notChainedPositions, typename notChainedNumbers>
struct chain<isReverse, position, number, trait::Valuelist<>, trait::Valuelist<>, notChainedPositions, notChainedNumbers>{
  static constexpr auto mask = 1U << position;
  static constexpr auto maskRead = 1U << number;
  static constexpr auto pinsCount = 0U;
  using newNumbers = trait::Valuelist<>;
  using newPositions = trait::Valuelist<>;
  using NotChainedPositions = notChainedPositions;
  using NotChainedNumbers = notChainedNumbers;
};

template<bool isReverse, typename positionsList, typename numbersList, typename notChainedPositions , typename notChainedNumbers>
using chain_t = chain<isReverse, trait::front_v<positionsList>, trait::front_v<numbersList>, 
                      positionsList, numbersList, notChainedPositions, notChainedNumbers>;

  struct initialization{
    using power = trait::lists_termwise_or_t<typename Pins::initialization::power...>;
    using pins = trait::lists_expand_t<typename Pins::initialization::pins...>;
    using interrupts = trait::lists_expand_t<typename Pins::initialization::interrupts...>;
  };

  using pins = typename initialization::pins;
  using number = typename numberHelper<pins>::type;
  using port = typename portHelper<pins>::type;
  using configuration = typename configurationHelper<pins>::type;
  using remap = typename remapHelper<pins>::type;
  using addressWrite = typename addressWriteHelper<pins>::type;
  using addressRead = typename addressReadHelper<pins>::type;
  using unique = typename uniqueHelper<pins>::type;

  using uniquePorts = trait::sort_insertion_smaller_t<trait::make_unique_t<port>>;
  using list_of_pins_from_ports = typename make_pins_list_of_ports<uniquePorts>::type;
  using mask = masks_t<return_mask, uniquePorts, port, number>;

  static constexpr auto size = trait::size_of_list_v<pins>;

template<bool isWrite, bool isReverse, typename chain>
__FORCE_INLINE static auto _GetChainValue(uint32_t value){
  static constexpr auto mask = isWrite ? chain::mask : chain::maskRead;
  static constexpr auto shift = chain::shift;
  auto _value = (value & mask);
  static constexpr auto isLeftShift = isWrite ? chain::isLeftShift : !chain::isLeftShift;
  return  isReverse ?  (chain::pinsCount > 1U ? __RBIT(_value) >> shift : 0U) :
         (isLeftShift ? _value << shift : (_value >> shift));  
}

template<bool isWrite, bool isReverse, typename positionsList, typename numbersList,
         typename notChainedPositions = trait::Valuelist<>, typename notChainedNumbers = trait::Valuelist<>>
__FORCE_INLINE static uint32_t _GetValue(uint32_t value){
  using namespace trait;
  using current = chain_t<isReverse, positionsList, numbersList, notChainedPositions, notChainedNumbers>;

  uint32_t _value = _GetChainValue<isWrite, isReverse, current>(value);

  static constexpr bool isEndOfReversesChains = (is_empty_v<typename current::newPositions> && isReverse);
  static constexpr bool isNextReverse = isEndOfReversesChains ? !isReverse : isReverse;
    
  using positions = std::conditional_t<isEndOfReversesChains, 
                                      typename current::NotChainedPositions, typename current::newPositions>;
  using numbers = std::conditional_t<isEndOfReversesChains, 
                                     typename current::NotChainedNumbers, typename current::newNumbers>;
  using ncPositions = std::conditional_t<isEndOfReversesChains, 
                                         Valuelist<>, typename current::NotChainedPositions>;
  using ncNumbers = std::conditional_t<isEndOfReversesChains, 
                                       Valuelist<>, typename current::NotChainedNumbers>;

  // If there are no pins in the reverse and direct chains
  if constexpr ((is_empty_v<positions> && !isNextReverse))
    return _value;
  else 
    return _value | _GetValue< isWrite, isNextReverse, positions, numbers,
                                  ncPositions,ncNumbers>(value);                       
}

template<typename positionsList, typename numbersList>
__FORCE_INLINE  static uint32_t _GetWriteValue(uint32_t value){
  return _GetValue<true, true, positionsList, numbersList>(value);
}

template<auto value, typename positions, typename numbers>
__FORCE_INLINE constexpr auto static _GetWriteValue(){
  using namespace trait;
  constexpr auto _value = (value & (1 << front_v<positions>)) ? 1 << front_v<numbers> : 0;
  if constexpr (!is_empty_v<pop_front_t<positions>>)
    return _value | _GetWriteValue<value, pop_front_t<positions>, pop_front_t<numbers>>();
  else
    return _value;
}

template<typename positionsList, typename numbersList>
__FORCE_INLINE  static uint32_t _GetReadValue(uint32_t value){
  return _GetValue<false, true, positionsList, numbersList>(value);
}

template<bool isWrite = false, bool isCompileTime = false, auto valueC = 0,
        typename _ports = uniquePorts, typename _masks = mask, typename _addressesWrite = addressWrite, typename _addressesRead = addressRead>
__FORCE_INLINE static uint32_t _WriteRead(uint32_t& value){
  if constexpr (size > 32){
    static_assert(size <= 32, "Number of pins in list is exceed 32");
  } else {
    using namespace trait;
    using address = std::conditional_t<isWrite, _addressesWrite, _addressesRead>; 
    if constexpr(!is_empty_v<address>){
      static constexpr auto port = front_v<_ports>;
      static constexpr auto mask = front_v<_masks>;
      static constexpr auto address = isWrite ? front_v<_addressesWrite> : front_v<_addressesRead>;

      using positions = make_pins_positions_t<port>;
      using numbers = make_pins_numbers_t<port>;
      static constexpr auto countPins = size_of_list_v<numbers>;

      if constexpr (isWrite){
        if constexpr(!isCompileTime)
          adapter::template _Write<address, mask, countPins>(_GetWriteValue<positions, numbers>(value));
        else
          adapter::template _Write<address, _GetWriteValue<valueC, positions, numbers>(), mask, countPins>();
      } else{
        auto readValue = adapter:: template _Read<address, mask>();
        value |= _GetReadValue<positions,numbers, countPins>(readValue);
      }

      using restPorts = pop_front_t<_ports>;
      using restMasks = pop_front_t<_masks>;
      using restAddressesWrite = pop_front_t<_addressesWrite>;
      using restAddressesRead = pop_front_t<_addressesRead>;

      _WriteRead<isWrite, isCompileTime, valueC, restPorts, restMasks, restAddressesWrite, restAddressesRead>(value);
    }
  }
  return value;
}

static uint32_t _Read(){
  uint32_t value = 0;
  return _WriteRead<>(value);
}

static void _WriteRunTime(uint32_t value){
  _WriteRead<true>(value);
}

template<uint32_t value>
static void _WriteCompileTime(){
  uint32_t valueDummy;
  _WriteRead<true, true, value>(valueDummy);
}


template<typename valuesList>
static void _Configure(){

  using config = typename trait::front_t<pins>::_configuration<port, number, valuesList, remap>;

  if constexpr (static_cast<bool>(size % (trait::size_of_list_v<uniquePorts> * adapter::pinsInPort)))
    adapter::template _ConfigureSet<typename config::addresses, typename config::values, typename config::masks>();
  else
    adapter::template _ConfigureWrite<typename config::addresses, typename config::values, typename config::masks>();

}

static void _Init(){ 
  _Configure<configuration>();
}
static void _DeInit(){
  using type = decltype(trait::front_v<configuration>);
  using valuesList = trait::generate_valuelist_t<trait::size_of_list_v<pins>, type::Default>;
  _Configure<valuesList>();
}
static void _LowPower(){ 
  using type = decltype(trait::front_v<configuration>);
  using valuesList = trait::generate_valuelist_t<trait::size_of_list_v<pins>, type::Low_Power>;
  _Configure<valuesList>();
}

template<typename pins, typename Result = trait::Typelist<>, bool empty = trait::is_empty_v<pins>>
struct pop_pins{
  using front = trait::front_t<pins>;
  using next = trait::pop_front_t<pins>;
  using result = trait::push_back_t<Result, front>;
  using type = typename pop_pins<next, result>::type;
};

template<typename pins, typename Result>
struct pop_pins<pins, Result, true>{
  using type = Result;
};

template<typename pins, auto config, typename Result = trait::Typelist<>, bool empty = trait::is_empty_v<pins>>
struct change_config{
  using front = typename trait::front_t<pins>::mode::set<config>;
  using next = trait::pop_front_t<pins>;
  using result = trait::push_back_t<Result, front>;
  using type = typename change_config<next, config, result>::type;
};

template<typename pins, auto config, typename Result>
struct change_config<pins, config, Result, true>{
  using type = Result;
};
    
template<typename List, auto config, auto port, size_t endNumber,
         typename Result = pins, size_t currentNumber = 0, 
         bool isEnd = ((endNumber > currentNumber) ? (currentNumber > endNumber) : (currentNumber <= endNumber))>
class add_pins{
  static constexpr auto isContain = trait::is_contain_value_v<currentNumber, List>;
  static constexpr auto nextNumber = (endNumber > currentNumber) ? currentNumber + 1 : currentNumber - 1;
  using pin = typename trait::front_t<pins>::mode::reconfigure<port, currentNumber, config>;
  using result = std::conditional_t<isContain, Result, trait::push_back_t<Result, pin>>;
public:  
  using type = typename add_pins<List, config, port, endNumber, result, nextNumber>::type;
};

template<typename List, auto config, auto port, size_t endNumber, typename Result, size_t currentNumber>
class add_pins<List, config, port, endNumber, Result, currentNumber, true>{
public:
  using pin = typename trait::front_t<pins>::mode::reconfigure<port, currentNumber, config>;
  using type = trait::push_back_t<Result, pin>;
  static_assert(endNumber <= (adapter::pinsInPort - 1), "Number of 'end pin' is exceed pins qty in the port");
};

template<auto config, typename Result = pins, 
         typename Ports = uniquePorts, typename lists_of_pins = list_of_pins_from_ports, bool empty = trait::is_empty_v<Ports>>
class generate_pinlist{
  static constexpr auto port = trait::front_v<Ports>;
  using list = trait::front_t<lists_of_pins>;
  using nextPorts = trait::pop_front_t<Ports>;
  using nextLists = trait::pop_front_t<lists_of_pins>;
  using result = typename add_pins<list, config, port, adapter::pinsInPort - 1, Result>::type;
public:
  using type = typename generate_pinlist<config, result, nextPorts, nextLists>::type;
};

template<auto config, typename Result, typename Ports, typename lists_of_pins>
class generate_pinlist<config, Result, Ports, lists_of_pins, true>{
public:
  using type = Result;
};

template<auto number, bool isGetPin = false>
static constexpr auto _CheckNumber(){
  if constexpr (size <= number || (number == 0 && !isGetPin)){
    static_assert(size > number && (number == 0 && !isGetPin), 
      "The number of pin is exceed size of Pinlist, or number of pins to pop == 0");
    return 0;
  } else{
    if constexpr (isGetPin) return size - number - 1U;
    else return number;
  }
}

template<typename T>
struct toPinlist;

template<typename... T>
struct toPinlist<trait::Typelist<T...>>{
  using type = typename adapter::newList<T...>;
};

template<auto number>
using pop_front_pins_t = typename toPinlist<typename pop_pins<trait::pop_front_number_t<_CheckNumber<number>(), pins>>::type>::type;

template<auto number>
using pop_back_pins_t = typename toPinlist<typename pop_pins<trait::pop_back_number_t<_CheckNumber<number>(), pins>>::type>::type;

template<auto number>
using get_pin_t = typename trait::get_element_t<_CheckNumber<number, true>(), pins>;

template<auto config>
using set = typename toPinlist<typename change_config<pins, config>::type>::type;

struct generate{

  template<auto config>
  using set = typename toPinlist<typename generate_pinlist<config>::type>::type;

  template<size_t numberPinEnd>
  using row = typename toPinlist<typename add_pins<make_pins_numbers_t<get_pin_t<0>::port>, 
              get_pin_t<0>::configuration, get_pin_t<0>::port, numberPinEnd,
              pins, get_pin_t<0>::number>::type>::type;

};

static_assert(trait::is_unique_v<unique>, 
             "Pins in list are not unique");

};

} // !namespase helper::pinlist

#endif // _PINLIST_HELPER_HPP