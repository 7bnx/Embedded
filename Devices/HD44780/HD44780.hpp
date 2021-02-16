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
#include <cstddef>
#include <array>
#include "../../Controllers/Common/Compiler/Compiler.h"
#include "../../Controllers/Common/Core/Interface.hpp"

/*!
  @file
  @brief HD44780 Driver
*/

/*!
  @brief Namespace for devices
*/
namespace device{

/*!
  @brief Class of LCD HD44780.
  @tparam <numberRow> number of rows in LCD (E.g.: 1,2,4...)
  @tparam <numberColumn> number of columns in LCD (E.g.: 16, 20...)
  @tparam <adapter> interface to send data (Pinlist<RS,E,D7,D6,D5,D4,D3,D2,D1,D0>)
  @tparam <timer> instance for delay. Should implement: Delay_ms, Delay_us, Delay_ns
*/  
template<size_t numberRow, size_t numberColumn, typename adapter, typename timer>
class HD44780{
public:

  /*!
    @brief Initialization of HD44780
  */ 
  static void Init();

  /*!
    @brief Clear LCD
  */
  __FORCE_INLINE static void Clear(){ Fill('-'); }

  /*!
    @brief Return cursor position to Home
  */
  __FORCE_INLINE static void Home(){ SetPosition(); }

  /*!
    @brief Set cursor position
    @param [in] column
    @param [in] row
  */ 
  static bool SetPosition(size_t column = 0, size_t row = 0);

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
  static void Fill(char symbol = ' ');

  /*!
    @brief Print data
    @param [in] data to print
  */ 
  static void Print(char data);

  /*!
    @brief Print array of data
    @param [in] data to print
    @param [in] length of data
  */ 
  static void Print(const uint8_t* data, size_t length);

  /*!
    @brief Print array of char
    @param [in] data to print
  */
  static void Print(const char* data);

  /*!
    @brief Print array of char
    @param [in] data to print
    @param [in] length of chars
  */
  static void Print(const char* data, size_t length);

private:

  static constexpr bool _Is8BitMode();

  static inline size_t currentColumn = 0;
  static inline size_t currentRow = 0;
  static constexpr size_t valueMaxChars = numberColumn * numberRow;
  static constexpr auto interface = adapter::interface;

  static constexpr std::array<uint16_t, 3> delayPowerOnUs = { 5000, 160, 160 };
  static constexpr uint16_t delayStartUpInMS = 15; 
  static constexpr uint16_t delayStrobeInNS = 500;
  static constexpr uint16_t delayDataInUS = 30;
  static constexpr uint16_t delayCommandInUS = 100;

  static constexpr uint16_t maskCommandWrite = 0x100;
  static constexpr uint16_t maskDataWrite = 0x300;
  static constexpr uint16_t maskStrobe4BitMode = 0x10;

  static constexpr uint16_t commandPowerOn = 0x13 << (_Is8BitMode() ? 4 : 0);
  static constexpr uint16_t commandEnableDisplayCursor = 0x0C; // Turn display On
  static constexpr uint16_t commandDisableDisplay = 0x08; // Turn display Off
  static constexpr uint16_t commandEnable4BitMode = 0x12;
  static constexpr uint16_t commandCursorDirection = 0x06; // Increment cursor after write
  static constexpr uint16_t commandSetPosition = 0x80;
  static constexpr uint16_t commandCursorMask = 2;
  static constexpr uint16_t commandCursorBlinkMask = 1;
  static constexpr std::array<uint16_t, 4> commandsInit = {
    maskCommandWrite | (_Is8BitMode() ? 0x30 : 0x20) | ((numberRow + 1) << 3) & 8, // Interface mode and number of rows
    maskCommandWrite | commandDisableDisplay,
    maskCommandWrite | commandCursorDirection, // Increment cursor after write
    maskCommandWrite | commandEnableDisplayCursor};

  static constexpr std::array<uint16_t, 4> addressRow = {0x00, 0x40, 0x14, 0x54};

  template<typename adapter_ = adapter>
  static void _WriteViaParallel(uint16_t data);
  
  template<bool isCommand>
  static void _Write(uint16_t data);

  static constexpr bool _IsParallelValid();

  static_assert(_IsParallelValid(),
                "Pinlist should consists 10 or 6 pins (for 8-bit and 4-bit mode)");

};

} // !namespace device

#include "HD44780.cpp"

#endif // !_HD44780_HPP