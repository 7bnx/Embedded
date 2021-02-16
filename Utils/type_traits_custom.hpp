//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Custom type traits
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _TYPE_TRAITS_CUSTOM_HPP
#define _TYPE_TRAITS_CUSTOM_HPP

#include <cstddef>
#include <type_traits>

/*!
  @file
  @brief Traits for metaprogramming
*/

/*!
  @brief Custom traits.
*/
namespace trait{

/*-----------------------------------Basic----------------------------------------*/

/*!
  @brief Basic list of types
  @tparam Types parameter pack
*/
template<typename... Types>
struct Typelist{};

/*!
  @brief Basic list of values
  @tparam Values parameter pack
*/
template<auto... Values>
struct Valuelist{};

/*------------------------------End of Basic--------------------------------------*/


/*----------------------------------Front-------------------------------------------
  Description:  Get front type or value from list

  using listOfTypes = Typelist<int, short, bool, unsigned>;
  using listOfValues = Valuelist<1,2,3,4,5,6,1>;

  |-----------------|--------------------|----------|
  |      Trait      |    Parameters      |  Result  |
  |-----------------|--------------------|----------|
  |     front_t     |   <listOfTypes>    |    int   |
  |-----------------|--------------------|----------|
  |     front_v     |   <listOfValues>   |     1    |
  |-----------------|--------------------|----------| */


template<typename List>
struct front;

template<typename Head, typename... Tail>
struct front<Typelist<Head, Tail...>>{ 
  using type = Head; 
};

template<auto Head, auto... Tail>
struct front<Valuelist<Head, Tail...>> {
  static constexpr auto value = Head;
  using type = Valuelist<Head>;
};

template<typename List>
using front_t = typename front<List>::type;

template<typename List>
static constexpr auto front_v = front<List>::value;

/*----------------------------------End of Front----------------------------------*/


/*----------------------------------Pop_Front---------------------------------------
  Description:  Pop front type or value from list and return rest of the list

  using listOfTypes = Typelist<int, short, bool>;
  using listOfValues = Valuelist<1,2,3,4,5,6,1>;

  |-----------------|--------------------|------------------------|
  |      Trait      |    Parameters      |         Result         |
  |-----------------|--------------------|------------------------|
  |   pop_front_t   |    <listOfTypes>   | Typelist<short, bool>  |
  |-----------------|--------------------|------------------------|
  |   pop_front_t   |   <listOfValues>   | Valuelist<2,3,4,5,6,1> |
  |-----------------|--------------------|------------------------| */

template<typename List>
struct pop_front;

template<typename Head, typename... Tail>
struct pop_front<Typelist<Head, Tail...>> {
  using type = Typelist<Tail...>;
};

template<auto Head, auto... Tail>
struct pop_front<Valuelist<Head, Tail...>> {
  using type = Valuelist<Tail...>;
};


template<typename List>
using pop_front_t = typename pop_front<List>::type;

/*------------------------------End of Pop_Front----------------------------------*/

/*------------------------------Pop_Front_Number-----------------------------------
  Description:  Pop front number of type or value from list and return rest of the list

  using listOfTypes = Typelist<int, short, bool>;
  using listOfValues = Valuelist<1,2,3,4,5,6,1>;

  |------------------------|--------------------|-----------------|
  |          Trait         |    Parameters      |     Result      |
  |------------------------|--------------------|-----------------|
  |   pop_front_number_t   |  <2, listOfTypes>  | Typelist<bool>  |
  |------------------------|--------------------|------------ ----|
  |   pop_front_number_t   |  <5, listOfValues> | Valuelist<6,1>  |
  |------------------------|--------------------|-----------------| */

template<std::size_t number, typename List>
struct pop_front_number : public pop_front_number<number - 1, pop_front_t<List>>{};

template<typename List>
struct pop_front_number<0, List>{
  using type = List;
};

template<std::size_t number, typename List>
using pop_front_number_t = typename pop_front_number<number, List>::type;

/*-----------------------------End of Pop_Front_Number-----------------------------*/

/*----------------------------------Push_Front--------------------------------------
  Description:  Push new type or value to front of the list

  using listOfTypes = Typelist<short, bool>;
  using listOfValues = Valuelist<1,2,3,4,5,6,1>;

  |-----------------------|--------------------------|-------------------------------|
  |      Trait            |        Parameters        |             Result            |
  |-----------------------|--------------------------|-------------------------------|
  |     push_front_t      |   <listOfTypes, float>   | Typelist<float, short, bool>  |
  |-----------------------|--------------------------|-------------------------------|
  |  push_front_value_t   |    <7, listOfValues>     |  Valuelist<7,1,2,3,4,5,6,1>   |
  |-----------------------|--------------------------|-------------------------------| */

template<typename List, typename NewElement>
struct push_front;

template<typename... List, typename NewElement>
struct push_front<Typelist<List...>, NewElement> {
  using type = Typelist<NewElement, List...>;
};

template<auto... List, auto NewElement>
struct push_front<Valuelist<List...>, Valuelist<NewElement>>{
  using type = Valuelist<NewElement, List...>;
};

template<typename NewElement>
struct push_front<Typelist<>, NewElement> {
  using type = Typelist<NewElement>;
};

template<auto NewElement>
struct push_front<Valuelist<>, Valuelist<NewElement>> {
  using type = Valuelist<NewElement>;
};

template<auto NewElement>
struct push_front<Typelist<>, Valuelist<NewElement>> {
  using type = Valuelist<NewElement>;
};


template<typename List, typename NewElement>
using push_front_t = typename push_front<List, NewElement>::type;

template<typename List, auto NewElement>
using push_front_value_t = typename push_front<List, Valuelist<NewElement>>::type;

/*------------------------------End of Push_Front---------------------------------*/

/*----------------------------------Push_Back---------------------------------------
  Description:  Push new type or value to back of the list

  using listOfTypes = Typelist<short, bool>;
  using listOfValues = Valuelist<1,2,3,4,5,6>;

  |-----------------------|--------------------------|-------------------------------|
  |         Trait         |        Parameters        |             Result            |
  |-----------------------|--------------------------|-------------------------------|
  |      push_back_t      |    <listOfTypes, int>    |  Typelist<short, bool, int>   |
  |-----------------------|--------------------------|-------------------------------|
  |   push_back_value_t   |     <listOfValues, 0>    |    Valuelist<1,2,3,4,5,6,0>   |
  |-----------------------|--------------------------|-------------------------------| */

template<typename List, typename NewElement>
struct push_back;

template<typename... List, typename NewElement>
struct push_back<Typelist<List...>, NewElement>{
  using type = Typelist<List..., NewElement>;
};

template<auto... List, auto NewElement>
struct push_back<Valuelist<List...>, Valuelist<NewElement>>{
  using type = Valuelist<List..., NewElement>;
};

template<typename NewElement>
struct push_back<Typelist<>, NewElement>{
  using type = Typelist<NewElement>;
};

template<auto NewElement>
struct push_back<Valuelist<>, Valuelist<NewElement>>{
  using type = Valuelist<NewElement>;
};

template<auto NewElement>
struct push_back<Typelist<>, Valuelist<NewElement>>{
  using type = Valuelist<NewElement>;
};

template<typename List, typename NewElement>
using push_back_t = typename push_back<List, NewElement>::type;

template<typename List, auto NewElement>
using push_back_value_t = typename push_back<List, Valuelist<NewElement>>::type;

/*----------------------------------End of Push_Back------------------------------*/

/*----------------------------------Is_Unique---------------------------------------

  Description:  Check parameters list for uniqueness and return bool value
  using listOfTypes = Typelist<int, short, bool, unsigned>;
  using listOfValues = Valuelist<1,2,3,4,5,6,1>;

  |---------------------|--------------------|----------|
  |        Trait        |      Parameters    |  Result  |
  |---------------------|--------------------|----------|
  |     is_unique_v     | <int, short, bool> |   true   |
  |---------------------|--------------------|----------|
  |     is_unique_v     | <int, short, int>  |  false   |
  |---------------------|--------------------|----------|
  |     is_unique_v     |   <listOfTypes>    |   true   |
  |---------------------|--------------------|----------|
  |     is_unique_v     |   <listOfValues>   |  false   |
  |---------------------|--------------------|----------| */

template <typename...>
static constexpr auto is_unique_v = std::true_type{};

/*!
  @brief Check the uniqueness of the types. E.g.: is_unique_v<int, short, int>
*/
template <typename Head, typename... Tail>
static constexpr auto is_unique_v<Head, Tail...> =
  std::bool_constant<(!std::is_same_v<Head, Tail> && ...) && is_unique_v<Tail...>>{};

/*!
  @brief Check the uniqueness of the types-list 
    E.g.: using list = Typelist<int, short, bool>; is_unique_v<list>
*/  
template<typename... list>
static constexpr auto is_unique_v<Typelist<list...>> = is_unique_v<list...>;


/*------------------------------End of Is_Unique----------------------------------*/

/*-----------------------------------Is_Empty---------------------------------------
  Description:  Check parameters list for empty and return bool value

  using listOfTypes = Typelist<int, short, bool, unsigned>;
  using listOfValues = Valuelist<>;

  |-------------------------|--------------------|----------|
  |          Trait          |     Parameters     |  Result  |
  |-------------------------|--------------------|----------|
  |        is_empty_v       |    <listOfTypes>   |  false   |
  |-------------------------|--------------------|----------|
  |        is_empty_v       |   <listOfValues>   |   true   |
  |-------------------------|--------------------|----------| */

/*!
  @brief Check the emptiness of the types in parameters.   \n 
    E.g.: is_empty<int, short, bool>::value;
*/ 
template<typename List>
struct is_empty{
    static constexpr auto value = false;
};

/*!
  @brief Check the emptiness of the types in parameter. Specializatio for empty parameters   \n 
    E.g.: is_empty<>::value;
*/ 
template<>
struct is_empty<Typelist<>>{
    static constexpr auto value = true;
};

template<>
struct is_empty<Valuelist<>>{
    static constexpr auto value = true;
};


/*!
  @brief Check the emptiness of the types-list in parameter.   \n 
    E.g.: using list = Typelist<int, short, bool>; is_empty_v<list>;
*/ 
template<typename List>
static constexpr auto is_empty_v = is_empty<List>::value;

/*--------------------------------End of Is_Empty---------------------------------*/

/*------------------------------------Reverse----------------------------------------
  Description:  Reverse types or values in list

  using listOfTypes = Typelist<short, bool>;
  using listOfValues = Valuelist<1,2>;

  |-----------------------|--------------------------|-------------------------------|
  |         Trait         |        Parameters        |             Result            |
  |-----------------------|--------------------------|-------------------------------|
  |       reverse_t       |      <listOfTypes>       |     Typelist<bool, short>     |
  |-----------------------|--------------------------|-------------------------------|
  |       reverse_t       |      <listOfValues>      |        Valuelist<2, 1>        |
  |-----------------------|--------------------------|-------------------------------|  */

template<typename List, bool empty = is_empty_v<List>>
struct reverse;

template<typename List>
using reverse_t = typename reverse<List>::type;

template<typename List>
struct reverse<List, false> : public push_back<reverse_t<pop_front_t<List>>, front_t<List>> { };

template<typename List>
struct reverse<List, true>{
  using type = List;
};

template<typename List>
using reverse_t = typename reverse<List>::type;

/*--------------------------------End of Reverse-----------------------------------*/


/*----------------------------------Pop_Back----------------------------------------
  Description:  Pop types or values from back of list

  using listOfTypes = Typelist<float, short, bool, int>;
  using listOfValues = Valuelist<1, 2, 3>;

  |-----------------------|--------------------------|-------------------------------|
  |         Trait         |        Parameters        |             Result            |
  |-----------------------|--------------------------|-------------------------------|
  |      push_front_t     |      <listOfTypes>       | Typelist<float, short, bool>  |
  |-----------------------|--------------------------|-------------------------------| 
  |      push_front_t     |     <listOfValues>       |        Valuelist<1, 2>        |
  |-----------------------|--------------------------|-------------------------------|*/

template<typename List>
struct pop_back{
  using type = reverse_t<pop_front_t<reverse_t<List>>>;
};


template<typename List>
using pop_back_t = typename pop_back<List>::type;


/*--------------------------------End of Pop Back----------------------------------*/


/*------------------------------Pop_Back_Number--------------------------------------
  Description:  Pop number of types or values from back of list

  using listOfTypes = Typelist<float, short, bool, int, double>;
  using listOfValues = Valuelist<1, 2, 3>;

  |-----------------------|--------------------------|-------------------------------|
  |          Trait        |        Parameters        |             Result            |
  |-----------------------|--------------------------|-------------------------------|
  |   pop_back_number_t   |     <listOfTypes, 2>     |     Typelist<float, short>    |
  |-----------------------|--------------------------|-------------------------------|
  |   pop_back_number_t   |     <listOfValues, 1>    |        Valuelist<1, 2>        |
  |-----------------------|--------------------------|-------------------------------| */

template<std::size_t number, typename List>
struct pop_back_number{
  using type = reverse_t<pop_front_number_t<number, reverse_t<List>>>;
};

template<std::size_t number, typename List>
using pop_back_number_t = typename pop_back_number<number, List>::type;


/*----------------------------End of Pop Back Number------------------------------*/

/*---------------------------------Size_Of_List-------------------------------------
  Description:  Get size of list

  using listOfTypes = Typelist<int, float, double, bool>;

  |------------------|--------------------|----------|
  |       Trait      |     Parameters     |  Result  |
  |------------------|--------------------|----------|
  |  size_of_list_v  |     listOfTypes    |    4     |
  |------------------|--------------------|----------| */

template<typename List, std::size_t count = 0U>
struct size_of_list : public size_of_list<pop_front_t<List>, count + 1>{};

template<std::size_t count>
struct size_of_list<Typelist<>, count>{
  static constexpr std::size_t value = count;
};

template<std::size_t count>
struct size_of_list<Valuelist<>, count>{
  static constexpr std::size_t value = count;
};

template<typename List>
static constexpr std::size_t size_of_list_v = size_of_list<List>::value;

/*-------------------------------End Size_Of_List---------------------------------*/

/*-----------------------------------Is_Contain-------------------------------------
  Description:  Check if list contains type or value

  using listOfTypes = Typelist<int, short, bool, unsigned>;
  using listOfValues = Valuelist<7,8,9>;

  |-------------------------|--------------------|----------|
  |          Trait          |     Parameters     |  Result  |
  |-------------------------|--------------------|----------|
  |   is_contain_type_v     | <int, listOfTypes> |   true   |
  |-------------------------|--------------------|----------|
  |   is_contain_value_v    | <1, listOfValues>  |  false   |
  |-------------------------|--------------------|----------| */

template<typename Type, typename List, bool empty = is_empty_v<List>>
struct is_contain {
  static constexpr auto value = 
    ((std::is_same_v<Type, front_t<List>>) || is_contain<Type, pop_front_t<List>>::value);
};

template<auto Value, typename List>
struct is_contain<Valuelist<Value>, List, false>{
  static constexpr auto value = 
    ((Value == front_v<List>) || is_contain<Valuelist<Value>, pop_front_t<List>>::value);
};

template<typename Type>
struct is_contain<Type, Typelist<>, true> {
  static constexpr auto value = false;
};

template<auto Value>
struct is_contain<Valuelist<Value>, Valuelist<>, true> {
  static constexpr auto value = false;
};

template<typename Type, typename List>
static constexpr auto is_contain_type_v = is_contain<Type, List>::value;

template<auto Value, typename List>
static constexpr auto is_contain_value_v = is_contain<Valuelist<Value>, List>::value;

/*--------------------------------End of Is_Contain-------------------------------*/

/*---------------------------------Get_Element-------------------------------------
  Description:  Get type or value from list by number

  using listOfTypes = Typelist<int, float, double, bool>;
  using listOfValues = Valuelist<5, 6, 1, 2>;

  |------------------|--------------------|----------|
  |       Trait      |     Parameters     |  Result  |
  |------------------|--------------------|----------|
  |  get_element_t   |  <3,listOfTypes>   |   bool   |
  |------------------|--------------------|----------| 
  |  get_element_v   |  <0,listOfValues>  |    5     |
  |------------------|--------------------|----------|*/

template<std::size_t number, typename List>
struct get_element : public get_element<number - 1, pop_front_t<List>>{};

template<typename List>
struct get_element<0, List>{
  using type = front_t<List>;
  static constexpr auto value = front_v<List>;
};

template<std::size_t number, typename List>
using get_element_t = typename get_element<number, List>::type;

template<std::size_t number, typename List>
static constexpr auto get_element_v = get_element<number, List>::value;

/*-----------------------------End of Get_Element---------------------------------*/

/*-------------------------------------Remove---------------------------------------
  Description: Remove type or value from list

  using listOfTypes = Typelist<int, float, double, int, bool>;
  using listOfValues = Valuelist<5, 6, 1, 2>;

  |------------------|---------------------|-------------------------------|
  |       Trait      |      Parameters     |             Result            |
  |------------------|---------------------|-------------------------------|
  |     remove_t     |  <int, listOfTypes> | Typelist<float, double,bool>  |
  |------------------|---------------------|-------------------------------| 
  |  remove_value_t  |   <listOfValues,1>  |       Valuelist<5,6,2>        |
  |------------------|---------------------|-------------------------------|*/


template<typename Element, typename List, typename Result = Typelist<>, bool empty = is_empty_v<List>>
struct remove;

template<typename Element, typename List, typename Result>
struct remove<Element, List, Result, false>{
  using first = front_t<List>;
  using next = pop_front_t<List>;
  using result = std::conditional_t<std::is_same_v<first, Element>, Result, push_back_t<Result, first>>;
  using type = typename remove<Element, next, result>::type;
};

template<auto Element, typename List, typename Result>
struct remove<Valuelist<Element>, List, Result, false>{
  static constexpr auto first = front_v<List>;
  using next = pop_front_t<List>;
  using result = std::conditional_t<Element == first, Result, push_back_value_t<Result, first>>;
  using type = typename remove<Valuelist<Element>, next, result>::type;
};

template<typename Element, typename List, typename Result>
struct remove<Element, List, Result, true>{
  using type = Result;
};


template<typename Element, typename List>
using remove_t = typename remove<Element, List>::type;

template<auto Element, typename List>
using remove_value_t = typename remove<Valuelist<Element>, List>::type;

/*--------------------------------End of Remove------------------------------------*/

/*----------------------------------Make_Unique-------------------------------------
  Description: Makes list of unique types/values

  using listOfTypes = Typelist<int, float, double, bool, int, double>;

  |------------------|--------------------|-------------------------------------|
  |       Trait      |     Parameters     |                Result               |
  |------------------|--------------------|-------------------------------------|
  |  make_unique_t   |    <listOfTypes>   | Typelist<int, float, double, bool>  |
  |------------------|--------------------|-------------------------------------| */

template<typename List, typename Result = Typelist<>, bool empty = is_empty_v<List>>
struct make_unique;

template<typename List, typename Result>
struct make_unique<List, Result, false>{
  using first = front_t<List>;
  using rest = pop_front_t<List>;
  using remove = remove_t<first, rest>;
  using result = push_back_t<Result, first>;
  using type = typename make_unique<remove, result>::type;
};

template<auto... List, typename Result>
struct make_unique<Valuelist<List...>, Result, false>{
  static constexpr auto first = front_v<Valuelist<List...>>;
  using rest = pop_front_t<Valuelist<List...>>;
  using remove = remove_value_t<first, rest>;
  using result = push_back_value_t<Result, first>;
  using type = typename make_unique<remove, result>::type;
};

template<typename List, typename Result>
struct make_unique<List, Result, true>{
  using type = Result;
};


template<typename List>
using make_unique_t = typename make_unique<List>::type;

/*-----------------------------End of Make_Unique---------------------------------*/

/*---------------------------------Lists Operation--------------------------------*/

