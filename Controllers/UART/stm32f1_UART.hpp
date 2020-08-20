//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Hardware-dependent driver for UART.  
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _STM32F1_UART_HPP
#define _STM32F1_UART_HPP

#include <cstdint>
#include <cstddef>
#include "stm32f10x.h"
#include "stm32f1_Clock.hpp"
#include "stm32f1_Pinlist.hpp"
#include "Circular_Buffer.hpp"
#include "Callback.hpp"

namespace stm32f1::hardware{

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
class _UARTImplementation{

public:

  /*!
    @brief Inizialization of UART
    @tparam <baud> desired baud-rate for uart
    @tparam <configurePins> if true, then pins will be initialized
  */  
  template<uint32_t baud, bool configurePins = false>
  static void Init(){
    base()->CR1 = initValueCR1;
    base()->BRR = CalcValueBRR<baud>();
    //base()->SR = 0; // To reset Status register, thereis should be some time after enabling UART in CR1
    base()->CR3 = USART_CR3_EIE | (isTXviaDMA ? USART_CR3_DMAT : 0U) | (isRXviaDMA ? USART_CR3_DMAR : 0U);
    if constexpr (isTXviaDMA || isRXviaDMA){
      if constexpr(isRXviaDMA){
        DMA_Channel_rx()->CPAR = baseUART + 4;
        DMA_Channel_rx()->CMAR = reinterpret_cast<uint32_t>(rxBuffer.GetHeadAddress());
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
    if constexpr (configurePins) InitPins();
    if constexpr (isPinsRemap) AFIO->MAPR |= uartPins[pinsGroup()][2];
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
    @brief Write data to uart transmitter
    @param [in] data to send
    @return true, if tx buffer not overflowed
  */ 
  static bool Write(uint8_t data){
    bool isOverflow = txBuffer.Push(data);
    Send();
    return isOverflow;
  }

  /*!
    @brief Write data to uart transmitter
    @param [in] pointer to data array
    @param [length] length of data to write
    @return true, if tx buffer not overflowed
  */ 
  static bool Write(const uint8_t* data, size_t length){
    bool isOverflow = txBuffer.Push(data, length);
    Send();
    return isOverflow;
  }

  /*!
    @brief Get front element of rx buffer
  */ 
  static uint8_t Front(){return rxBuffer.Front();}

  /*!
    @brief Pop front element of rx buffer
  */ 
  static uint8_t Pop(){
    if(rxBuffer.GetCount() < 2)
      isCallbackRxNotEmptyCalled = false;
    return rxBuffer.Pop();
  }

  /*!
    @brief Pop data to destination array
    @param [out] destination pointer to array
    @param [in] length to pop
    @return number of poped elements
  */ 
  static size_t Pop(uint8_t* destination, size_t length){
    if(rxBuffer.GetCount() <= length)
      isCallbackRxNotEmptyCalled = false;
    return rxBuffer.Pop(destination, length);
  }

  /*!
    @brief Get data from rx buffer by index
    @param [in] index of rx buffer
    @return data by index. If rx buffer is empty - return front value
  */ 
  static uint8_t GetAt(size_t index){
    return rxBuffer[index];
  }

  /*!
    @brief Get data from rx buffer by index
    @param [in] index of rx buffer
    @return data by index. If rx buffer is empty - return front value
  */ 
  uint8_t operator[](size_t index){return GetAt(index);}

  /*!
    @brief Check the rx buffer
    @return true if rx buffer is empty
  */ 
  static bool IsRxEmpty(){
    if constexpr(isRXviaDMA) CheckRxDMA();
    return rxBuffer.IsEmpty();
  }

  /*!
    @brief Check the tx buffer
    @return true if tx buffer is empty
  */ 
  static bool IsTxEmpty(){return txBuffer.IsEmpty();}

  /*!
    @brief Return number of elements in rx buffer
  */ 
  static size_t GetRxCount(){return rxBuffer.GetCount();}

  /*!
    @brief Return number of elements in tx buffer
  */ 
  static size_t GetTxCount(){return txBuffer.GetCount();}

  /*!
    @brief DMA TX Handler
  */ 
  static void ISR_DMA_TX(){
    DMA1->IFCR  = dmaIsrClearValue<DMA_Channel_TX_Number>;
    DMA_Channel_tx()->CCR &= ~DMA_CCR1_EN;
    if (!txBuffer.IsEmpty())
      EnableRxDMA();
    else if (!__NVIC_GetEnableIRQ(uartIRQn)) CallbackTxIdle(); 
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
      if (!txBuffer.IsEmpty()) base()->DR = txBuffer.Pop();
      else base()->CR1 &=~ USART_CR1_TXEIE;
	  }

    if (CR1 & USART_CR1_RXNEIE && valueSR & USART_SR_RXNE){
      rxBuffer.Push(drValue);
    }

    if (valueSR & USART_SR_TC){
      base()->SR &=~USART_SR_TC;
      CallbackTxIdle();
    }

    if (valueSR & USART_SR_IDLE){
      if (!isCallbackRxNotEmptyCalled){
        CallbackRxNotEmpty();
        isCallbackRxNotEmptyCalled = true;
      }
    }

    if(valueSR & (USART_SR_ORE | USART_SR_PE | USART_SR_NE | USART_SR_FE)){
      CallbackError();
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
    if(!__NVIC_GetEnableIRQ(uartIRQn))
      valueSR = base()->SR;
    error.parity = valueSR & USART_SR_PE;
    error.overrun = valueSR & USART_SR_ORE;
    error.noise = valueSR & USART_SR_NE;
    error.frame = valueSR & USART_SR_FE;
    return error;
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
  static inline container::CircularBuffer<uint8_t, txBufferSize> txBuffer;
  static inline container::CircularBuffer<uint8_t, rxBufferSize> rxBuffer;

  struct errorUart{
    bool parity;
    bool overrun;
    bool noise;
    bool frame;
  };
  static inline errorUart error;
  static inline size_t rxBuffer_prevCount = rxBufferSize;
  static inline bool isCallbackRxNotEmptyCalled = false;
  static inline uint32_t valueSR = 0;

  static void Send(){
    if constexpr(!isTXviaDMA) base()->CR1 |= USART_CR1_TXEIE; // Enable transmit
    else 
      if (!(DMA_Channel_tx()->CCR & DMA_CCR1_EN)) EnableRxDMA();
  }

  static void EnableRxDMA(){
    DMA_Channel_tx()->CNDTR = txBuffer.GetCountToBufferLastIndex();
    DMA_Channel_tx()->CMAR = (uint32_t)(txBuffer.GetHeadAddress());
    txBuffer.SetHeadToLastIndex();
    DMA_Channel_tx()->CCR |= DMA_CCR1_EN;
  }

  static void CheckRxDMA(){
    size_t dmaCNTDR = DMA_Channel_rx()->CNDTR;
     if(rxBuffer_prevCount != dmaCNTDR){
      size_t diff = rxBuffer_prevCount > dmaCNTDR ? 
                    rxBuffer_prevCount - dmaCNTDR : 
                    rxBufferSize - dmaCNTDR + rxBuffer_prevCount;
      rxBuffer_prevCount = dmaCNTDR;
      rxBuffer.AddToTail(diff);
      if(!isCallbackRxNotEmptyCalled){
        isCallbackRxNotEmptyCalled = true;
        CallbackRxNotEmpty();
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

  static void InitPins(){
    constexpr uint8_t group = pinsGroup();
    _PinlistImplementation<
      _PinImplementation<uartPinsBase[group][0], uartPins[group][0], controller::pinConfiguration::Alternative_PushPull>,
      _PinImplementation<uartPinsBase[group][1], uartPins[group][1], controller::pinConfiguration::Input_PullUp>>::Init();
  }

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

  static constexpr uint8_t uartPins[][3] = {{9, 10, 0},
                                            {6, 7, 4},
                                            {2, 3, 0},
                                            {5, 6, 8},
                                            {10, 11, 0},
                                            {8, 9, 48},
                                            {10, 11, 16}};

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
    _ClockImplementation::APB2() : 
    _ClockImplementation::APB1()))/baud;
  }

public:
  using pins = utils::Typelist<
      _PinImplementation<uartPinsBase[pinsGroup()][0], uartPins[pinsGroup()][0], controller::pinConfiguration::Alternative_PushPull>,
      _PinImplementation<uartPinsBase[pinsGroup()][1], uartPins[pinsGroup()][1], controller::pinConfiguration::Input_PullUp>>;

  using power = utils::Typelist<utils::ctv<baseUART>, 
                                utils::ctv<uartPinsBase[pinsGroup()][0]>, 
                                utils::ctv<uartPinsBase[pinsGroup()][1]>, 
                                utils::ctv<AFIO_BASE>,
                                utils::ctv<isTXviaDMA || isRXviaDMA ? DMA1_BASE: 0U>>;

};

} //!namspace stm32f1::hardware

#endif //!_STM32F1_UART_HPP