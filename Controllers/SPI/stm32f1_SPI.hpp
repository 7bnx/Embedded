//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Hardware-dependent driver for SPI. STM32F1-series 
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _STM32F1_SPI_HPP
#define _STM32F1_SPI_HPP

#include <cstdint>
#include <cstddef>
#include "stm32f10x.h"
#include "stm32f1_Pinlist.hpp"
#include "stm32f1_Power.hpp"
#include "connection.hpp"
#include "interrupt.hpp"
#include "type_traits_custom.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief Possible clock divisors of SPI
*/
enum class spiDivisor{
  SPI_DIV_2	= (0 << 3),  //!< divisor = 2
	SPI_DIV_4	= (1 << 3),  //!< divisor = 4
	SPI_DIV_8	= (2 << 3),  //!< divisor = 8
	SPI_DIV_16	= (3 << 3),  //!< divisor = 16
	SPI_DIV_32	= (4 << 3),  //!< divisor = 32
	SPI_DIV_64	= (5 << 3),  //!< divisor = 64
	SPI_DIV_128	= (6 << 3),  //!< divisor = 128
	SPI_DIV_256	= (7 << 3)   //!< divisor = 256
};

/*!
  @brief SPI driver
  @tparam <baseSPI> base address of SPI
  @tparam <txBufferSize> size for tx buffers
  @tparam <rxBufferSize> size for rx buffers
  @tparam <isMaster> set spi to master mode
  @tparam <isTXviaDMA> if true, then data transmits via DMA, otherwise via SPI Interrupt
  @tparam <isRXviaDMA> if true, then data receives via DMA, otherwise via SPI Interrupt
  @tparam <isPinsRemap> if true, then TX and RX pins will be remaped
*/  
template<uint32_t baseSPI, size_t txBufferSize, size_t rxBufferSize, bool isMaster,
         bool isTXviaDMA = true, bool isRXviaDMA = true, bool isPinsRemap = false>
