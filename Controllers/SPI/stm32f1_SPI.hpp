//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Driver for SPI. STM32F1-series
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _STM32F1_SPI_HPP
#define _STM32F1_SPI_HPP

#include "stm32f1_SPI_Helper.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief  SPI1 interface
  @tparam <txBufferSize> size of tx buffer
  @tparam <txBufferSize> size of rx buffer
  @tparam <comm> communication type: via DMA or ISR
  @tparam <divisor> baud rate control
  @tparam <remap> remap pins of SPI
  @tparam <size> data frame format: 8/16 bit
  @tparam <format> frame format: MSB/LSB
  @tparam <mode> Polarity IDLE state and Phase capture edge
*/  
template<size_t txBufferSize, size_t rxBufferSize, 
         configuration::spi::communication comm = configuration::spi::communication::txDMA_rxDMA,
         configuration::spi::divisor divisor = configuration::spi::divisor::DIVISOR_128,
         configuration::spi::remap remap = configuration::spi::remap::None,
         configuration::spi::frame_size frame_size = configuration::spi::frame_size::FRAME_8_BIT,
         configuration::spi::frame_format frame_format = configuration::spi::frame_format::MSB,
         configuration::spi::mode mode = configuration::spi::mode::POL_0_PHA_FIRST>
class SPI1 : public helper::spi::Helper<SPI1<txBufferSize, rxBufferSize, comm, divisor, remap, frame_size, frame_format, mode>, 
                                        1, txBufferSize, rxBufferSize, comm, divisor, remap, frame_size, frame_format, mode>{
protected:

  using base = helper::spi::Helper<SPI1<txBufferSize, rxBufferSize, comm, divisor, remap, frame_size, frame_format, mode>, 
                                   1, txBufferSize, rxBufferSize, comm, divisor, remap, frame_size, frame_format, mode>;

  static constexpr uint32_t remapValue = remap == configuration::spi::remap::None ? 0 : 0x011;

  struct power{
    static constexpr uint32_t
      SPI1EN = 1 << 12,
      SPIEN = 0,
      DMAEN = 1;
  };

  static constexpr uint32_t dmaID = 1;
  static constexpr uint32_t channelDMATX = 3;
  static constexpr uint32_t channelDMARX = 2;

  static constexpr uint32_t baseAddress = 0x40013000;

  struct pins{
    using MOSI = std::conditional_t<remap == configuration::spi::remap::None, Pin::PA_7, Pin::PB_5>;
    using MISO = std::conditional_t<remap == configuration::spi::remap::None, Pin::PA_6, Pin::PB_4>;
    using CLCK = std::conditional_t<remap == configuration::spi::remap::None, Pin::PA_5, Pin::PB_3>;
  };

  struct irq{
    static constexpr uint32_t
      SPI = 35,
      DMATX = 13,
      DMARX = 12;
  };

  friend base;

};

/*!
  @brief  SPI2 interface
  @tparam <txBufferSize> size of tx buffer
  @tparam <txBufferSize> size of rx buffer
  @tparam <comm> communication type: via DMA or ISR
  @tparam <divisor> baud rate control
  @tparam <size> data frame format: 8/16 bit
  @tparam <format> frame format: MSB/LSB
  @tparam <mode> Polarity IDLE state and Phase capture edge
*/  
template<size_t txBufferSize, size_t rxBufferSize, 
         configuration::spi::communication comm = configuration::spi::communication::txDMA_rxDMA,
         configuration::spi::divisor divisor = configuration::spi::divisor::DIVISOR_128,
         configuration::spi::frame_size frame_size = configuration::spi::frame_size::FRAME_8_BIT,
         configuration::spi::frame_format frame_format = configuration::spi::frame_format::MSB,
         configuration::spi::mode mode = configuration::spi::mode::POL_0_PHA_FIRST>
class SPI2 : public helper::spi::Helper<SPI2<txBufferSize, rxBufferSize, comm, divisor, frame_size, frame_format, mode>, 
                                        2, txBufferSize, rxBufferSize, comm, divisor, configuration::spi::remap::None, 
                                        frame_size, frame_format, mode>{
protected:

  using base = helper::spi::Helper<SPI2<txBufferSize, rxBufferSize, comm, divisor, frame_size, frame_format, mode>, 
                                   2, txBufferSize, rxBufferSize, comm, divisor, configuration::spi::remap::None, 
                                   frame_size, frame_format, mode>;

  static constexpr uint32_t remapValue = 0;

  struct pins{
    using MOSI = Pin::PB_15;
    using MISO = Pin::PB_14;
    using CLCK = Pin::PB_13;
  };

  struct power{
    static constexpr uint32_t
      SPI1EN = 0,
      SPIEN = 1 << 14,
      DMAEN = 1;
  };

  static constexpr uint32_t dmaID = 1;
  static constexpr uint32_t channelDMATX = 5;
  static constexpr uint32_t channelDMARX = 4;

  static constexpr uint32_t baseAddress = 0x40003800;

  struct irq{
    static constexpr uint32_t
      SPI = 36,
      DMATX = 15,
      DMARX = 14;
  };

  friend base;

};

} // !namspace controller

#endif //!_STM32F1_SPI_HPP