  /*Description: Operations with lists of values

  using list1 = Valuelist<1, 4, 8, 16>;
  using list2 = Valuelist<1, 5, 96, 17>;

  |------------------------------|-------------------|---------------------------|
  |               Trait          |    Parameters     |           Result          |
  |------------------------------|-------------------|---------------------------|
  |     lists_termwise_and_t     |  <list1, list2>   |  Valuelist<1, 4, 0, 16>   |
  |------------------------------|-------------------|---------------------------|
  |     lists_termwise_or_t      |  <list1, list2>   |  Valuelist<1, 5, 104, 17> |
  |---------------------------- -|-------------------|---------------------------|
  |     lists_termwise_xor_t     |  <list1, list2>   |  Valuelist<0, 1, 104, 1>  |
  |------------------------------|-------------------|---------------------------| */

// Operation termwise

template<template <auto value1, auto value2> typename operation, 
         typename List1, typename List2, typename Result = Valuelist<>>
struct operation_2_termwise_valuelists{
  constexpr static auto newValue = operation<front_v<List1>, front_v<List2>>::value;
  using nextList1 = pop_front_t<List1>;
  using nextList2 = pop_front_t<List2>;
    
  using result = push_back_value_t<Result, newValue>;
  using type = typename 
      operation_2_termwise_valuelists<operation, nextList1, nextList2, result>::type;
};

template<template <auto value1, auto value2> typename operation, typename Result>
struct operation_2_termwise_valuelists<operation, Valuelist<>, Valuelist<>, Result>{
  using type = Result;
};

template<template <auto value1, auto value2> typename operation, 
         typename List2, typename Result>
struct operation_2_termwise_valuelists<operation, Valuelist<>, List2, Result>{
  using type = typename 
      operation_2_termwise_valuelists<operation, Valuelist<0>, List2, Result>::type;
};

template<template <auto value1, auto value2> typename operation, 
         typename List1, typename Result>
struct operation_2_termwise_valuelists<operation, List1, Valuelist<>, Result>{
  using type = typename 
      operation_2_termwise_valuelists<operation, List1, Valuelist<0>, Result>::type;
};

template<auto value1, auto value2>
struct and_operation{ static constexpr auto value = value1 & value2;};

template<auto value1, auto value2>
struct or_operation{ static constexpr auto value = value1 | value2;};

template<auto value1, auto value2>
struct xor_operation{ static constexpr auto value = value1 ^ value2;};

template<auto value1, auto value2>
struct non_zero_then_or_operation{ static constexpr auto value = value1 != 0 ? value1 | value2 : value1;};

template<auto value1, auto valueShift>
struct shift_right_operation{ static constexpr auto value = value1 >> valueShift;};

template<auto value1, auto valueADD>
struct add_operation{ static constexpr auto value = value1 + valueADD;};

template<auto value1, auto valueMUL>
struct mul_operation{ static constexpr auto value = value1*valueMUL;};

// End of Operation termwise

// Operation Expand

template<typename T1, typename T2>
struct operation_expand{
  using type = Typelist<T1, T2>;
};

template<auto V1, auto V2>
struct operation_expand<Valuelist<V1>, Valuelist<V2>>{
  using type = Valuelist<V1, V2>;
};

template<typename... T1, typename... T2>
struct operation_expand<Typelist<T1...>, Typelist<T2...>>{
  using type = Typelist<T1..., T2...>;
};

template<auto... V1, auto... V2>
struct operation_expand<Valuelist<V1...>, Valuelist<V2...>>{
  using type = Valuelist<V1..., V2...>;
};

// End of Operation Expand

// ValueList Operation

template<template <auto value1, auto value2> class operation, typename List, bool isEnd = size_of_list_v<List> == 1>
class valuelist_operation{
  static constexpr auto value1 = front_v<pop_front_t<List>>;
  static constexpr auto result = operation<value1, front_v<List>>::value;
  using next = push_front_value_t<pop_front_t<pop_front_t<List>>, result>;
public:
  static constexpr auto value = valuelist_operation<operation, next>::value;
};

template<template <auto value1, auto valu2> class operation, typename List>
class valuelist_operation<operation, List, true>{
public:
  static constexpr auto value = front_v<List>;
};

// End of Valuelist Operation

//

template<template <auto value, auto operationValue> class operation, typename List, auto operationValue, typename Result = Valuelist<>>
class valuelist_operations{
  static constexpr auto value = front_v<List>;
  using result = push_back_value_t<Result, operation<value, operationValue>::value>;
  using restList = pop_front_t<List>;
public:
  using type = typename valuelist_operations<operation, restList, operationValue, result>::type;
};

template<template <auto value, auto operationValue> class operation, auto operationValue, typename Result>
class valuelist_operations<operation, Valuelist<>, operationValue, Result>{
public:
  using type = Result;
};

//
template<template<typename first, typename second> class operation,
         typename Lists, bool isEnd = size_of_list_v<Lists> == 1>
class lists_operation{