class SPI: public common::connection<uint8_t, txBufferSize, rxBufferSize, 
                  SPI<baseSPI, txBufferSize, rxBufferSize, isMaster, isTXviaDMA, isRXviaDMA, isPinsRemap>>{

private:
  using baseConnection = common::connection<uint8_t, txBufferSize, rxBufferSize, 
                    SPI<baseSPI, txBufferSize, rxBufferSize, isMaster, isTXviaDMA, isRXviaDMA, isPinsRemap>>;

public:
  /*!
    @brief Initialization of SPI
    @tparam <divisor> of spi clock source
    @tparam <isPolarityHigh> Clock is high, when IDLE
    @tparam <isFirstEdge> The first clock transition is the first data capture edge
  */  
  template<spiDivisor divisor, bool isPolarityHigh = true, bool isFirstEdge = true>
  static void Init(){
    base()->CR2 = SPI_CR2_ERRIE                  | // Enable Error interrupt 
                  isTXviaDMA << 1U               | // Enable TX DMA
                  isRXviaDMA << 0U               | // Enable RX DMA
                 !isRXviaDMA << 6U;                // Enable RX interrupr
    base()->CR1 = SPI_CR1_SPE                    | // Enable SPI 
                  isMaster << 2U                 | // Master or Slave configuration
                  SPI_CR1_SSM                    | // Enable software slave managment 
                  SPI_CR1_SSI                    | // Force slave select to active
                  static_cast<uint32_t>(divisor) | // Set spi clock divisor
                 !isPolarityHigh << 1U           | // If true -> CLCK IDLE state is high
                 !isFirstEdge << 0U;               // If true -> capture edge is second
    if constexpr (isTXviaDMA || isRXviaDMA){
      if constexpr(isRXviaDMA){
        DMA_Channel_rx()->CPAR = baseSPI + 0x0CU;
        DMA_Channel_rx()->CMAR = reinterpret_cast<uint32_t>(baseConnection::rxBuffer.GetHeadAddress());
        DMA_Channel_rx()->CNDTR = rxBufferSize;
        DMA_Channel_rx()->CCR  =  DMA_CCR1_EN // Enable DMA channel
                               |  DMA_CCR1_MINC // Memory increment
                               |  DMA_CCR2_CIRC; // Enable circular mode
      }
      if constexpr(isTXviaDMA){
        DMA_Channel_tx()->CPAR = baseSPI + 0x0CU;
        DMA_Channel_tx()->CCR  =  DMA_CCR1_MINC // Memory increment
                               |  DMA_CCR1_DIR  // Direction - to peripheria
                               |  DMA_CCR1_TCIE;// Full Transfer complete ISR
      }
    }
  }

  /*!
    @brief Get SPI Errors
  */
  static const auto& GetErrors(){
    if constexpr (isTXviaDMA && isRXviaDMA)
      valueSR = base()->SR;
    error.master =    valueSR & SPI_SR_MODF;
    error.overrun =   valueSR & SPI_SR_OVR;
    error.crc =       valueSR & SPI_SR_CRCERR;
    error.underrun =  valueSR & SPI_SR_UDR;
    return error;
  }

  /*!
    @brief Clear SPI Errors
  */
  static void ClearErrors(){
    (void) base()->SR;
    (void) base()->DR;
    if (error.master)
      base()->CR1 |= SPI_CR1_SPE | (isMaster << 2);
    valueSR = 0;
  }

  /*!
    @brief DMA TX Handler
  */
  static void ISR_DMA_TX(){
    ((DMA_TypeDef *)DMA_BASE)->IFCR = dmaIsrClearValue<DMA_Channel_TX_Number>;;
    DMA_Channel_tx()->CCR &= ~DMA_CCR1_EN;
    if (!baseConnection::txBuffer.IsEmpty())
      EnableTxDMA();
    else if (!__NVIC_GetEnableIRQ(spiIRQn)) baseConnection::CallbackTxIdle();
    if constexpr (isRXviaDMA){
      if(!baseConnection::isCallbackRxNotEmptyCalled){
        CheckRxDMA();
        baseConnection::CallbackRxNotEmpty();
        baseConnection::isCallbackRxNotEmptyCalled = true;
      }
    }
  }

  /*!
    @brief ISR Handler
  */
  __attribute__ ((always_inline)) inline
  static void ISR(){
    uint32_t CR2 = base()->CR2;
	  valueSR =  base()->SR;
    uint16_t valueDR = base()->DR;

    if (CR2 & SPI_CR2_TXEIE && valueSR & SPI_SR_TXE){
      if (!baseConnection::txBuffer.IsEmpty()) base()->DR = baseConnection::txBuffer.Pop();
      else {
        base()->CR2 &=~ SPI_CR2_TXEIE;
        baseConnection::CallbackTxIdle();
      }
	  }

    if (CR2 & SPI_CR2_RXNEIE && valueSR & SPI_SR_RXNE){
      baseConnection::rxBuffer.Push(valueDR);
      if (!baseConnection::isCallbackRxNotEmptyCalled){
        baseConnection::CallbackRxNotEmpty();
        baseConnection::isCallbackRxNotEmptyCalled = true;
      }
    }

    if(valueSR & (SPI_SR_OVR | SPI_SR_MODF | SPI_SR_CRCERR | SPI_SR_UDR)){
      baseConnection::CallbackError();
    }

    if constexpr (isRXviaDMA){
      if(!baseConnection::isCallbackRxNotEmptyCalled){
        CheckRxDMA();
        baseConnection::CallbackRxNotEmpty();
        baseConnection::isCallbackRxNotEmptyCalled = true;
      }
    }
  }

private:

  friend Power;
  friend Interrupt;
  template<typename...>
  friend class Pinlist;
  friend baseConnection;

  struct errorSPI{
    bool master;
    bool overrun;
    bool crc;
    bool underrun;
  };

  static inline errorSPI error;
  static inline size_t rxBuffer_prevCount = rxBufferSize;
  static inline uint32_t valueSR = 0;

  __attribute__ ((always_inline)) inline
  static auto constexpr base() {return (SPI_TypeDef*)baseSPI;}

  template<uint8_t channelNumber>
  static constexpr auto dmaIsrClearValue = 15U << (channelNumber)*4;

  static void Send(){
    if constexpr(!isTXviaDMA) 
      base()->CR2 |= SPI_CR2_TXEIE; // Enable transmit
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
     if(rxBuffer_prevCount != dmaCNTDR){
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

  static constexpr auto DMA_BASE = baseSPI == SPI3_BASE ? DMA2_BASE : DMA1_BASE;

  __attribute__ ((always_inline)) inline
  static constexpr auto DMA_Channel_rx(){ 
    return (DMA_Channel_TypeDef *)(DMA_BASE + 8U + 20*DMA_Channel_RX_Number);
  }  

  __attribute__ ((always_inline)) inline
  static constexpr auto DMA_Channel_tx(){ 
    return (DMA_Channel_TypeDef *)(DMA_BASE + 8U + 20*DMA_Channel_TX_Number);
  }

  static constexpr uint32_t DMA_Channel_TX_Number =(
                                        baseSPI == SPI1_BASE ? 3: 
                                        baseSPI == SPI2_BASE ? 5: 2) - 1;

  static constexpr uint32_t DMA_Channel_RX_Number = (
                                        baseSPI == SPI1_BASE ? 2: 
                                        baseSPI == SPI2_BASE ? 4: 1) - 1;

  __attribute__ ((always_inline)) inline
  static constexpr uint8_t pinsGroup(){
    if (baseSPI == SPI1_BASE){
      if (!isPinsRemap) return 0;
      else return 1;
    } else if (baseSPI == SPI2_BASE){
      return 2;
    } else if (baseSPI == SPI3_BASE){
      if (!isPinsRemap) return 3;
      else return 4;
    }
  }
                                          //NSS, SCK, MISO, MOSI
  static constexpr uint8_t spiPins[][4] = { {4, 5, 6, 7}, // GPIOA
                                            {15, 3, 4, 5}, // 1 - GPIOA, rest - GPIOB
                                            {12, 13, 14, 15}, // GPIOB
                                            {15, 3, 4, 5}, // 1 - GPIOA, rest - GPIOB
                                            {4, 10, 11, 12}}; // 1 - GPIOA, rest GPIOC

  static constexpr uint32_t pinsRemapValue = baseSPI == SPI1_BASE ? 1 | AFIO_MAPR_SWJ_CFG_JTAGDISABLE :
                                                                        0x10000000;

  static constexpr uint32_t spiPinsBase[][2] = { {GPIOA_BASE, GPIOA_BASE},
                                                 {GPIOA_BASE, GPIOB_BASE}, 
                                                 {GPIOB_BASE, GPIOB_BASE},
                                                 {GPIOA_BASE, GPIOB_BASE},
                                                 {GPIOA_BASE, GPIOC_BASE},};

  static constexpr auto spiIRQn = baseSPI == SPI1_BASE ? IRQn::SPI1_IRQn :
    #if defined (STM32F10X_HD) || (STM32F10X_HD_VL) || (STM32F10X_XL) || (STM32F10X_CL)
                                  baseSPI == SPI3_BASE ? IRQn::SPI3_IRQn : 
    #endif  
                                                         IRQn::SPI2_IRQn;

  static constexpr auto spiDMAtxIRQn = baseSPI == SPI1_BASE ? IRQn::DMA1_Channel3_IRQn :
    #if defined (STM32F10X_HD) || (STM32F10X_HD_VL) || (STM32F10X_XL) || (STM32F10X_CL)
                                       baseSPI == SPI3_BASE ? IRQn::DMA2_Channel2_IRQn : 
    #endif  
                                                              IRQn::DMA1_Channel5_IRQn;

  static constexpr auto spiDMArxIRQn = baseSPI == SPI1_BASE ? IRQn::DMA1_Channel2_IRQn :
    #if defined (STM32F10X_HD) || (STM32F10X_HD_VL) || (STM32F10X_XL) || (STM32F10X_CL)
                                       baseSPI == SPI3_BASE ? IRQn::DMA2_Channel1_IRQn : 
    #endif  
                                                              IRQn::DMA1_Channel4_IRQn;
  
  using power = utils::Typelist<
                                utils::ctv<baseSPI>, 
                                utils::ctv<spiPinsBase[pinsGroup()][0]>, 
                                utils::ctv<spiPinsBase[pinsGroup()][1]>, 
                                utils::ctv<AFIO_BASE>,
                                utils::ctv<isTXviaDMA || isRXviaDMA ? DMA_BASE: 0U>
                              >;

  using interrupts = utils::Typelist< 
                                      std::conditional_t<isTXviaDMA, utils::ctv<spiDMAtxIRQn>, utils::ctv<spiIRQn>>
                                    >;

  using pins = utils::Typelist<
      Pin<spiPinsBase[pinsGroup()][1], spiPins[pinsGroup()][1], pinConfiguration::Alternative_PushPull>,
      Pin<spiPinsBase[pinsGroup()][1], spiPins[pinsGroup()][2], pinConfiguration::Input_Float>,
      Pin<spiPinsBase[pinsGroup()][1], spiPins[pinsGroup()][3], pinConfiguration::Alternative_PushPull>>;


  using remap = utils::ctv<pinsRemapValue>;

  static_assert(baseSPI == SPI1_BASE || baseSPI == SPI2_BASE || baseSPI == SPI3_BASE,
               "baseSPI should be one of SPI1_BASE, SPI2_BASE, SPI3_BASE");

  static_assert(!(baseSPI == SPI2_BASE && isPinsRemap),
                "Pins of SPI2 can't be remapped");

};

} // !namespace controller

#endif // !_STM32F1_SPI_HPP