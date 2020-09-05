//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Point structures
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _POINT_HPP
#define _POINT_HPP

namespace utils{

/*!
  @brief Two - dimension point
  @tparam <T> type of point
*/  
template<typename T>
struct point2D{

  T x;
  T y;

  /*!
    @brief Set value to point
    @param [in] xValue set value of x
    @param [in] yValue set value of y
  */ 
  __attribute__ ((always_inline)) inline  
  void Set(T xValue = 0, T yValue = 0){x = xValue; y = yValue;}
  
  /*!
    @brief Reset point value to 0 
  */
  __attribute__ ((always_inline)) inline   
  void Reset(){Set();}
};

/*!
  @brief Three - dimension point
  @tparam <T> type of point
*/  
template<typename T>
struct point3D{

  T x;
  T y;
  T z;

  /*!
    @brief Set value to point
    @param [in] xValue set value of x
    @param [in] yValue set value of y
    @param [in] zValue set value of z
  */
  __attribute__ ((always_inline)) inline   
  void Set(T xValue = 0, T yValue = 0, T zValue = 0){x = xValue; y = yValue; z = zValue;}

  /*!
    @brief Reset point value to 0 
  */
  __attribute__ ((always_inline)) inline  
  void Reset(){Set();}
};

} // !namespace utils

#endif // !_POINT_HPP