  using first = front_t<Lists>;
  using second = front_t<pop_front_t<Lists>>;
  using next = pop_front_t<pop_front_t<Lists>>;
  using result = operation<first, second>;

public:

  using type = typename lists_operation<operation, push_front_t<next, result>>::type;

};

template<template<typename first, typename second> class operation,
         typename Lists>
class lists_operation<operation, Lists, true>{
public:
  using type = front_t<Lists>;
};

template<typename List1, typename List2>
using operation_and_termwise_t = typename 
    operation_2_termwise_valuelists<and_operation, List1, List2>::type;

template<typename List1, typename List2>
using operation_or_termwise_t = typename 
    operation_2_termwise_valuelists<or_operation, List1, List2>::type;

template<typename List1, typename List2>
using operation_xor_termwise_t = typename 
    operation_2_termwise_valuelists<xor_operation, List1, List2>::type;

template<typename T1, typename T2>
using operation_expand_t = typename operation_expand<T1, T2>::type;


template<typename... Lists>
using lists_termwise_and_t = 
    typename lists_operation<operation_and_termwise_t, Typelist<Lists...>>::type;

template<typename... Lists>
using lists_termwise_or_t = typename 
    lists_operation<operation_or_termwise_t, Typelist<Lists...>>::type;

template<typename... Lists>
using lists_termwise_xor_t = typename 
    lists_operation<operation_xor_termwise_t, Typelist<Lists...>>::type;

template<typename... Lists>
using lists_expand_t = typename lists_operation<operation_expand_t, Typelist<Lists...>>::type;

template<typename List>
static constexpr auto valuelist_or_v = valuelist_operation<or_operation, List>::value; 

template<typename List>
using valuelist_or_t = Valuelist<valuelist_operation<or_operation, List>::value>; 

template<auto valueShift, typename List>
using valuelist_shift_right_t = typename valuelist_operations<shift_right_operation, List, valueShift>::type;

template<auto valueADD, typename List>
using valuelist_add_t = typename valuelist_operations<add_operation, List, valueADD>::type;

template<auto valueMUL, typename List>
using valuelist_mul_t = typename valuelist_operations<mul_operation, List, valueMUL>::type;

template<typename List1, typename List2>
using operation_non_zero_then_or_t = typename 
    operation_2_termwise_valuelists<non_zero_then_or_operation, List1, List2>::type;

/*--------------------------------End of Lists Operation----------------------------*/

/*-------------------------------------Sort-----------------------------------------*/

/*Description: Sort list

  using listValues = Valuelist<1, 5, 96, 17>;

  |--------------------------|--------------------|------------------------|
  |           Trait          |     Parameters     |         Result         |
  |--------------------------|--------------------|------------------------|
  | sort_insertion_smaller_t |    <listValues>    |  Valuelist<1,5,17,96>  |
  |--------------------------|--------------------|------------------------| 
  | sort_insertion_greater_t |    <listValues>    |  Valuelist<96,17,5,1>  |
  |--------------------------|--------------------|------------------------| */

template<typename List, typename Element, template<typename T, typename U> class compare, 
         bool = is_empty_v<List>>
class insert_sorted;

template<typename List, typename Element, template<typename T, typename U> class compare>
using insert_sorted_t = typename insert_sorted<List, Element, compare>::type;

template<typename List, typename Element, template<typename T, typename U> class compare>
class insert_sorted<List, Element, compare, false>{
  
