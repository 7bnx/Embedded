//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Addition to <type_traits> for simplify metaprogramming
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _TYPE_TRAITS_CUSTOM_HPP
#define _TYPE_TRAITS_CUSTOM_HPP

#include <cstdint>
#include <type_traits>

/*!
  @file
  @brief Traits for metaprogramming
*/

/*!
  @brief Namespace for utils.
*/
namespace utils{

/*-----------------------------------Basic----------------------------------------*/

/*!
  @brief Basic list of types
  @tparam Types variadic parameter
*/
template<typename... Types>
struct Typelist{};

/*!
  @brief Basic list of values
  @tparam Values variadic parameter
*/
template<auto... Values>
struct Valuelist{};

/*!
  @brief Basic type to store value
  @tparam<Value> to store
*/
template<auto Value>
struct ctv {
  static constexpr auto value = Value;
};

/*---------------------------------End of Basic-----------------------------------*/


/*----------------------------------Front-------------------------------------------
  Description:  Pop front type or value from list

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

template<>
struct front<Typelist<>>{
  using type = Typelist<>;
};

template<auto Head, auto... Tail>
struct front<Valuelist<Head, Tail...>> {
  using type = ctv<Head>;
  static constexpr auto value = Head;
};

template<typename List>
using front_t = typename front<List>::type;

template<typename List>
static constexpr auto front_v = front<List>::value;

/*----------------------------------End of Front----------------------------------*/


/*----------------------------------Pop_Front---------------------------------------
  Description:  Pop front type or value from list

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
struct pop_front{
  using type = Typelist<>;
};

template<typename Head, typename... Tail>
struct pop_front<Typelist<Head, Tail...>> {
  using type = Typelist<Tail...>;
};

template<typename... List>
struct pop_front<Typelist<List...>> {
  using type = Typelist<List...>;
};

template<>
struct pop_front<Typelist<>> {
  using type = Typelist<>;
};

template<auto Head, auto... Tail>
struct pop_front<Valuelist<Head, Tail...>> {
  using type = Valuelist<Tail...>;
};

template<typename List>
using pop_front_t = typename pop_front<List>::type;

/*------------------------------End of Pop_Front----------------------------------*/


/*----------------------------------Push_Front--------------------------------------
  Description:  Push new element to front of the list

  using listOfTypes = Typelist<short, bool>;
  using listOfValues = Valuelist<1,2,3,4,5,6>;

  |-----------------------|--------------------------|-------------------------------|
  |      Trait            |        Parameters        |             Result            |
  |-----------------------|--------------------------|-------------------------------|
  |      push_front_t     |   <listOfTypes, float>   | Typelist<float, short, bool>  |
  |-----------------------|--------------------------|-------------------------------|
  |   push_front_value_t  |     <listOfValues, 0>    |    Valuelist<0,1,2,3,4,5,6>   |
  |-----------------------|--------------------------|-------------------------------| */

template<typename List, typename NewElement>
struct push_front;

template<typename... List, typename NewElement>
struct push_front<Typelist<List...>, NewElement> {
  using type = Typelist<NewElement, List...>;
};

template<typename List>
struct push_front<List, Typelist<>> {
  using type = List;
};

template<typename... List>
struct push_front<Typelist<List...>, Typelist<>> {
  using type = Typelist<List...>;
};

template<>
struct push_front<Typelist<>, Typelist<>> {
  using type = Typelist<>;
};

template<auto... Values, auto New>
struct push_front<Valuelist<Values...>, ctv<New>>{
  using type = Valuelist<New, Values...>;
};

template<auto... Values, auto...New>
struct push_front<Valuelist<Values...>, Valuelist<New...>>{
  using type = Typelist<Valuelist<New...>, Valuelist<Values...>>;
};

template<typename List, typename NewElement>
using push_front_t = typename push_front<List, NewElement>::type;

template<typename List, auto NewElement>
struct push_front_value;

template<auto... List, auto NewElement>
struct push_front_value<Valuelist<List...>, NewElement>{
  using type = Valuelist<NewElement, List...>;
};

template<auto NewElement>
struct push_front_value<Valuelist<>, NewElement>{
  using type = Valuelist<NewElement>;
};

template<typename List, auto NewElement>
using push_front_value_t = typename push_front_value<List, NewElement>::type;

/*------------------------------End of Push_Front---------------------------------*/


/*----------------------------------Push_Back---------------------------------------
  Description:  Push new element to back of the list

  using listOfTypes = Typelist<short, bool>;
  using listOfValues = Valuelist<1,2,3,4,5,6>;

  |-----------------------|--------------------------|-------------------------------|
  |      Trait            |        Parameters        |             Result            |
  |-----------------------|--------------------------|-------------------------------|
  |      push_back_t      |   <listOfTypes, float>   | Typelist<short, bool, float>  |
  |-----------------------|--------------------------|-------------------------------|
  |   push_back_value_t   |     <listOfValues, 0>    |    Valuelist<1,2,3,4,5,6,0>   |
  |-----------------------|--------------------------|-------------------------------| */

template<typename List, typename NewElement>
struct push_back{
  using type = Typelist<List, NewElement>;
};

template<typename List>
struct push_back<List, Typelist<>>{
  using type = Typelist<List>;
};

template<typename... List>
struct push_back<Typelist<List...>, Typelist<>>{
  using type = Typelist<List...>;
};

template<>
struct push_back<Typelist<>, Typelist<>>{
  using type = Typelist<>;
};

template<typename... Elements, typename NewElement>
struct push_back<Typelist<Elements...>, NewElement>{
  using type = Typelist<Elements..., NewElement>;
};

template<typename List, typename NewElement>
using push_back_t = typename push_back<List, NewElement>::type;

template<typename List, auto NewElement>
struct push_back_value;

template<auto... List, auto NewElement>
struct push_back_value<Valuelist<List...>, NewElement>{
  using type = Valuelist<List..., NewElement>;
};

template<auto NewElement>
struct push_back_value<Valuelist<>, NewElement>{
  using type = Valuelist<NewElement>;
};

template<typename List, auto NewElement>
using push_back_value_t = typename push_back_value<List, NewElement>::type;

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
  |---------------------|--------------------|----------|
  |  is_unique_value_v  |   <1,2,3,4,5,6>    |   true   |
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
  @brief Check the uniqueness of the types-list. \n 
    E.g.: using list = Typelist<int, short, bool>; is_unique_v<list>
*/  
template<typename... list>
static constexpr auto is_unique_v<Typelist<list...>> = is_unique_v<list...>;

/*!
  @brief Check the uniqueness of the values-list. \n 
    E.g.: using list = Valuelist<1,2,3,4,5>; is_unique_v<list>
*/
template <auto...values>
static constexpr auto is_unique_v<Valuelist<values...>> = std::true_type{};

/*!
  @brief Check the uniqueness of the values-list. \n 
    E.g.: using list = Valuelist<1,2,3,4,5>; is_unique_v<list>
*/
template <auto Head, auto... Tail>
static constexpr auto is_unique_v<Valuelist<Head, Tail...>> = 
  std::bool_constant<((Head != Tail)  && ...) && is_unique_v<Valuelist<Tail...>>>{};

/*!
  @brief Check the uniqueness of the numbers.    \n
    E.g.: is_unique_numbers_v<1,2,3,4>
*/    
template<auto... values>
static constexpr auto is_unique_value_v = is_unique_v<Valuelist<values...>>;

/*------------------------------End of Is_Unique----------------------------------*/


/*-----------------------------------Is_Empty---------------------------------------
  Description:  Check parameters list for empty and return bool value

  using listOfTypes = Typelist<int, short, bool, unsigned>;
  using listOfValues = Valuelist<>;

  |-------------------------|--------------------|----------|
  |          Trait          |     Parameters     |  Result  |
  |-------------------------|--------------------|----------|
  |     is_empty::value     |         <>         |   true   |
  |-------------------------|--------------------|----------|
  |     is_empty::value     | <int, short, int>  |  false   |
  |-------------------------|--------------------|----------|
  |     is_empty::value     |    <listOfTypes>   |  false   |
  |-------------------------|--------------------|----------|
  |        is_empty_v       |   <listOfValues>   |   true   |
  |-------------------------|--------------------|----------|
  |  is_empty_valuelist_v   |   <1,2,3,4,5,6>    |  false   |
  |-------------------------|--------------------|----------| */

/*!
  @brief Check the emptiness of the types in parameters.   \n 
    E.g.: is_empty<int, short, bool>::value;
*/ 
template<typename... types>
struct is_empty{
    static constexpr auto value = sizeof... (types) == 0;
};

/*!
  @brief Check the emptiness of the types in parameter. Specializatio for empty parameters   \n 
    E.g.: is_empty<>::value;
*/ 
template<>
struct is_empty<Typelist<>>{
    static constexpr auto value = true;
};

/*!
  @brief Check the emptiness of the values-list in parameter.   \n 
    E.g.: using list = Valuelist<1, 2, 3>; is_empty<list>::value;
*/ 
template<auto... values>
struct is_empty<Valuelist<values...>>{
    static constexpr auto value = sizeof... (values) == 0;
};

/*!
  @brief Check the emptiness of the types-list in parameter.   \n 
    E.g.: using list = Typelist<int, short, bool>; is_empty_v<list>;
*/ 
template<typename... types>
static constexpr auto is_empty_v = is_empty<types...>::value;

template<>
struct is_empty<Typelist<Valuelist<>>>{
    static constexpr auto value = true;
};

/*!
  @brief Check the emptiness of the numbers in parameter.   \n 
    E.g.: is_empty_valuelist_v<1,2,3,4>;
*/ 
template<auto... values>
static constexpr auto is_empty_valuelist_v = 
  is_empty<Valuelist<values...>>::value;

/*--------------------------------End of Is_Empty---------------------------------*/


/*-----------------------------------Is_Contain-------------------------------------
  Description:  Check if list contains type or value

  using listOfTypes = Typelist<int, short, bool, unsigned>;
  using listOfValues = Valuelist<7,8,9>;

  |-------------------------|--------------------|----------|
  |          Trait          |     Parameters     |  Result  |
  |-------------------------|--------------------|----------|
  |   is_contain_type_v     | <int, listOfTypes> |   true   |
  |-------------------------|--------------------|----------|
  |  is_contain_value_v     | <1, listOfValues>  |  false   |
  |-------------------------|--------------------|----------| */

template<typename Type, typename List, bool empty = is_empty_v<List>>
struct is_contain {
  static constexpr auto value = 
    ((std::is_same_v<Type, front_t<List>>) || is_contain<Type, pop_front_t<List>>::value);
};

template<typename Type>
struct is_contain<Type, Typelist<>, true> {
  static constexpr auto value = false;
};

template<typename Type, typename List>
static constexpr auto is_contain_type_v = is_contain<Type, List>::value;

template<auto Value, typename List, bool empty = is_empty_v<List>>
struct is_contain_number {
  static constexpr auto value = 
    ((Value == front_v<List>) || is_contain_number<Value, pop_front_t<List>>::value);
};

template<auto Value>
struct is_contain_number<Value, Valuelist<>, true> {
  static constexpr auto value = false;
};

template<auto Value, typename List>
static constexpr auto is_contain_number_v = is_contain_number<Value, List>::value;

/*--------------------------------End of Is_Contain-------------------------------*/


/*-----------------------------------Make_Unique------------------------------------
  Description:  Make unique list of types or values

  using listOfTypes = Typelist<bool, int, int, short>;
  using listOfValues = Valuelist<7, 7, 8, 9, 8, 7>;

  |-----------------|----------------|----------------------------------|
  |      Trait      |   Parameters   |              Result              | 
  |-----------------|----------------|----------------------------------|
  |  make_unique_t  | <listOfTypes>  |    Typelist<bool, int, short>    |
  |-----------------|----------------|----------------------------------|
  |  make_unique_t  | <listOfValues> | Typelist<ctv<7>, ctv<8>, ctv<9>> |
  |-----------------|----------------|----------------------------------| */


template<typename Types, bool empty = is_empty_v<Types>>
struct make_unique{
private:
  struct NONE;
  using next = pop_front_t<Types>;
  using first = front_t<Types>;
  using add = std::conditional_t<is_contain_type_v<first, next>, NONE, first>;
  using rest = typename make_unique<next>::type;
public:
  using type = std::conditional_t<std::is_same_v<NONE, add>, rest, push_front_t<rest, add>>;
};

template<auto... Values>
struct make_unique<Valuelist<Values...>, false>{
  struct NONE;
  using next = pop_front_t<Valuelist<Values...>>;
  static constexpr auto first = front_v<Valuelist<Values...>>;
  using add = std::conditional_t<is_contain_number_v<first, next>, NONE, ctv<first>>;
  using rest = typename make_unique<next>::type;
public:
  using type = std::conditional_t<std::is_same_v<NONE, add>, rest, push_front_t<rest, add>>;
};

template<typename Types>
struct make_unique<Types, true>{
public:
  using type = Typelist<>;
};

template<typename List>
using make_unique_t = typename make_unique<List>::type;


/*-------------------------------End of Make_Unique-------------------------------*/


/*-----------------------------------Expand-----------------------------------------
  Description:  Check parameters list for empty and return bool value

  using list1 = Typelist<int, short>;
  using list2 = Typelist<float, double>

  |------------|----------------|-------------------------------------|
  |    Trait   |   Parameters   |               Result                |
  |------------|----------------|-------------------------------------|
  |  expand_t  | <list1, list2> | Typelist<int, short, float, double> |
  |------------|----------------|-------------------------------------| */

template<typename List, bool empty = utils::is_empty_v<List>>
struct expand;

template<>
struct expand<utils::Typelist<>, true>{
  using type = utils::Typelist<>;
};

template<typename... List>
struct expand<Typelist<List...>, false>{
private:
  template<typename P1, typename P2>
  struct add{
    using type = Typelist<P1, P2>;
  };

