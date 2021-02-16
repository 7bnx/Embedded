//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Driver for UART. STM32F1-series
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _STM32F1_UART_HPP
#define _STM32F1_UART_HPP

#include "stm32f1_UART_Helper.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief  UART1 interface
  @tparam <txBufferSize> size of tx buffer
  @tparam <txBufferSize> size of rx buffer
  @tparam <comm> communication type - via DMA or ISR. Default TX and RX via DMA
  @tparam <mode> default mode is none parity, 8 data-bits, 1 stop-bit
  @tparam <remap> remap pins of UART
*/  
template<size_t txBufferSize, size_t rxBufferSize, 
         configuration::uart::communication comm = configuration::uart::communication::txDMA_rxDMA,
         configuration::uart::mode mode = configuration::uart::mode::E_8_1, 
         configuration::uart::remap remap = configuration::uart::remap::None>
class UART1 : public helper::uart::Helper<UART1<txBufferSize, rxBufferSize, comm, mode, remap>, 
                          1, txBufferSize, rxBufferSize, comm, mode, remap>{
protected:

  using base = helper::uart::Helper<UART1<txBufferSize, rxBufferSize, comm, mode, remap>,
                    1, txBufferSize, rxBufferSize, comm, mode, remap>;

  static constexpr uint32_t remapValue = remap == configuration::uart::remap::None ? 0 : 0x211;

  static constexpr uint32_t dmaID = 1;
  static constexpr uint32_t channelDMATX = 4;
  static constexpr uint32_t channelDMARX = 5;

  static constexpr uint32_t baseAddress = 0x40013800;

  friend base;

  struct pins{
    using TX = std::conditional_t<remap == configuration::uart::remap::None, Pin::PA_9, Pin::PB_6>;
    using RX = std::conditional_t<remap == configuration::uart::remap::None, Pin::PA_10, Pin::PB_7>;
  };

  struct power{
    static constexpr uint32_t
      UART1EN = 1 << 14,
      UARTEN = 0,
      DMAEN = 1;
  };

  struct irq{
    static constexpr uint32_t
      UART = 37,
      DMATX = 14,
      DMARX = 15;
  };

  static_assert(remap != configuration::uart::remap::Full, "UART1 has no full remap");

};

/*!
  @brief  UART2 interface
  @tparam <txBufferSize> size of tx buffer
  @tparam <txBufferSize> size of rx buffer
  @tparam <comm> communication type - via DMA or ISR. Default TX and RX via DMA
  @tparam <mode> default mode is none parity, 8 data-bits, 1 stop-bit
  @tparam <remap> remap pins of UART
*/ 
template<size_t txBufferSize, size_t rxBufferSize, 
         configuration::uart::communication comm = configuration::uart::communication::txDMA_rxDMA,
         configuration::uart::mode mode = configuration::uart::mode::N_8_1, 
         configuration::uart::remap remap = configuration::uart::remap::None>
class UART2 : public helper::uart::Helper<UART2<txBufferSize, rxBufferSize, comm, mode, remap>, 
                          2, txBufferSize, rxBufferSize, comm, mode, remap>{
protected:

  using base = helper::uart::Helper<UART2<txBufferSize, rxBufferSize, comm, mode, remap>,
                    2, txBufferSize, rxBufferSize, comm, mode, remap>;

  static constexpr uint32_t remapValue = remap == configuration::uart::remap::None ? 0 : 0x311;

  static constexpr uint32_t dmaID = 1;
  static constexpr uint32_t channelDMATX = 7;
  static constexpr uint32_t channelDMARX = 6;

  static constexpr uint32_t baseAddress = 0x40004400;

  struct pins{
    using TX = std::conditional_t<remap == configuration::uart::remap::None, Pin::PA_2, Pin::PD_5>;
    using RX = std::conditional_t<remap == configuration::uart::remap::None, Pin::PA_3, Pin::PD_6>;
  };

  struct power{
    static constexpr uint32_t
      UART1EN = 0,
      UARTEN = 0x20000,
      DMAEN = 1;
  };

  struct irq{
    static constexpr uint32_t
      UART = 38,
      DMATX = 17,
      DMARX = 16;
  };

  friend base;

  static_assert(remap != configuration::uart::remap::Full, "UART2 has no full remap");

};

/*!
  @brief  UART3 interface
  @tparam <txBufferSize> size of tx buffer
  @tparam <txBufferSize> size of rx buffer
  @tparam <comm> communication type - via DMA or ISR. Default TX and RX via DMA
  @tparam <mode> default mode is none parity, 8 data-bits, 1 stop-bit
  @tparam <remap> remap pins of UART
*/ 
template<size_t txBufferSize, size_t rxBufferSize, 
         configuration::uart::communication comm = configuration::uart::communication::txDMA_rxDMA,
         configuration::uart::mode mode = configuration::uart::mode::N_8_1, 
         configuration::uart::remap remap = configuration::uart::remap::None>
class UART3 : public helper::uart::Helper<UART3<txBufferSize, rxBufferSize, comm, mode, remap>, 
                          3, txBufferSize, rxBufferSize, comm, mode, remap>{
protected:

  using base = helper::uart::Helper<UART3<txBufferSize, rxBufferSize, comm, mode, remap>,
                    3, txBufferSize, rxBufferSize, comm, mode, remap>;

  static constexpr uint32_t remapValue = remap == configuration::uart::remap::None ? 0 :
                                         remap == configuration::uart::remap::Partial  ? 0x411 : 0x412;

  static constexpr uint32_t dmaID = 1;
  static constexpr uint32_t channelDMATX = 2;
  static constexpr uint32_t channelDMARX = 3;

  static constexpr uint32_t baseAddress = 0x40004800;

  struct pins{
    using TX = std::conditional_t<remap == configuration::uart::remap::None,    Pin::PB_10,
               std::conditional_t<remap == configuration::uart::remap::Partial, Pin::PC_10, Pin::PD_8>>;
    using RX = std::conditional_t<remap == configuration::uart::remap::None, Pin::PB_11,
               std::conditional_t<remap == configuration::uart::remap::Partial, Pin::PC_11, Pin::PD_9>>;
  };

  struct power{
    static constexpr uint32_t
      UART1EN = 0,
      UARTEN = 0x40000,
      DMAEN = 1;
  };

  struct irq{
    static constexpr uint32_t
      UART = 39,
      DMATX = 12,
      DMARX = 13;
  };

  friend base;

};

} // !namspace controller

#endif //!_STM32F1_UART_HPP