//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  HD44780 LCD controller
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _HD44780_HPP
#define _HD44780_HPP

#include <cstdint>
#include "interface.hpp"
#include "point.hpp"
#include "systick.hpp"

/*!
  @file
  @brief HD44780 Driver
*/

/*!
  @brief Namespace for devices.
*/
namespace device{

/*!
  @brief Class of LCD HD44780.
  @tparam rowNumber number of rows in LCD (E.g.: 1,2,4...)
  @tparam columnNumber number of columns in LCD (E.g.: 16, 20...)
  @tparam interface to send data (Pinlist<RS,E,D7,D6,D5,D4,D3,D2,D1,D0>)
*/  
template<uint8_t rowNumber, uint8_t columnNumber, typename interface>
class HD44780{
public:

  /*!
    @brief Initialization of HD44780
  */ 
  static void Init();

  /*!
    @brief Clear LCD
  */
  __attribute__ ((always_inline)) inline 
  static void Clear();

  /*!
    @brief Return cursor position to Home
  */
  __attribute__ ((always_inline)) inline 
  static void Home();

  /*!
    @brief Set cursor position
    @param [in] column
    @param [in] row
  */ 
  static bool SetPosition(uint8_t column, uint8_t row);

  /*!
    @brief Return current cursor position 
  */
  __attribute__ ((always_inline)) inline 
  static const auto& GetPosition(){ return currentPosition; }

  /*!
    @brief Enable cursor
    @param [in] isOn enable cursor
    @param [in] isBlink set blink of cursor 
  */ 
  static void EnableCursor(bool isOn = false, bool isBlink = false);

  /*!
    @brief Fill LCD
    @param [in] symbol to fill LCD
  */
  static void Fill(char symbol);

  /*!
    @brief Print data
    @param [in] data to print
  */ 
  static void Print(char data);

  /*!
    @brief Print array of data
    @param [in] data to print
    @param [in] length to print
  */ 
  static void Print(const uint8_t* data, size_t length);

  /*!
    @brief Print array of data
    @param [in] data to print
    @param [in] length to print
  */
  static void Print(const char* data, size_t length);

private:

  static inline utils::point2D<uint8_t> currentPosition = {0, 0};

  static constexpr bool is8bitMode = interface::size == 10 ? true : false;
  static constexpr auto connection = interface::interface;

  template<typename _interface = interface>
  static void WriteViaParallel(uint16_t data);
  
  static void Write(uint16_t data);

};

} // !namespace device

#include "HD44780.cpp"

#endif // !_HD44780_HPP