//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Helper for UART of STM32F1
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _STM32F1_UART_HELPER_HPP
#define _STM32F1_UART_HELPER_HPP

#include "../Common/Compiler/Compiler.h"
#include "../Common/Core/Interrupt.hpp"
#include "../Interfaces/IPower.hpp"
#include "../Interfaces/IConnection.hpp"
#include "../Clock/stm32f1_Clock.hpp"
#include "../Pinlist/stm32f1_Pinlist.hpp"
#include "../DMA/stm32f1_DMA.hpp"

/*!
  @brief Configuration for UART
*/ 
namespace controller::configuration::uart{

/*!
  @brief Remap of pins
*/ 
enum class remap{
  /*! @brief Pins not remaped*/ 
  None,
  /*! @brief Partial remap of pins*/ 
  Partial,
  /*! @brief Full remap of pins*/ 
  Full
};

/*!
  @brief UART mode: Parity, data length, stop bits
*/ 
enum class mode{
  /*! @brief None parity, 8-bit data, 0.5 stop bit*/
  N_8_0_5 = 0x4000,
  /*! @brief None parity, 8-bit data, 1 stop bit*/
  N_8_1 = 0,
  /*! @brief None parity, 8-bit data, 1.5 stop bit*/
  N_8_1_5 = 0xC000,
  /*! @brief None parity, 8-bit data, 2 stop bit*/
  N_8_2 = 0x8000,
  /*! @brief None parity, 9-bit data, 0.5 stop bit*/
  N_9_0_5 = 0x5000,
  /*! @brief None parity, 9-bit data, 1 stop bit*/
  N_9_1 = 0x1000,
  /*! @brief None parity, 9-bit data, 1.5 stop bit*/
  N_9_1_5 = 0xD000,
  /*! @brief None parity, 9-bit data, 2 stop bit*/
  N_9_2 = 0x9000,
  /*! @brief Even parity, 8-bit data, 0.5 stop bit*/
  E_8_0_5 = 0x4500,
  /*! @brief Even parity, 8-bit data, 1 stop bit*/
  E_8_1 = 0x500,
  /*! @brief Even parity, 8-bit data, 1.5 stop bit*/
  E_8_1_5 = 0xC500,
  /*! @brief Even parity, 8-bit data, 2 stop bit*/
  E_8_2 = 0x8500,
  /*! @brief Even parity, 9-bit data, 0.5 stop bit*/
  E_9_0_5 = 0x5500,
  /*! @brief Even parity, 9-bit data, 1 stop bit*/
  E_9_1 = 0x1500,
  /*! @brief Even parity, 9-bit data, 1.5 stop bit*/
  E_9_1_5 = 0xD500,
  /*! @brief Even parity, 9-bit data, 2 stop bit*/
  E_9_2 = 0x9500,
  /*! @brief Odd parity, 8-bit data, 0.5 stop bit*/
  O_8_0_5 = 0x4700,
  /*! @brief Odd parity, 8-bit data, 1 stop bit*/
  O_8_1 = 0x700,
  /*! @brief Odd parity, 8-bit data, 1.5 stop bit*/
  O_8_1_5 = 0xC700,
  /*! @brief Odd parity, 8-bit data, 2 stop bit*/
  O_8_2 = 0x8700,
  /*! @brief Odd parity, 9-bit data, 0.5 stop bit*/
  O_9_0_5 = 0x5700,
  /*! @brief Odd parity, 9-bit data, 1 stop bit*/
  O_9_1 = 0x1700,
  /*! @brief Odd parity, 9-bit data, 1.5 stop bit*/
  O_9_1_5 = 0xD700,
  /*! @brief Odd parity, 9-bit data, 2 stop bit*/
  O_9_2 = 0x9700
};

/*!
  @brief UART communication: via DMA or ISR
*/ 
enum class communication{
  /*! @brief Transmitt - DMA, Receive - DMA*/
  txDMA_rxDMA = 0xC000,
  /*! @brief Transmitt - ISR, Receive - ISR*/
  txISR_rxISR = 0x20,
  /*! @brief Transmitt - DMA, Receive - ISR*/
  txDMA_rxISR = 0x8020,
  /*! @brief Transmitt - ISR, Receive - DMA*/
  txISR_rxDMA = 0x4000
};

} // !namespace controllse::configuration::uart


