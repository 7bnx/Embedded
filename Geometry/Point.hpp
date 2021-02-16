//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Point structures
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _POINT_HPP
#define _POINT_HPP

#include <cstddef>
#include <array>
#include "Compiler.h"

/*!
  @brief Namespace for geometry
*/
namespace geometry{

/*!
  @brief Multi-dimensional point
  @tparam <T> point's type
  @tparam <Dimensions> number of dimensions
*/
template<typename T, size_t Dimensions>
struct Point{

  /*!
    @brief Array with values of dimesions
  */ 
  std::array<T, Dimensions> values;

  /*!
    @brief Constructor
    @param [in] args values of dimensions
  */
  template <typename ... Args>
  Point(const Args& ... args): values{ args... }{}

  /*!
    @brief Set all dimensions to 0
  */
  __FORCE_INLINE void Reset(){
    for(auto v : values)
      v = 0;
  }

  /*!
    @brief Number of point's dimensions
  */
  const size_t size = Dimensions;
};

/*!
  @brief Two-dimensional point
  @tparam <T> point's type
*/
template<typename T>
struct Point2D: public Point<T, 2>{
  using base = Point<T, 2>;

  /*!
    @brief Constructor
    @param [in] x value of X-dimension
    @param [in] y value of Y-dimension
  */
  Point2D(const T& x = 0, const T& y = 0): base(x, y) {  }

  /*!
    @brief Set values of x- and y- dimensions
    @param [in] x value of X-dimension
    @param [in] y value of Y-dimension
  */
  __FORCE_INLINE void Set(const T & x = 0, const T & y = 0){
    base::values[0] = x;
    base::values[1] = y;
  }

  /*!
    @brief Set value of X-dimensions
  */
  __FORCE_INLINE void SetX(const T & x = 0){ base::values[0] = x; }

  /*!
    @brief Set value of Y-dimensions
  */
  __FORCE_INLINE void SetY(const T & y = 0){ base::values[1] = y; }

  /*!
    @brief Get value of X-dimensions
  */
  __FORCE_INLINE const auto & GetX() const{ return base::values[0]; }

  /*!
    @brief Get value of Y-dimensions
  */
  __FORCE_INLINE const auto & GetY() const{ return base::values[1]; }
};

template<typename T>
struct Point3D: public Point<T, 3>{
  using base = Point<T, 3>;

  /*!
    @brief Set values of x-, y- and z- dimensions
    @param [in] x value of X-dimension
    @param [in] y value of Y-dimension
    @param [in] y value of Z-dimension
  */
  Point3D(const T& x = 0, const T& y = 0, const T& z = 0) : base(x, y, z) {  }

  /*!
    @brief Constructor
    @param [in] x value of X-dimension
    @param [in] y value of Y-dimension
    @param [in] y value of Z-dimension
  */
  __FORCE_INLINE void Set(const T & x = 0, const T & y = 0, const T & z = 0){
    base::values[0] = x;
    base::values[1] = y;
    base::values[2] = z;
  }

  /*!
    @brief Set value of X-dimensions
  */
  __FORCE_INLINE void SetX(const T & x = 0){ base::values[0] = x; }

  /*!
    @brief Set value of Y-dimensions
  */
  __FORCE_INLINE void SetY(const T & y = 0){ base::values[1] = y; }

  /*!
    @brief Set value of Z-dimensions
  */
  __FORCE_INLINE void SetZ(const T & z = 0){ base::values[2] = z; }

  /*!
    @brief Get value of X-dimensions
  */
  __FORCE_INLINE const auto & GetX() const{ return base::values[0]; }

  /*!
    @brief Get value of Y-dimensions
  */
  __FORCE_INLINE const auto & GetY() const{ return base::values[1]; }

  /*!
    @brief Get value of Z-dimensions
  */
  __FORCE_INLINE const auto & GetZ() const{ return base::values[2]; }
};

} // !namespace geometry

#endif // !_POINT_HPP