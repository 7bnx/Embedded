//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Operations with registers of controller
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _REGISTERS_HPP
#define _REGISTERS_HPP

#include "../Compiler/Compiler.h"
#include "../../../Utils/type_traits_custom.hpp"

namespace controller::hardware{

/*!
  @brief Operations with registers
*/
class Registers{

  Registers() = delete;

  template<auto address, typename accessType = decltype(address)>
  static inline auto& reg = *reinterpret_cast<volatile accessType * const>(address);

/*!
  @brief Set or Reset bits in the registers
  @tparam <ValueList> list of values to set 
  @tparam <ReseMaskListtList> list of values to reset/mask
  @tparam <AddressesList> list of registers addresses to operate
*/
  template<bool isSet, typename AddressesList, typename ValueList, typename MaskList>
  __FORCE_INLINE static void _Modify(){
    using namespace trait;

    if constexpr (!is_empty_v<ValueList> && !is_empty_v<MaskList> && !is_empty_v<AddressesList>){

      constexpr auto value = front_v<ValueList>;
      constexpr auto mask = front_v<MaskList>;

      if constexpr(value || mask){
        constexpr auto address = front_v<AddressesList>;
        
        if constexpr (isSet) _Set<address, value, mask>();
        else _Write<address, value, mask>();
      }
        
      using restValue = pop_front_t<ValueList>;
      using restMask = pop_front_t<MaskList>;
      using restAddress = pop_front_t<AddressesList>;
      
      _Modify<isSet, restAddress, restValue, restMask>();
    }
  };

protected:

  /*!
    @brief Set bits of registers
    @tparam <AddressesList> addresses of registers
    @tparam <SetList> values to set
    @tparam <MaskList> list of masks
  */
  template<typename AddressesList, typename SetList, typename MaskList = SetList>
  __FORCE_INLINE static void _Set(){
    _Modify<true, AddressesList, SetList, MaskList>();
  }

  /*!
    @brief Set bits of register
    @tparam <address> address of register
    @tparam <mask> mask of register
    @param [in] value to set
  */
  template<auto address, auto mask>
  __FORCE_INLINE static void _Set(decltype(mask) value){
      reg<address> = (reg<address> &(~mask)) | value;
  }

  /*!
    @brief Set bits of register
    @tparam <address> address of register
    @tparam <value> to set
  */
  template<auto address, auto value>
  __FORCE_INLINE static void _Set(){
      reg<address> |= value;
  }

  /*!
    @brief Set bits of register
    @tparam <address> address of register
    @tparam <value> to set
    @tparam <mask> mask of register
  */
  template<auto address, auto value, auto mask>
  __FORCE_INLINE static void _Set(){
      reg<address> = (reg<address> &(~mask)) | value;
  }

  /*!
    @brief Clear bits of register
    @tparam <address> address of register
    @tparam <value> to clear
  */
  template<auto address, auto value>
  __FORCE_INLINE static void _Clear(){
      reg<address> = reg<address> &(~value);
  }

  /*!
    @brief Clear bits of register
    @tparam <address> address of register
    @tparam <T> type of value
    @param [in] value to clear
  */
  template<auto address, typename T>
  __FORCE_INLINE static void _Clear(T value){
      reg<address> = reg<address> &(~value);
  }

  /*!
    @brief Write values to registers
    @tparam <AddressesList> addresses of registers
    @tparam <ValueList> values to write
    @tparam <MaskList> list of masks
  */
  template<typename AddressesList, typename ValueList, typename MaskList = ValueList>
  __FORCE_INLINE static void _Write(){
    _Modify<false, AddressesList, ValueList, MaskList>();
  }

  /*!
    @brief Write value to register
    @tparam <address> address of register
    @tparam <mask> mask of register
    @param [in] value to write
  */
  template<auto address, auto mask>
  __FORCE_INLINE static void _Write(decltype(mask) value){
      reg<address> = mask & value;
  }

  /*!
    @brief Write value to register
    @tparam <address> address of register
    @tparam <T> type of value
    @param [in] value to write
  */
  template<auto address, typename T>
  __FORCE_INLINE static void _Write(T value){
      static_assert(std::is_integral_v<T>, "value should be an integral");
      reg<address> = value;
  }

  /*!
    @brief Write value to register
    @tparam <address> address of register
    @tparam <value> to write
    @tparam <mask> mask of register
  */
  template<auto address, auto value, auto mask>  
  __FORCE_INLINE static void _Write(){
      reg<address> = mask & value;
  }

  /*!
    @brief Write value to register
    @tparam <address> address of register
    @tparam <value> to write
  */
  template<auto address, auto value>  
  __FORCE_INLINE static void _Write(){
      reg<address> = value;
  }

  template<typename listAddresses, typename T, typename ... Types>  
  __FORCE_INLINE static void _Write(T value, Types... values){
    if constexpr (!trait::is_empty_v<listAddresses>){
      using restAddresses = trait::pop_front_t<listAddresses>;
      static constexpr auto address = trait::front_v<listAddresses>;
      reg<address> = value;
      _Write<restAddresses, Types...>(values...);
    }
  }

  template<typename listAddresses, typename T>
  __FORCE_INLINE static void _Write(T value){
    static constexpr auto address = trait::front_v<listAddresses>;
    reg<address> = value;
  }

  template<auto... addresses, typename T, typename ... Types>
  __FORCE_INLINE static void _Write(T v, Types... values){
    using listAddresses = trait::Valuelist<addresses...>;
    _Write<listAddresses>(v, values...);
  }

  /*!
    @brief Read value of register
    @tparam <address> address of register
    @tparam <mask> of value
    @tparam <accessType> type of mask and access to value of register
  */
  template<auto address, auto mask, typename accessType = decltype(address)>
  __FORCE_INLINE static auto _Read(){ return mask & reg<address, accessType>; }

  /*!
    @brief Read value of register
    @tparam <address> address of register
    @tparam <accessType> type of mask and access to value of register
  */
  template<auto address, typename accessType = decltype(address)>
  __FORCE_INLINE static auto _Read(){ return reg<address, accessType>; }

  /*!
    @brief Read value of register
    @tparam <address> address of register
    @tparam <accessType> type of mask and access to value of register
    @tparam [in] mask of value
  */
  template<auto address, typename accessType = decltype(address)>
  __FORCE_INLINE static auto _Read(accessType mask){ return mask & reg<address, accessType>; }

};

}; // ! namespace controller::hardware

#endif // !_REGISTERS_HPP