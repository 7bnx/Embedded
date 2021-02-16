//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Common interface for controllers connection(UART, SPI...)
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _ICONNECTION_HPP
#define _ICONNECTION_HPP

#include <cstdint>
#include "../Common/Core/Interface.hpp"
#include "../Common/Compiler/Compiler.h"
#include "../../Containers/Circular_Buffer.hpp"

/*!
  @brief Controller's common interfaces
*/
namespace controller::interfaces{

/*!
  @brief Interface for connection with Circular Buffer
  @tparam <Type> of buffer
  @tparam <txSize> size of transmitter buffer
  @tparam <rxSize> size of receiver buffer
  @tparam <adapter> hardware interface of controller (UART, SPI ...)
*/
template<typename Type, size_t txSize, size_t rxSize, controller::interface interface_, typename adapter>
struct IConnection{

  /*!
    @brief Interface of connection
  */ 
  static constexpr auto interface = interface_;

  /*!
    @brief Write data to transmitter, stored in tx buffer
    @return true, if tx buffer not empty
  */ 
  static bool Write(){
    if (txBuffer.IsEmpty()) 
      return false;
    adapter::_Send();
    return true;
  }

  /*!
    @brief Write data to transmitter
    @param [in] data to send
    @return true, if tx buffer not overflowed
  */ 
  static bool Write(Type data){
    bool isOverflow = txBuffer.Push(data);
    adapter::_Send();
    return isOverflow;
  }

  /*!
    @brief Write data to transmitter
    @param [in] pointer to data array
    @param [in] size of data to write
    @return true, if tx buffer not overflowed
  */ 
  static bool Write(const Type* data, size_t size){
    bool isOverflow = txBuffer.Push(data, size);
    adapter::_Send();
    return isOverflow;
  }

  /*!
    @brief Write char-data to transmitter
    @param [in] pointer to char-data array
    @param [in] size of char-data to write
    @return true, if tx buffer not overflowed
  */ 
  static bool Write(const char* data, size_t size){
    bool isOverflow = txBuffer.Push(reinterpret_cast<const uint8_t*>(data), size);
    adapter::_Send();
    return isOverflow;
  }

  /*!
    @brief Push data to tx buffer
    @param [in] pointer to data array
    @param [in] size of data to Push
    @return true, if tx buffer not overflowed
  */ 
  static bool Push(const Type* data, size_t size){
    return txBuffer.Push(data, size);
  }

  /*!
    @brief Push data to tx buffer
    @param [in] data to send
    @return true, if tx buffer not overflowed
  */
  static bool Push(const Type data){
    return txBuffer.Push(data);
  }

  /*!
    @brief Fill tx buffer with element
    @param [in] element to Fill
    @param [in] size of elements to Fill
  */
  static void Push(const Type element, size_t size){
    txBuffer.Fill(element, size);
  }

  /*!
    @brief Write data from receiver (if not empty) to transmitter
  */ 
  static void Echo(){
    if (auto count = GetRxCount(); count){
      for(;count != 0; --count)
        txBuffer.Push(rxBuffer.Pop());
      adapter::_Send();
    }
  }

  /*!
    @brief Check the tx buffer
    @return true if tx buffer is empty
  */
  __FORCE_INLINE static bool IsTxEmpty(){
    adapter::_CheckTxBuffer();
    return txBuffer.IsEmpty(); 
  }

  /*!
    @brief Return number of elements in tx buffer
  */
  __FORCE_INLINE static size_t GetTxCount(){
    adapter::_CheckTxBuffer();
    return txBuffer.GetCount(); 
  }

  /*!
    @brief Check the rx buffer
    @return true if rx buffer is empty
  */
  __FORCE_INLINE static bool IsRxEmpty(){
    adapter::_CheckRxBuffer();
    return rxBuffer.IsEmpty();
  }

  /*!
    @brief Return number of elements in rx buffer
  */
  __FORCE_INLINE static size_t GetRxCount(){
    adapter::_CheckRxBuffer();
    return rxBuffer.GetCount(); 
  }

  /*!
    @brief Read front element of rx buffer
  */
  __FORCE_INLINE static auto Front(){ return rxBuffer.Front(); }

  /*!
    @brief Pop front element of rx buffer
  */ 
  __FORCE_INLINE static auto Read(){
    return rxBuffer.Pop();
  }

  /*!
    @brief Pop data to destination array
    @param [out] destination pointer to array
    @param [in] length of data to pop
    @return number of poped elements
  */ 
  __FORCE_INLINE static size_t Read(Type* destination, size_t length){
    return rxBuffer.Pop(destination, length);
  }

  /*!
    @brief Get element from rx buffer
    @param [in] index of element
    @return element by index. If rx buffer is empty - return front value
  */
  __FORCE_INLINE static Type GetAt(size_t index){
    return rxBuffer[index];
  }

  /*!
    @brief Flush tx buffer
  */
  __FORCE_INLINE static void FlushTX() { txBuffer.Flush(); }

  /*!
    @brief Flush rx buffer
  */
  __FORCE_INLINE static void FlushRX() { rxBuffer.Flush(); }

  /*!
    @brief Callback for TX Idle state
  */
  static inline void (*CallbackIdleTX)() = nullptr;

  /*!
    @brief Callback for RX not empty state
  */
  static inline void (*CallbackRxNotEmpty)() = nullptr;

  /*!
    @brief Callback for communication error
  */
  static inline void (*CallbackError)() = nullptr;

  /*!
    @brief type of buffers elements
  */
  using type = Type;

protected:

  static inline container::CircularBuffer<Type, txSize> txBuffer;
  static inline container::CircularBuffer<Type, rxSize> rxBuffer;

};

} // !namespace controller::interfaces

#endif  // !_ICONNECTION_HPP