namespace helper::uart{

using namespace controller::configuration::uart;
using namespace controller::interfaces;
using namespace controller::hardware;
using namespace controller::configuration::dma;

/*!
  @brief UART-Helper for STM32F1 series. Don't use it Directly
  @tparam <adapter> specific UART device
  @tparam <uartID> number of UART
  @tparam <txBufferSize> size of tx buffer
  @tparam <txBufferSize> size of rx buffer
  @tparam <comm> communication type: via DMA or ISR
  @tparam <mode> parity, size of data, number of stop bits
  @tparam <remap> remap pins of UART
*/  
template<typename adapter, uint32_t uartID, size_t txBufferSize, size_t rxBufferSize, 
         communication comm, mode mode, remap remap>
class Helper: public IConnection<uint8_t, txBufferSize, rxBufferSize, controller::interface::UART,
                                 Helper<adapter, uartID, txBufferSize, rxBufferSize, comm, mode, remap>>, 
              private controller::Clock{

  using connection = IConnection<uint8_t, txBufferSize, rxBufferSize, controller::interface::UART,
                                Helper<adapter, uartID, txBufferSize, rxBufferSize, comm, mode, remap>>;


  struct dma{
    using tx = controller::DMA<adapter::dmaID, adapter::channelDMATX>;
    using rx = controller::DMA<adapter::dmaID, adapter::channelDMARX>;
  };

  struct address{
    static constexpr uint32_t
      base = adapter::baseAddress,
      SR = base,
      DR = base + 4,
      BRR = base + 8,
      CR1 = base + 12,
      CR2 = base + 16,
      CR3 = base + 20,
      GTRP = base + 24;
  };

  struct mask{
    struct SR{
      static constexpr uint32_t 
        TXE = 1 << 7, // Transmitter data register empty
        TC = 1 << 6, // Transmitter complete
        RXNE = 1 << 5, // Read data register not empty
        IDLE = 1 << 4, // IDLE line detected
        ORE = 1 << 3, // Overrun error
        NE = 1 << 2, // Noise error flag
        FE = 1 << 1, // Framing error flag
        PE = 1 << 0; // Parity error flag
    };
    struct CR1{
      static constexpr uint32_t 
        UE = 1 << 13, // UART enable
        PCE = 1 << 10, // Parity control enable
        PE = 1 << 8, // Parity error interrupt enable
        TXEIE = 1 << 7, // TXE interrupt enable
        TCIE = 1 << 6, // Transmission complete interrupt enable
        RXNEIE = 1 << 5, // RXNE interrupt enable
        IDLEIE = 1 << 4, // IDLE interrupt enable
        TE = 1 << 3, // Transmitter enable
        RE = 1 << 2; // Receiver enable
    };
    struct CR3{
      static constexpr uint32_t 
        EIE = 1 << 0; // Error interrupt enable
    };
  };

public:

  /*!
    @brief Initialization of UART
    @tparam <baud> desired baud-rate for uart
  */  
  template<uint32_t baud>
  static void Init(){
    Registers::_Write<address::CR1, valueCR1>();

    if constexpr (valueCR2)
      Registers::_Write<address::CR2, valueCR2>();
    Registers::_Write<address::CR3, valueCR3>();
    Registers::_Write<address::BRR>(_CalculateBRR<baud>());

    if constexpr (isRXDMA || isTXDMA){
      if constexpr(isRXDMA){
        auto memoryAddress = reinterpret_cast<uint32_t>(connection::rxBuffer.GetHeadAddress());
        dma::rx::template SetPeripheral<address::DR>();
        dma::rx::template SetMemory(memoryAddress);
        dma::rx::template SetCount<rxBufferSize>();
        dma::rx::template Init<true, minc::MINC_Enabled, pinc::PINC_Disabled,
                               dir::DIR_ToMemory, circ::CIRC_Enabled, isr::ISR_None>();
      }
      if constexpr(isTXDMA){
        dma::tx::template SetPeripheral<address::DR>();
        dma::tx::template Init<false, minc::MINC_Enabled, pinc::PINC_Disabled,
                               dir::DIR_ToPeripheral, circ::CIRC_Disabled, isr::ISR_TC>();
      }
    }
  }

  /*!
    @brief Set baud rate
    @tparam <baud> desired baud-rate for uart
  */ 
  template<uint32_t baud>
  __FORCE_INLINE static void SetBaud(){
    Registers::_Write<address::BRR>(_CalculateBRR<baud>());
  }

  /*!
    @brief DMA TX Handler
  */ 
  __FORCE_INLINE static void ISR_DMA_TX(){
    dma::tx::ClearFlags();
    dma::tx::Disable();
    if (!connection::txBuffer.IsEmpty())
      _EnableTxDMA();
    else if (connection::CallbackIdleTX)
      connection::CallbackIdleTX(); 
  }

  /*!
    @brief ISR Handler
  */ 
  __FORCE_INLINE static void ISR(){
    valueSR = Registers::_Read<address::SR>();
    uint8_t drValue = Registers::_Read<address::DR>();

    if constexpr (!isTXDMA){
      if (valueSR & mask::SR::TXE){
        if (!connection::txBuffer.IsEmpty())
          Registers::_Write<address::DR>(connection::txBuffer.Pop());
        else Registers::_Clear<address::CR1, mask::CR1::TXEIE | mask::CR1::TCIE>();
      }
    }

    if constexpr (!isRXDMA){
      if (valueSR & mask::SR::RXNE)
        connection::rxBuffer.Push(drValue);
    }

    if (valueSR & mask::SR::TC){
      Registers::_Set<address::SR, 0U, mask::SR::TC>();
      if (connection::CallbackIdleTX)
        connection::CallbackIdleTX();
    }

    if (valueSR & mask::SR::IDLE && connection::CallbackRxNotEmpty){
      if constexpr(isRXDMA) 
        _CheckRxDMA();
      connection::CallbackRxNotEmpty();
    }

    if((valueSR & (mask::SR::ORE | mask::SR::PE | mask::SR::NE | mask::SR::FE)) && connection::CallbackError){
      connection::CallbackError();
    }
  }

  /*!
    @brief Clear UART errors
  */
  __FORCE_INLINE static void ClearErrors(){
    (void) Registers::_Read<address::SR>();
    (void) Registers::_Read<address::DR>();
    valueSR = 0;
  }

  /*!
    @brief Get UART Errors
  */
  static const auto& GetErrors(){
    error.parity = valueSR & mask::SR::PE;
    error.overrun = valueSR & mask::SR::ORE;
    error.noise = valueSR & mask::SR::NE;
    error.frame = valueSR & mask::SR::FE;
    return error;
  }

protected:

  struct error{
    bool parity;
    bool overrun;
    bool noise;
    bool frame;
  };
  
  static inline error error;
  static inline size_t countPrevRxBuffer = rxBufferSize;
  static inline uint32_t valueSR = 0;

  __FORCE_INLINE static void _Send(){
    if constexpr(!isTXDMA) 
      Registers::_Set<address::CR1, mask::CR1::TXEIE | mask::CR1::TCIE>();
    else if (!dma::tx::IsEnabled()) 
      _EnableTxDMA();
  }

  __FORCE_INLINE static void _CheckTxBuffer(){ }

  __FORCE_INLINE static void _CheckRxBuffer(){ }

  static void _EnableTxDMA(){
    auto memoryAddress = reinterpret_cast<uint32_t>(connection::txBuffer.GetHeadAddress());
    dma::tx::SetCount(connection::txBuffer.GetCountToBufferLastIndex());
    dma::tx::SetMemory(memoryAddress);
    connection::txBuffer.SetHeadToLastIndex();
    dma::tx::Enable();
  }

  static void _CheckRxDMA(){
    size_t count = dma::rx::GetCount();
    bool isTransferComplete = dma::rx::IsTransferComplete();
    if(countPrevRxBuffer != count || isTransferComplete){
      size_t diff = countPrevRxBuffer > count ? 
                    countPrevRxBuffer - count  + (isTransferComplete ? rxBufferSize : 0) : 
                    rxBufferSize - count + countPrevRxBuffer;
      connection::rxBuffer.AddToTail(diff);
      countPrevRxBuffer = count;
    }
    dma::rx::ClearFlags();
  }

  static constexpr bool isTXDMA = comm == communication::txDMA_rxDMA ||
                                  comm == communication::txDMA_rxISR;

  static constexpr bool isRXDMA = comm == communication::txDMA_rxDMA ||
                                  comm == communication::txISR_rxDMA;

  static constexpr uint32_t valueCR1 = mask::CR1::UE  | // Enable USART
                                       mask::CR1::TE  | // Enable Transmit
                                       mask::CR1::RE  | // Enable Receiver
                                       mask::CR1::IDLEIE  | // Enable IDLE ISR
                                       (isTXDMA ?  mask::CR1::TCIE : 0) | // Enable Transfer complete ISR
                                       (uint32_t)comm & 0x20  | // Enable RXNEIE
                                       (uint32_t)mode & 0x100 | // Enable parity error ISR
                                       (uint32_t)mode & 0x200 | // Parity Selection (Even or Odd)
                                       (uint32_t)mode & 0x400 | // Parity control disabled
                                       (uint32_t)mode & 0x3000; // Data length (8 or 9 bit)

  static constexpr uint32_t valueCR2 = ((uint32_t)mode >> 2) & 0x3000; // Set (unpack) number of stop bits
  
  static constexpr uint32_t valueCR3 = mask::CR3::EIE | // Enable error ISR 
                                      (((uint32_t)comm >> 8) & 0xC0); // Enable DMAT and DMAR

  template<uint32_t baud>
  __FORCE_INLINE static uint32_t _CalculateBRR(){ 
    return ((baud>>1) + (uartID == 1 ? 
    Clock::APB2() : 
    Clock::APB1()))/baud;
  }

  template<typename>
  friend class controller::interfaces::IPower;

  friend controller::Interrupt;

  template<typename...>
  friend class helper::pinlist::Helper;

  friend connection;

  struct initialization{
    using TX = typename adapter::pins::TX::mode::set<controller::configuration::pin::Alternative_PushPull, adapter::remapValue>;
    using RX = typename adapter::pins::RX::mode::set<controller::configuration::pin::Input_Float, adapter::remapValue>;
    using powerUart = controller::Power::fromValues<
          (isTXDMA || isRXDMA) ? adapter::power::DMAEN : 0, 
          adapter::power::UARTEN, 
          adapter::power::UART1EN>;
    using power = typename controller::Power::fromPeripherals<powerUart, TX, RX>::power;
    using pins = trait::Typelist<TX, RX>;
    using interrupts = trait::remove_value_t<0,trait::Valuelist<adapter::irq::UART, isTXDMA ? adapter::irq::DMATX : 0>>;
  };

};

} // !namespace helper::uart

#endif // !_STM32F1_UART_HELPER_HPP