  using newTail = typename std::conditional_t<compare<Element, front_t<List>>::value, List, insert_sorted_t<pop_front_t<List>, Element, compare>>;
  using newHead = typename std::conditional_t<compare<Element, front_t<List>>::value, Element, front_t<List>>;
public:
  using type = push_front_t<newTail, newHead>;
  
};

template<typename List, typename Element, template<typename T, typename U> class compare>
class insert_sorted<List, Element, compare, true> : public push_front<List, Element>{};


template<typename List, template<typename T, typename U> class compare, bool = is_empty_v<List>>
class sort_insertion;

template<typename List, template<typename T, typename U> class compare>
using sort_insertion_t = typename sort_insertion<List, compare>::type;

template<typename List, template<typename Т, typename U> class compare>
class sort_insertion<List, compare, false>{
public:
  using type = insert_sorted_t<sort_insertion_t<pop_front_t<List>, compare>, front_t<List>, compare>;
};

template<typename List, template<typename Т, typename U> class compare>
class sort_insertion<List, compare, true>{
public:
  using type = List;
};


template<typename T, typename U>
struct smaller_then{
  static constexpr bool value = sizeof(T) < sizeof(U) ;
};

template<auto T, auto U>
struct smaller_then<Valuelist<T>, Valuelist<U>>{
  static constexpr bool value = T < U;
};

template<typename T, typename U>
struct greater_then{
  static constexpr bool value = sizeof(T) > sizeof(U) ;
};

template<auto T, auto U>
struct greater_then<Valuelist<T>, Valuelist<U>>{
  static constexpr bool value = T > U;
};


template<typename List>
using sort_insertion_smaller_t = sort_insertion_t<List, smaller_then>;

template<typename List>
using sort_insertion_greater_t = sort_insertion_t<List, greater_then>;

/*----------------------------------End of Sort-------------------------------------*/

/*------------------------------Generate Valuelist----------------------------------*/

/*Description: Generates custom valuelist from value

  |----------------------|--------------------|------------------------|
  |          Trait       |     Parameters     |         Result         |
  |----------------------|--------------------|------------------------|
  | generate_valuelist_t |       <3, 2>       |    Valuelist<2,2,2>    |
  |----------------------|--------------------|------------------------| */

template<std::size_t number, auto startValue, auto increment, typename Result = Valuelist<>>
class generate_valuelist{
  using result = push_back_value_t<Result, startValue>;
public:
  using type = typename generate_valuelist<number - 1, startValue + increment, increment, result>::type;
};

template<auto startValue, auto increment, typename Result>
class generate_valuelist<0, startValue, increment, Result>{
public:
  using type = Result;
};

template<std::size_t number, auto startValue, auto increment>
using generate_valuelist_with_increment_t = typename generate_valuelist<number, startValue, increment>::type;

template<std::size_t number, auto Value>
using generate_valuelist_t = typename generate_valuelist<number, Value, 0>::type;



/*----------------------------End of Generate Valuelist-----------------------------*/

/*-----------------------------------Replace----------------------------------------*/

/*Description: Replace types or values from one list to another

  using list1 = Valuelist<1, 4, 8, 16>;
  using list2 = Valuelist<7, 5>;

  |-----------------|--------------------|------------------------|
  |      Trait      |     Parameters     |         Result         |
  |-----------------|--------------------|------------------------|
  |    replace_t    |  <2,list1, list2>  | Valuelist<1, 4, 7, 5>  |
  |-----------------|--------------------|------------------------| */

template<std::size_t startIndex, typename List, typename Replace>
class replace{
  static constexpr auto sizeList = size_of_list_v<List>;
  static constexpr auto sizeReplace = size_of_list_v<Replace>;
  static constexpr auto numberPopFront = sizeList >= startIndex ? sizeList - startIndex : 0;
  static constexpr auto numberPopBack = (startIndex + sizeReplace) >= sizeList ? sizeList : startIndex + sizeReplace;
  using frontList = pop_back_number_t<numberPopFront, List>;
  using backList = pop_front_number_t<numberPopBack, List>;
public:
  using type = lists_expand_t<frontList, Replace, backList>;
};

template<std::size_t startIndex, typename List, typename Replace>
using replace_t = typename replace<startIndex, List, Replace>::type;

/*-------------------------------------Replace--------------------------------------*/

} // !namespace trait

#endif //!_TYPE_TRAITS_CUSTOM_HPP