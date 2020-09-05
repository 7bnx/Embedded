//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Common interface for controllers connection(UART, SPI...)
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _CONNECTION_HPP
#define _CONNECTION_HPP

#include <cstdint>
#include <cstddef>
#include "Circular_Buffer.hpp"
#include "Callback.hpp"


/*!
  @brief Controller's common interfaces
*/
namespace controller::common{

/*!
  @brief Interface for connection with Circular Buffer
  @tparam <type> of buffer
  @tparam <txSize> size of transmitter buffer
  @tparam <rxSize> size of receiver buffer
  @tparam <interface> of connection
*/
template<typename type, size_t txSize, size_t rxSize, typename interface>
class connection{
public:

  /*!
    @brief Write data to transmitter
    @param [in] data to send
    @return true, if tx buffer not overflowed
  */ 
  static bool Write(type data){
    bool isOverflow = txBuffer.Push(data);
    interface::Send();
    return isOverflow;
  }

  /*!
    @brief Write data to transmitter
    @param [in] pointer to data array
    @param [in] length of data to write
    @return true, if tx buffer not overflowed
  */ 
  static bool Write(const type* data, size_t length){
    bool isOverflow = txBuffer.Push(data, length);
    interface::Send();
    return isOverflow;
  }

  /*!
    @brief Write char-data to transmitter
    @param [in] pointer to char-data array
    @param [in] length of char-data to write
    @return true, if tx buffer not overflowed
  */ 
  static bool Write(const char* data, size_t length){
    bool isOverflow = txBuffer.Push(reinterpret_cast<const uint8_t*>(data), length);
    interface::Send();
    return isOverflow;
  }

  /*!
    @brief Check the tx buffer
    @return true if tx buffer is empty
  */
  __attribute__ ((always_inline)) inline
  static bool IsTxEmpty(){
    interface::CheckTxBuffer();
    return txBuffer.IsEmpty(); 
  }

  /*!
    @brief Return number of elements in tx buffer
  */
  __attribute__ ((always_inline)) inline
  static size_t GetTxCount(){
    interface::CheckTxBuffer();
    return txBuffer.GetCount(); 
  }

  /*!
    @brief Check the rx buffer
    @return true if rx buffer is empty
  */
  __attribute__ ((always_inline)) inline
  static bool IsRxEmpty(){
    interface::CheckRxBuffer();
    return rxBuffer.IsEmpty();
  }

  /*!
    @brief Return number of elements in rx buffer
  */
  __attribute__ ((always_inline)) inline
  static size_t GetRxCount(){
    interface::CheckRxBuffer();
    return rxBuffer.GetCount(); 
  }

  /*!
    @brief Read front element of rx buffer
  */
  __attribute__ ((always_inline)) inline
  static type Front(){ return rxBuffer.Front(); }

  /*!
    @brief Pop front element of rx buffer
  */ 
  static type Pop(){
    if(rxBuffer.GetCount() <= 1)
      isCallbackRxNotEmptyCalled = false;
    return rxBuffer.Pop();
  }

  /*!
    @brief Pop data to destination array
    @param [out] destination pointer to array
    @param [in] length to pop
    @return number of poped elements
  */ 
  static size_t Pop(type* destination, size_t length){
    if(rxBuffer.GetCount() <= length)
      isCallbackRxNotEmptyCalled = false;
    return rxBuffer.Pop(destination, length);
  }

  /*!
    @brief Get data from rx buffer by index
    @param [in] index of rx buffer
    @return data by index. If rx buffer is empty - return front value
  */
  __attribute__ ((always_inline)) inline
  static type GetAt(size_t index){
    return rxBuffer[index];
  }

  /*!
    @brief Callback for TX Idle state
  */
  static inline utils::Callback CallbackTxIdle;

  /*!
    @brief Callback for RX empty state
  */
  static inline utils::Callback CallbackRxNotEmpty;

  /*!
    @brief Callback for UART error
  */
  static inline utils::Callback CallbackError;

private:

  friend interface;
  static inline container::CircularBuffer<type, txSize> txBuffer;
  static inline container::CircularBuffer<type, rxSize> rxBuffer;

  static inline bool isCallbackRxNotEmptyCalled = false;
};

} // !namespace controller::common

#endif  // !_CONNECTION_HPP