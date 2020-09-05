//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Hardware-dependent driver for UART. STM32F1-series
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _STM32F1_UART_HPP
#define _STM32F1_UART_HPP

#include <cstdint>
#include <cstddef>
#include "stm32f10x.h"
#include "stm32f1_Clock.hpp"
#include "stm32f1_Pinlist.hpp"
#include "stm32f1_Power.hpp"
#include "connection.hpp"
#include "interrupt.hpp"

namespace controller{

/*!
  @brief UART Driver for STM32F1 series. Don't use it Directly
  @tparam <baseUART> base address of UART
  @tparam <txBufferSize> size of tx buffer
  @tparam <txBufferSize> size of rx buffer
  @tparam <isTXviaDMA> if true, then data transmits via DMA, otherwise via TX ISR
  @tparam <isRXviaDMA> if true, then data receives via DMA, otherwise via RX ISR
  @tparam <isPinsRemap> if true, then TX and RX pins will be remaped
*/  
template<uint32_t baseUART, size_t txBufferSize, size_t rxBufferSize,
         bool isTXviaDMA = true, bool isRXviaDMA = true, bool isPinsRemap = false>
class UART: public common::connection<uint8_t, txBufferSize, rxBufferSize, 
                  UART<baseUART, txBufferSize, rxBufferSize, isTXviaDMA, isRXviaDMA, isPinsRemap>>{

private:
  using baseConnection = common::connection<uint8_t, txBufferSize, rxBufferSize, 
                          UART<baseUART, txBufferSize, rxBufferSize, isTXviaDMA, isRXviaDMA, isPinsRemap>>;
public:

  /*!
    @brief Inizialization of UART
    @tparam <baud> desired baud-rate for uart
    @tparam <configurePins> if true, then pins will be initialized
  */  
  template<uint32_t baud>
  static void Init(){
    base()->CR1 = initValueCR1;
    base()->BRR = CalcValueBRR<baud>();
    //base()->SR = 0; // To reset Status register, there is should be some time after enabling UART in CR1
    base()->CR3 = USART_CR3_EIE | (isTXviaDMA ? USART_CR3_DMAT : 0U) | (isRXviaDMA ? USART_CR3_DMAR : 0U);
    if constexpr (isTXviaDMA || isRXviaDMA){
      if constexpr(isRXviaDMA){
        DMA_Channel_rx()->CPAR = baseUART + 4;
        DMA_Channel_rx()->CMAR = reinterpret_cast<uint32_t>(baseConnection::rxBuffer.GetHeadAddress());
        DMA_Channel_rx()->CNDTR = rxBufferSize;
        DMA_Channel_rx()->CCR  =  DMA_CCR1_EN // Enable DMA channel
                               |  DMA_CCR1_MINC // Memory increment
                               |  DMA_CCR1_HTIE // Half Transfer complete ISR
                               |  DMA_CCR1_TCIE // Full Transfer complete ISR
                               |  DMA_CCR2_CIRC; // Enable circular mode
      }
      if constexpr(isTXviaDMA){
        DMA_Channel_tx()->CPAR = baseUART + 4;
        DMA_Channel_tx()->CCR  =  DMA_CCR1_MINC // Memory increment
                               |  DMA_CCR1_DIR  // Direction - to peripheria
                               |  DMA_CCR1_TCIE;// Full Transfer complete ISR
      }
    }
  }

  /*!
    @brief Set baud rate
    @tparam <baud> desired baud-rate for uart
  */ 
  template<uint32_t baud>
  static void SetBaud(){
    base()->BRR = CalcValueBRR<baud>();
  }

  /*!
    @brief DMA TX Handler
  */ 
  static void ISR_DMA_TX(){
    DMA1->IFCR  = dmaIsrClearValue<DMA_Channel_TX_Number>;
    DMA_Channel_tx()->CCR &= ~DMA_CCR1_EN;
    if (!baseConnection::txBuffer.IsEmpty())
      EnableTxDMA();
    else if (!__NVIC_GetEnableIRQ(uartIRQn)) baseConnection::CallbackTxIdle(); 
  }

  /*!
    @brief DMA RX Handler
  */ 
  static void ISR_DMA_RX(){
    DMA1->IFCR  = dmaIsrClearValue<DMA_Channel_RX_Number>;
    CheckRxDMA();
  }

  /*!
    @brief ISR Handler
  */ 
  static void ISR(){
    uint32_t CR1 = base()->CR1;
	  valueSR =  base()->SR;
    uint8_t drValue = base()->DR;

    if (CR1 & USART_CR1_TXEIE && valueSR & USART_SR_TXE){
      if (!baseConnection::txBuffer.IsEmpty()) base()->DR = baseConnection::txBuffer.Pop();
      else base()->CR1 &=~ USART_CR1_TXEIE;
	  }

    if (CR1 & USART_CR1_RXNEIE && valueSR & USART_SR_RXNE){
      baseConnection::rxBuffer.Push(drValue);
    }

    if (valueSR & USART_SR_TC){
      base()->SR &=~USART_SR_TC;
      baseConnection::CallbackTxIdle();
    }

    if (valueSR & USART_SR_IDLE){
      if (!baseConnection::isCallbackRxNotEmptyCalled){
        baseConnection::CallbackRxNotEmpty();
        baseConnection::isCallbackRxNotEmptyCalled = true;
      }
    }

    if(valueSR & (USART_SR_ORE | USART_SR_PE | USART_SR_NE | USART_SR_FE)){
      baseConnection::CallbackError();
    }
  }

  /*!
    @brief Clear UART errors
  */
  static void ClearErrors(){
    (void) base()->SR;
    (void) base()->DR;
    valueSR = 0;
  }

  /*!
    @brief Get UART Errors
  */
  static const auto& GetErrors(){
    if constexpr (isTXviaDMA && isRXviaDMA)
      valueSR = base()->SR;
    error.parity = valueSR & USART_SR_PE;
    error.overrun = valueSR & USART_SR_ORE;
    error.noise = valueSR & USART_SR_NE;
    error.frame = valueSR & USART_SR_FE;
    return error;
  }

private:

  struct errorUart{
    bool parity;
    bool overrun;
    bool noise;
    bool frame;
  };
  
  static inline errorUart error;
  static inline size_t rxBuffer_prevCount = rxBufferSize;
  static inline uint32_t valueSR = 0;

  static void Send(){
    if constexpr(!isTXviaDMA) base()->CR1 |= USART_CR1_TXEIE; // Enable transmit
    else 
      if (!(DMA_Channel_tx()->CCR & DMA_CCR1_EN)) EnableTxDMA();
  }

  __attribute__ ((always_inline)) inline
  static void CheckTxBuffer(){ }

  __attribute__ ((always_inline)) inline
  static void CheckRxBuffer(){ 
    if constexpr(isRXviaDMA) 
      CheckRxDMA();
  }

  static void EnableTxDMA(){
    DMA_Channel_tx()->CNDTR = baseConnection::txBuffer.GetCountToBufferLastIndex();
    DMA_Channel_tx()->CMAR = (uint32_t)(baseConnection::txBuffer.GetHeadAddress());
    baseConnection::txBuffer.SetHeadToLastIndex();
    DMA_Channel_tx()->CCR |= DMA_CCR1_EN;
  }

  static void CheckRxDMA(){
    size_t dmaCNTDR = DMA_Channel_rx()->CNDTR;
     if(baseConnection::rxBuffer_prevCount != dmaCNTDR){
      size_t diff = rxBuffer_prevCount > dmaCNTDR ? 
                    rxBuffer_prevCount - dmaCNTDR : 
                    rxBufferSize - dmaCNTDR + rxBuffer_prevCount;
      rxBuffer_prevCount = dmaCNTDR;
      baseConnection::rxBuffer.AddToTail(diff);
      if(!baseConnection::isCallbackRxNotEmptyCalled){
        baseConnection::isCallbackRxNotEmptyCalled = true;
        baseConnection::CallbackRxNotEmpty();
      }
    }
  }

  static inline auto constexpr base() {return (USART_TypeDef*)baseUART;}

  static constexpr auto DMA_Channel_rx(){ 
    return (DMA_Channel_TypeDef *)(DMA1_BASE + 8U + 20*DMA_Channel_RX_Number);
  }  

  static constexpr auto DMA_Channel_tx(){ 
    return (DMA_Channel_TypeDef *)(DMA1_BASE + 8U + 20*DMA_Channel_TX_Number);
  }

  static constexpr uint32_t DMA_Channel_TX_Number =(
                                        baseUART == USART1_BASE ? 4: 
                                        baseUART == USART2_BASE ? 7: 
                                        baseUART == USART3_BASE ? 2: 5) - 1;

  static constexpr uint32_t DMA_Channel_RX_Number = (
                                        baseUART == USART1_BASE ? 5: 
                                        baseUART == USART2_BASE ? 6: 
                                        baseUART == USART3_BASE ? 4: 3) - 1;


  static constexpr uint8_t pinsGroup(){
    if (baseUART == USART1_BASE){
      if (!isPinsRemap) return 0;
      else return 1;
    } else if (baseUART == USART2_BASE){
      if (!isPinsRemap) return 2;
      else return 3;
    } else if (baseUART == USART3_BASE){
      if (!isPinsRemap) return 4;
      //else if (Full Remap) return 5; // Full remap
      else return 6;
    }
  }

  static constexpr uint8_t uartPins[][2] = {{9, 10},
                                            {6, 7},
                                            {2, 3},
                                            {5, 6},
                                            {10, 11},
                                            {8, 9},
                                            {10, 11}};

  static constexpr uint32_t pinsRemapValue = baseUART == USART1_BASE ? 4U :
                                             baseUART == USART2_BASE ? 8U : 
                                             baseUART == USART3_BASE ? 16U : 
                                                                             0U;
  static constexpr uint32_t uartPinsBase[][2] = { {GPIOA_BASE, GPIOA_BASE},
                                                  {GPIOB_BASE, GPIOB_BASE}, 
                                                  {GPIOA_BASE, GPIOA_BASE},
                                                  {GPIOD_BASE, GPIOD_BASE},
                                                  {GPIOB_BASE, GPIOB_BASE},
                                                  {GPIOD_BASE, GPIOC_BASE},
                                                  {GPIOC_BASE, GPIOD_BASE}};

  static constexpr auto uartIRQn = baseUART == USART1_BASE ? IRQn::USART1_IRQn :
                                   baseUART == USART2_BASE ? IRQn::USART2_IRQn :
                                                             IRQn::USART3_IRQn;

  static constexpr auto uartDMAtxIRQn = baseUART == USART1_BASE ? IRQn::DMA1_Channel4_IRQn :
                                        baseUART == USART2_BASE ? IRQn::DMA1_Channel7_IRQn :
                                        baseUART == USART3_BASE ? IRQn::DMA1_Channel2_IRQn :
                                                                  IRQn::DMA1_Channel5_IRQn ;

  static constexpr auto uartDMArxIRQn = baseUART == USART1_BASE ? IRQn::DMA1_Channel5_IRQn :
                                        baseUART == USART2_BASE ? IRQn::DMA1_Channel6_IRQn :
                                        baseUART == USART3_BASE ? IRQn::DMA1_Channel4_IRQn : 
                                                                  IRQn::DMA1_Channel3_IRQn ;

  static constexpr auto initValueCR1 = USART_CR1_UE | // Enable USART
                                       USART_CR1_TE | // Enable Transmit
                                       USART_CR1_RE | // Enable Receiver
                                       USART_CR1_IDLEIE  | // Enable IDLE ISR
                                       USART_CR1_TCIE | // Enable Transfer complete ISR
                                       USART_CR1_PEIE | // Enable parity error
                                      //USART_CR1_PCE | // Enable parity check
                                      (isRXviaDMA ? 0U : USART_CR1_RXNEIE); // Enable Receiver ISR

  template<uint8_t channelNumber>
  static constexpr auto dmaIsrClearValue = 15U << (channelNumber)*4;

  template<uint32_t baud>
  __attribute__ ((always_inline)) inline 
  static uint32_t CalcValueBRR(){ 
    return ((baud>>1) + (baseUART == USART1_BASE ? 
    Clock::APB2() : 
    Clock::APB1()))/baud;
  }

  friend Power;
  friend Interrupt;
  template<typename...>
  friend class Pinlist;

  friend baseConnection;

  using pins = utils::Typelist<
      Pin<uartPinsBase[pinsGroup()][0], uartPins[pinsGroup()][0], pinConfiguration::Alternative_PushPull>,
      Pin<uartPinsBase[pinsGroup()][1], uartPins[pinsGroup()][1], pinConfiguration::Input_Float>>;

  using power = utils::Typelist<utils::ctv<baseUART>, 
                                utils::ctv<uartPinsBase[pinsGroup()][0]>, 
                                utils::ctv<uartPinsBase[pinsGroup()][1]>, 
                                utils::ctv<AFIO_BASE>,
                                utils::ctv<isTXviaDMA || isRXviaDMA ? DMA1_BASE: 0U>>;

  using interrupts = utils::make_unique_t<utils::Typelist< 
                                      std::conditional_t<isTXviaDMA, utils::ctv<uartDMAtxIRQn>, utils::ctv<uartIRQn>>,
                                      std::conditional_t<isTXviaDMA, utils::ctv<uartDMArxIRQn>, utils::ctv<uartIRQn>>
                                        >>;

  using remap = utils::ctv<pinsRemapValue>;

};

} // !namspace controller

#endif //!_STM32F1_UART_HPP