  template<typename P1, typename... P2>
  struct add<Typelist<P2...>, P1>{
    using type = Typelist<P1, P2...>;
  };

  template<typename... P1, typename... P2>
  struct add<Typelist<P1...>, Typelist<P2...>>{
    using type = Typelist<P2..., P1...>;
  };

  using front = front_t<Typelist<List...>>;
  using next = pop_front_t<Typelist<List...>>;
  using rest = typename expand<next>::type;
public:
  using type = typename add<rest, front>::type;

};

template<typename... List>
using expand_t = typename expand<Typelist<List...>>::type;


template<typename List, bool empty = utils::is_empty_v<List>>
struct expand_value;

template<>
struct expand_value<utils::Typelist<>, true>{
  using type = utils::Valuelist<>;
};

/*-------------------------------End of Expand------------------------------------*/


/*----------------------------------Is_Greater--------------------------------------
  Description:  Check if value is greater then values in list

  using listOfValues = Valuelist<7,8,9>;

  |----------------|--------------------|----------|
  |      Trait     |      Parameters    |  Result  |
  |----------------|--------------------|----------|
  |  is_greater_v  | <10, listOfValues> |   true   |
  |----------------|--------------------|----------| */


template<auto Value, typename List>
static constexpr auto is_greater_v = Value > front_v<List> && is_greater_v<Value, pop_front_t<List>>;

template<auto Value>
static constexpr auto is_greater_v<Value, Valuelist<>> = true;

/*-------------------------------End of Is_Greater--------------------------------*/


/*-----------------------------------Is_Lower---------------------------------------
  Description:  Check if value is lower then values in list

  using listOfValues = Valuelist<7,8,9>;

  |--------------|--------------------|----------|
  |     Trait    |     Parameters     |  Result  |
  |--------------|--------------------|----------|
  |  is_lower_v  | <10, listOfValues> |  false   |
  |--------------|--------------------|----------| */


template<auto Value, typename List>
static constexpr auto is_lower_v = Value < front_v<List> && is_lower_v<Value, pop_front_t<List>>;

template<auto Value>
static constexpr auto is_lower_v<Value, Valuelist<>> = true;

/*-------------------------------End of Is_Exceed---------------------------------*/


/*---------------------------------Size_Of_List-------------------------------------
  Description:  Get number of types in list

  using listOfTypes = Typelist<int, float, double, bool>;

  |------------------|--------------------|----------|
  |       Trait      |     Parameters     |  Result  |
  |------------------|--------------------|----------|
  |  size_of_list_v  |     listOfTypes    |    4     |
  |------------------|--------------------|----------| */


template<typename List>
static constexpr uint32_t size_of_list_v = is_empty_v<List> ? 0U : 1U + 
                 size_of_list_v<pop_front_t<List>>;

/*-------------------------------End Size_Of_List---------------------------------*/


} // !namespace utils

#endif //!_TYPE_TRAITS_CUSTOM_HPP