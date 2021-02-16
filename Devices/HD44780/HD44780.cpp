#ifndef _HD44780_CPP
#define _HD44780_CPP

#include "HD44780.hpp"

#define PARAMETERS size_t numberRow, size_t numberColumn, typename adapter, typename timer
#define CLASS HD44780<numberRow, numberColumn, adapter, timer>

namespace device{

/*!
  @brief Initialization of HD44780
*/ 
template<PARAMETERS>
void CLASS::Init(){
  timer::Delay_ms(delayStartUpInMS);

  for(auto delay : delayPowerOnUs){
    _WriteViaParallel(commandPowerOn);
    timer::Delay_us(delay);
  }

  if constexpr(!_Is8BitMode()){
    _WriteViaParallel(commandEnable4BitMode);
    timer::Delay_us(delayCommandInUS);
  }
  
  for(auto command : commandsInit)
    _Write<true>(command);

  Clear();
}

/*!
  @brief Set cursor position
  @param [in] column
  @param [in] row
*/
template<PARAMETERS>
bool CLASS::SetPosition(size_t column, size_t row){
  if (row >= numberRow || column >= numberColumn) return false;
  uint16_t command = maskCommandWrite | commandSetPosition | (column + addressRow[row]);
  _Write<true>(command);
  currentColumn = column;
  currentRow = row;
  return true;
}

/*!
  @brief Enable cursor
  @param [in] isOn enable cursor
  @param [in] isBlink set blink of cursor 
*/ 
template<PARAMETERS>
void CLASS::EnableCursor(bool isOn, bool isBlink){
  uint16_t command =  maskCommandWrite | commandEnableDisplayCursor | 
                      (isOn ? commandCursorMask : 0) | (isBlink ?  commandCursorBlinkMask : 0);
  _Write<true>(command);
}

/*!
  @brief Fill LCD
  @param [in] symbol to fill LCD
*/
template<PARAMETERS>
void CLASS::Fill(char symbol){
  currentColumn = currentRow = 0;
  SetPosition(0,0);
  for (size_t i = 0; i < valueMaxChars; ++i)
    Print(maskDataWrite | symbol);
}

/*!
  @brief Print data
  @param [in] data to print
*/
template<PARAMETERS>
void CLASS::Print(char data){
  _Write<false>(maskDataWrite | data);
  currentColumn = (currentColumn + 1) % numberColumn;
  if (currentColumn == 0){
    currentRow = (currentRow + 1) % numberRow;
    SetPosition(0, currentRow);
  }
}

/*!
  @brief Print array of data
  @param [in] data to print
  @param [in] length to print
*/ 
template<PARAMETERS>
void CLASS::Print(const uint8_t* data, size_t length){
  length = (length > valueMaxChars) ? length - valueMaxChars : length;
  for (size_t i = 0; i < length; ++i)
    Print(maskDataWrite | *(data + i));
}

/*!
  @brief Print array of data
  @param [in] data to print
  @param [in] length to print
*/
template<PARAMETERS>
void CLASS::Print(const char* data){
  while(*data != '\0')
    Print(maskDataWrite | *(data++));
}

/*!
  @brief Print array of char
  @param [in] data to print
  @param [in] length of chars
*/
template<PARAMETERS>
void CLASS::Print(const char* data, size_t length){
  Print(reinterpret_cast<const uint8_t *>(data), length);
}

template<PARAMETERS>
template<typename adapter_>
void CLASS::_WriteViaParallel(uint16_t data){
  using pinStrobe = typename adapter_::template get_pin<adapter::size - 2>;
  adapter_::template Write(data);
  timer::Delay_ns(delayStrobeInNS);
  pinStrobe::Low();
}

template<PARAMETERS>
template<bool isCommand>
void CLASS::_Write(uint16_t data){
  if constexpr (interface == controller::interface::PARALLEL){
    uint16_t dataToWrite = _Is8BitMode() ? data : data >> 4;
    _WriteViaParallel(dataToWrite);
    if constexpr (!_Is8BitMode()){
      dataToWrite = maskStrobe4BitMode | data;
      using pins4BitMode = typename adapter::template pop_front_pins<1>;
      _WriteViaParallel<pins4BitMode>(dataToWrite);
    }
  } else if constexpr (interface == controller::interface::I2C){
  }
  timer::Delay_us(isCommand ? delayCommandInUS : delayDataInUS);
}

template<PARAMETERS>
constexpr bool CLASS::_Is8BitMode(){
  if constexpr (interface == controller::interface::PARALLEL)
    if constexpr (adapter::size != 10) return false;
  return true;
}

template<PARAMETERS>
constexpr bool CLASS::_IsParallelValid(){
  if constexpr (interface == controller::interface::PARALLEL){
    if constexpr (adapter::size == 10 || adapter::size == 6) return true;
    else return false;
  }
  return true;
}

} // !namespace device

#undef PARAMETERS
#undef CLASS

#endif // !_HD44780_CPP