#ifndef _HD44780_CPP
#define _HD44780_CPP

#include "HD44780.hpp"

#define PARAMETERS uint8_t rowNumber, uint8_t columnNumber, typename interface
#define CLASS HD44780<rowNumber, columnNumber, interface>

const uint16_t delayStartUpInMS = 15; 

const uint16_t delayPowerOnInUS[] = {5000, 160, 160}; 

const uint16_t delayStrobeInNS = 450;

const uint16_t delayInstructionInUS = 100;

const uint16_t delayClearHomeInUS = 4100;

const uint8_t rowBaseAddress[] = {0x00, 0x40, 0x14, 0x54};

const uint16_t maskStrobePin = 0x100;
const uint16_t maskDataPin = 0x200;

const size_t commandsPowerOnNumber = 3;
const size_t commandsInitInstructionNumber = 3;

template<bool is8bitMode>
const uint16_t commandPowerOn = 0x13 << (is8bitMode*4);
const uint16_t commandEnable4BitMode = 0x12;
const uint8_t commandClear = 0x01; // Clear displat and set cursor to Home
const uint8_t commandCursorDirection = 0x06; // Increment cursor after write
template<bool is8bitMode, uint8_t rowNumber> // Set 8 bit mode or 4 bit   // Set row number 
const uint8_t commandInterfaceLength = (is8bitMode ? 0x30 : 0x20) | ((rowNumber + 1) << 3) & 8;
const uint8_t commandSetPosition = 0x80;
const uint8_t commandEnableDisplayCursor = 0x0C; // Turn display On
const uint8_t commandCursorMask = 1;
const uint8_t commandCursorBlinkMask = 0;
template<bool is8bitMode, uint8_t rowNumber>
const uint16_t commandsInit[commandsInitInstructionNumber] = {
                                                    maskStrobePin | commandInterfaceLength<is8bitMode, rowNumber>,
                                                    maskStrobePin | commandCursorDirection,
                                                    maskStrobePin | commandEnableDisplayCursor
                                                  };


namespace device{

/*!
  @brief Inizialization of HD44780
*/ 
template<PARAMETERS>
void CLASS::Init(){
  controller::Systick::Delay_ms(delayStartUpInMS);

  for(size_t i = 0; i < commandsPowerOnNumber; ++i){
    WriteViaParallel(commandPowerOn<is8bitMode>);
    controller::Systick::Delay_us(delayPowerOnInUS[i]);
  }

  if constexpr(!is8bitMode){
    WriteViaParallel(commandEnable4BitMode);
    controller::Systick::Delay_us(delayInstructionInUS);
  }

  for(size_t i = 0; i < commandsInitInstructionNumber; ++i)
    Write(commandsInit<is8bitMode, rowNumber>[i]);

  Clear();

}

/*!
  @brief Clear LCD
*/ 
template<PARAMETERS>
void CLASS::Clear(){
  Fill(' ');
}

/*!
  @brief Return cursor position to Home
*/ 
template<PARAMETERS>
void CLASS::Home(){
  SetPosition(0,0);
}

/*!
  @brief Set cursor position
  @param [in] column
  @param [in] row
*/
template<PARAMETERS>
bool CLASS::SetPosition(uint8_t x, uint8_t y){
  if(y >= rowNumber || x >= columnNumber) return false;
  uint16_t command = maskStrobePin | commandSetPosition | (x + rowBaseAddress[y]);
  Write(command);
  currentPosition.Set(x, y);
  return true;
}

/*!
  @brief Enable cursor
  @param [in] isOn enable cursor
  @param [in] isBlink set blink of cursor 
*/ 
template<PARAMETERS>
void CLASS::EnableCursor(bool isOn, bool isBlink){
  uint16_t command = maskStrobePin | 
                      commandEnableDisplayCursor | 
                      isOn << commandCursorMask | 
                      isBlink <<commandCursorBlinkMask;
  Write(command);
}

/*!
  @brief Fill LCD
  @param [in] symbol to fill LCD
*/
template<PARAMETERS>
void CLASS::Fill(char symbol){
  currentPosition.Reset();
  SetPosition(0,0);
  uint8_t y = 0;
  for (int i = 0; i < columnNumber * rowNumber; ++i){
    Write(maskDataPin | maskStrobePin | symbol);
    if ((i + 1) % columnNumber == 0){
      y = (y + 1) % rowNumber;
      SetPosition(0, y);
    }
  }
}

/*!
  @brief Print data
  @param [in] data to print
*/
template<PARAMETERS>
void CLASS::Print(char data){
  Write(maskDataPin | maskStrobePin | data);
  auto[x, y] = currentPosition;
  x = (x + 1) % (columnNumber);
  if (x == 0){
    y = (y + 1) % rowNumber;
    SetPosition(0, y);
  }
  currentPosition.Set(x, y);
}

/*!
  @brief Print array of data
  @param [in] data to print
  @param [in] length to print
*/ 
template<PARAMETERS>
void CLASS::Print(const uint8_t* data, size_t length){
  size_t startIndex = length > (columnNumber * rowNumber) ? length - columnNumber * rowNumber : 0U;
  auto[x, y] = currentPosition;
  size_t xValueToNextRow = columnNumber - x + startIndex;
  for(size_t i = startIndex; i < length; ++i){
    Write(maskDataPin | maskStrobePin | *(data + i));
    if (i + 1 >= xValueToNextRow){
      xValueToNextRow += columnNumber;
      y = (y + 1) % (rowNumber);
      SetPosition(0, y);
    }
  }
  x = (x + (length - startIndex)) % (columnNumber);
  currentPosition.Set(x, y);
}

/*!
  @brief Print array of data
  @param [in] data to print
  @param [in] length to print
*/
template<PARAMETERS>
void CLASS::Print(const char* data, size_t length){
  Print(reinterpret_cast<const uint8_t*>(data), length);
}

template<PARAMETERS>
template<typename _interface>
void CLASS::WriteViaParallel(uint16_t data){
  _interface::Write(data);
  controller::Systick::Delay_ns(delayStrobeInNS);
  interface::template Write<interface::size - 2>(false);
}

template<PARAMETERS>
void CLASS::Write(uint16_t data){
  if constexpr (connection == controller::interface::parallel){
    uint16_t dataToWrite = is8bitMode ? data : data >> 4;
    WriteViaParallel<interface>(dataToWrite);
    if constexpr (!is8bitMode){
      dataToWrite = 0x10 | data;
      using dataPins = typename interface::template pop_front_pins_t<1>;
      WriteViaParallel<dataPins>(dataToWrite);
    }
    controller::Systick::Delay_us(delayInstructionInUS);
  } else if constexpr (connection == controller::interface::i2c){

  }
}

} // !namespace device

#undef PARAMETERS
#undef CLASS

#endif // !_HD44780_CPP