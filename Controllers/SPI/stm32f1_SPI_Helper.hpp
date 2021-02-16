//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Helper for SPI of STM32F1
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _STM32F1_SPI_HELPER_HPP
#define _STM32F1_SPI_HELPER_HPP

#include "../Common/Compiler/Compiler.h"
#include "../Interfaces/IPower.hpp"
#include "../Interfaces/IConnection.hpp"
#include "../Common/Core/Interrupt.hpp"
#include "../Clock/stm32f1_Clock.hpp"
#include "../Pinlist/stm32f1_Pinlist.hpp"
#include "../DMA/stm32f1_DMA.hpp"

/*!
  @brief Configuration for SPI
*/
namespace controller::configuration::spi{

/*!
  @brief Remap of pins
*/ 
enum class remap{
  /*! @brief Pins not remaped*/ 
  None,
  /*! @brief Full remap of pins*/ 
  Full
};

/*!
  @brief Baud rate control
*/
enum class divisor{
  /*! @brief Clock / 2*/
  DIVISOR_2  = (0 << 3),
  /*! @brief Clock / 4*/
  DIVISOR_4  = (1 << 3),
  /*! @brief Clock / 8*/
  DIVISOR_8  = (2 << 3),
  /*! @brief Clock / 16*/
  DIVISOR_16  = (3 << 3),
  /*! @brief Clock / 32*/
  DIVISOR_32  = (4 << 3),
  /*! @brief Clock / 64*/
  DIVISOR_64  = (5 << 3),
  /*! @brief Clock / 128*/
  DIVISOR_128  = (6 << 3),
  /*! @brief Clock / 256*/
  DIVISOR_256  = (7 << 3)
};

/*!
  @brief Clock phase and polarity
*/
enum class mode{
  /*! @brief CLCK is 0 when idle, the first clock transition is the first data capture edge*/
  POL_0_PHA_FIRST  = 0,
  /*! @brief CLCK is 1 when idle, the first clock transition is the first data capture edge*/
  POL_1_PHA_FIRST  = 4,
  /*! @brief CLCK is 0 when idle, the second clock transition is the first data capture edge*/
  POL_0_PHA_SECOND  = 0,
  /*! @brief CLCK is 1 when idle, the second clock transition is the first data capture edge*/
  POL_1_PHA_SECOND  = 4
};

/*!
  @brief Data frame format: 8/16 bit
*/
enum class frame_size{
  /*! @brief 8-bit data frame format is selected for transmission/reception*/
  FRAME_8_BIT  = 0,
  /*! @brief 16-bit data frame format is selected for transmission/reception*/
  FRAME_16_BIT  = 0x800
};

/*!
  @brief Frame format: MSB/LSB
*/
enum class frame_format{
  /*! @brief MSB transmitted first*/
  MSB  = 0,
  /*! @brief LSB transmitted first*/
  LSB  = 0x80
};

/*!
  @brief SPI communication: via DMA or ISR
*/ 
enum class communication{
  /*! @brief Transmitt - DMA, Receive - DMA*/
  txDMA_rxDMA = 3,
  /*! @brief Transmitt - ISR, Receive - ISR*/
  txISR_rxISR = 0x40,
  /*! @brief Transmitt - DMA, Receive - ISR*/
  txDMA_rxISR = 0x42,
  /*! @brief Transmitt - ISR, Receive - DMA*/
  txISR_rxDMA = 1
};

} // !namespace controllse::configuration::spi


