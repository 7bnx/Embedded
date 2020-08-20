//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  UART driver 
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _UART_HPP
#define _UART_HPP

#include <cstdint>
#include <cstddef>
#include "controller_define.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief UART driver
  @tparam <baseUART> base address of UART
  @tparam <txBufferSize> size of tx buffer
  @tparam <txBufferSize> size of rx buffer
  @tparam <isTXviaDMA> if true, then data transmits via DMA, otherwise via TX ISR
  @tparam <isRXviaDMA> if true, then data receives via DMA, otherwise via RX ISR
  @tparam <isPinsRemap> if true, then TX and RX pins will be remaped
*/  
template<uint32_t baseUART, size_t txBufferSize, size_t rxBufferSize, 
         bool isTXviaDMA = true, bool isRXviaDMA = true, bool isPinsRemap = false>
class UART: public hardware::_UARTImplementation<baseUART, txBufferSize, rxBufferSize, 
                                                 isTXviaDMA, isRXviaDMA,isPinsRemap>{

private:

};

} //!namespace controller

#endif //!UART_HPP