namespace helper::spi{

using namespace controller::configuration::spi;
using namespace controller::interfaces;
using namespace controller::hardware;
using namespace controller::configuration::dma;

/*!
  @brief UART Driver for STM32F1 series. Don't use it Directly
  @tparam <adapter> spicific spi device
  @tparam <spiID> number of SPI
  @tparam <txBufferSize> size of tx buffer
  @tparam <txBufferSize> size of rx buffer
  @tparam <comm> communication type: via DMA or ISR
  @tparam <divisor> baud rate control
  @tparam <remap> remap pins of SPI
  @tparam <size> data frame format: 8/16 bit
  @tparam <format> frame format: MSB/LSB
  @tparam <mode> Polarity IDLE state and Phase capture edge
*/  
template<typename adapter, uint32_t spiID, size_t txBufferSize, size_t rxBufferSize, 
         communication comm, divisor divisor, remap remap, frame_size frame_size, frame_format frame_format, mode mode>
class Helper: public IConnection<std::conditional_t<frame_size == frame_size::FRAME_8_BIT, uint8_t, uint16_t>, 
                     txBufferSize, rxBufferSize, controller::interface::SPI,
                     Helper<adapter, spiID, txBufferSize, rxBufferSize, comm, divisor, remap, frame_size, frame_format, mode>>, 
              private controller::Clock{

  using connection = IConnection<std::conditional_t<frame_size == frame_size::FRAME_8_BIT, uint8_t, uint16_t>, 
                     txBufferSize, rxBufferSize, controller::interface::SPI,
                     Helper<adapter, spiID, txBufferSize, rxBufferSize, comm, divisor, remap, frame_size, frame_format, mode>>;


  struct dma{
    using tx = controller::DMA<adapter::dmaID, adapter::channelDMATX>;
    using rx = controller::DMA<adapter::dmaID, adapter::channelDMARX>;
  };

  struct address{
    static constexpr uint32_t
      base = adapter::baseAddress,
      CR1 = addressBase,
      CR2 = addressBase + 4,
      SR = addressBase + 8,
      DR = addressBase + 12;
  };

  struct mask{
    struct SR{
      static constexpr uint32_t
        BSY = 1 << 7, // Busy flag
        OVR = 1 << 6, // Overrun flag
        MODF = 1 << 5, // MODF fault
        CRCERR = 1 << 4, // CRC error flag
        UDR = 1 << 3, // Underrun flag
        TXE = 1 << 1, // Transmit buffer empty
        RXNE = 1 << 0; // Receive buffer not empty
    };
    struct CR1{
      static constexpr uint32_t
        SPE = 1 << 6, // SPI enable
        MSTR = 1 << 2, // Master selection
        SSI = 1 << 8, // Internal slave select
        SSM = 1 << 9; // Sofrware slave managment
    };
    struct CR2{
      static constexpr uint32_t
        TXEIE = 1 << 7, // Tx buffer empty interrupt enable
        RXNEIE = 1 << 6, // Rx buffer not empty interrupt
        ERRIE = 1 << 5, // Error interrupt enable
        SSOE = 1 << 2, // Slave select output enable
        TXDMAEN = 1 << 1, // Tx buffer DMA enable
        RXDMAEN = 1 << 0; // Rx buffer DMA enable
    };
    struct CR3{
      static constexpr uint32_t
        EIE = 1 << 0; // Error interrupt enable
    };
  };

  static inline size_t countToSend = 0;
  static inline size_t countToSendCurrent = 0;

public:

  /*!
    @brief Initialization of SPI
  */  
  static void Init(){
    Registers::_Write<address::CR2, valueCR2>();
    Registers::_Write<address::CR1, valueCR1>();

    if constexpr (isRXDMA || isTXDMA){
      if constexpr(isRXDMA){
        dma::rx::template SetPeripheral<address::DR>();
        dma::rx::template Init<false, minc::MINC_Enabled, pinc::PINC_Disabled,
                               dir::DIR_ToMemory, circ::CIRC_Disabled, isr::ISR_TC>();
      }
      if constexpr(isTXDMA){
        dma::tx::template SetPeripheral<address::DR>();
        dma::tx::template Init<false, minc::MINC_Enabled, pinc::PINC_Disabled,
                               dir::DIR_ToPeripheral, circ::CIRC_Disabled, isr::ISR_TC>();
      }
    }
  }

  __FORCE_INLINE static void SetCountSendAtOnce(size_t count){
    countToSend = countToSendCurrent = count;
  };

  /*!
    @brief Interrupt Handler for TX via DMA
  */ 
  __FORCE_INLINE static void ISR_DMA_TX(){
    dma::tx::ClearFlags();
    dma::tx::Disable();
    if (connection::txBuffer.IsEmpty()){
      if (connection::CallbackIdleTX)
        connection::CallbackIdleTX();
    }
    else if (!countToSend)
      _EnableTxDMA(); 
  }

  /*!
    @brief Interrupt Handler for RX via DMA
  */ 
  __FORCE_INLINE static void ISR_DMA_RX(){
    dma::rx::ClearFlags();
    dma::rx::Disable();
    connection::rxBuffer.AddToTail(countToAddRX);
    if (countRX){
      _EnableRxDMA();
    }
    else if (connection::CallbackRxNotEmpty && (!countToSend || (countToSend && !countToSendCurrent))){
      countToSendCurrent = countToSend; 
      connection::CallbackRxNotEmpty();
    }
    if (countToSend && !connection::txBuffer.IsEmpty()){
      if constexpr (isTXDMA) _EnableTxDMA(); 
      else Registers::_Set<address::CR2, mask::CR2::TXEIE>();
    }
  }

  /*!
    @brief Interrupt Handler
  */ 
  __FORCE_INLINE static void ISR(){
    valueSR = Registers::_Read<address::SR>();
    typename connection::type valueDR = Registers::_Read<address::DR>();

    if constexpr (!isTXDMA){
      if (valueSR & mask::SR::TXE && Registers::_Read<address::CR2, mask::CR2::TXEIE>()){
        if (!connection::txBuffer.IsEmpty() && (!countToSend || (countToSend && countToSendCurrent))){
          if constexpr (isRXDMA) {
            countRX++;
            _EnableRxDMA();
          }
          if (countToSend && countToSendCurrent) countToSendCurrent--;
          Registers::_Write<address::DR>(connection::txBuffer.Pop());
        }
        else {
          Registers::_Clear<address::CR2, mask::CR2::TXEIE>();
          if (connection::CallbackIdleTX)
            connection::CallbackIdleTX();
        }
      }
    }

    if constexpr (!isRXDMA){
      if (valueSR & mask::SR::RXNE){
        connection::rxBuffer.Push(valueDR);
        if (!Registers::_Read<address::SR, mask::SR::BSY>()){
          if (connection::CallbackRxNotEmpty && (!countToSend || (countToSend && !countToSendCurrent))){
            countToSendCurrent = countToSend;
            connection::CallbackRxNotEmpty();
          }
          if (countToSend && !connection::txBuffer.IsEmpty()){
            if constexpr (isTXDMA) _EnableTxDMA();
            else Registers::_Set<address::CR2, mask::CR2::TXEIE>();
          }
        }
      }
    }

    if((valueSR & (mask::SR::OVR | mask::SR::MODF | mask::SR::CRCERR | mask::SR::UDR)) && connection::CallbackError){
      connection::CallbackError();
    }
  }

  /*!
    @brief Clear SPI errors
  */
  __FORCE_INLINE static void ClearErrors(){
    (void) Registers::_Read<address::SR>();
    (void) Registers::_Read<address::DR>();
    valueSR = 0;
  }

  /*!
    @brief Get SPI Errors
  */
  static const auto& GetErrors(){
    error.crc = valueSR & mask::SR::CRCERR;
    error.master = valueSR & mask::SR::MODF;
    error.overrun = valueSR & mask::SR::OVR;
    error.underrun = valueSR & mask::SR::UDR;
    return error;
  }

protected:

  struct error{
    bool master;
    bool overrun;
    bool crc;
    bool underrun;
  };
  
  static inline error error;
  static inline size_t rxBuffer_prevCount = rxBufferSize;
  static inline uint32_t valueSR = 0;
  static inline uint32_t countRX = 0;
  static inline uint32_t countToAddRX = 0;

  __FORCE_INLINE static void _Send(){
    if constexpr(!isTXDMA) 
      Registers::_Set<address::CR2, mask::CR2::TXEIE>();
    else _EnableTxDMA();
  }

  __FORCE_INLINE static void _CheckTxBuffer(){ }

  __FORCE_INLINE static void _CheckRxBuffer(){ }

  static void _EnableTxDMA(){
    if (dma::tx::IsEnabled()) return;
    auto address = reinterpret_cast<uint32_t>(connection::txBuffer.GetHeadAddress());
    auto count = connection::txBuffer.GetCountToBufferLastIndex();
    if (countToSend){
      if (!countToSendCurrent) countToSendCurrent = countToSend;
      if (count > countToSendCurrent){
        count = countToSendCurrent;
        countToSendCurrent = 0;
      }
      else countToSendCurrent -= count; 
    }
    if constexpr (isRXDMA){
      countRX += count;
      _EnableRxDMA();
    }
    dma::tx::SetCount(count);
    dma::tx::SetMemory(address);
    connection::txBuffer.AddToHead(count);
    dma::tx::Enable();
  }

  static void _EnableRxDMA(){
    if (dma::rx::IsEnabled()) return; 
    auto address = reinterpret_cast<uint32_t>(connection::rxBuffer.GetTailAddress());
    countToAddRX = connection::rxBuffer.GetCountToBuffersEnd();
    countToAddRX = countRX > countToAddRX ? countToAddRX : countRX;
    countRX -= countToAddRX;
    dma::rx::SetCount(countToAddRX);
    dma::rx::SetMemory(address);
    dma::rx::Enable();
  }

  static constexpr uint32_t valueCR1 = mask::CR1::SPE  | // Enable SPI
                                       mask::CR1::SSM  | // Enable software slave managment 
                                       mask::CR1::SSI  | // Force slave select to active
                                       mask::CR1::MSTR | // Master mode
                                       (uint32_t)mode | // Set polarity and Phase
                                       (uint32_t)divisor | // Set clock divisor
                                       (uint32_t)frame_size | // Size of data: 8/16 bit
                                       (uint32_t)frame_format; // LSB/MSB

  static constexpr uint32_t valueCR2 = mask::CR2::ERRIE  | // Enable Error interrupt 
                                       (uint32_t)comm;// Set polarity and Phase

  template<typename>
  friend class controller::interfaces::IPower;

  friend controller::Interrupt;

  template<typename...>
  friend class helper::pinlist::Helper;

  friend connection;


  static constexpr bool isTXDMA = comm == communication::txDMA_rxDMA ||
                                  comm == communication::txDMA_rxISR;

  static constexpr bool isRXDMA = comm == communication::txDMA_rxDMA ||
                                  comm == communication::txISR_rxDMA;


  struct initialization{
    static constexpr auto irqnDMATX = isTXDMA ? adapter::irq::DMATX : 0;
    static constexpr auto irqnDMARX = isRXDMA ? adapter::irq::DMARX : 0;
    using MOSI = typename adapter::pins::MOSI::mode::set<controller::configuration::pin::Alternative_PushPull, adapter::remapValue>;
    using MISO = typename adapter::pins::MISO::mode::set<controller::configuration::pin::Input_Float, adapter::remapValue>;
    using CLCK = typename adapter::pins::CLCK::mode::set<controller::configuration::pin::Alternative_PushPull, adapter::remapValue>;
    using powerSPI = controller::Power::fromValues<
      (isTXDMA || isRXDMA) ? adapter::power::DMAEN : 0, 
      adapter::power::SPIEN, 
      adapter::power::SPI1EN>;
    using power = typename controller::Power::fromPeripherals<powerSPI, MOSI, MISO, CLCK>::power;
    using pins = trait::Typelist<MOSI, MISO, CLCK>;
    using interrupts = trait::remove_value_t<0,trait::Valuelist<adapter::irq::SPI, irqnDMATX, irqnDMARX>>;
  };

};

} // !namespace helper::spi

#endif  // !_STM32F1_SPI_HELPER_HPP