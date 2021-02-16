//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  DMA of STM32F1
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _STM32F1_DMA_HPP
#define _STM32F1_DMA_HPP

#include <cstdint>
#include "../Common/Compiler/Compiler.h"
#include "../Common/Core/Registers.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief Configuration fo controllers devices
*/
namespace configuration::dma{

/*!
  @brief Size of DMA Element
*/
enum class data_size{
  /*! @brief 8-bit element*/
  Size_8 = 0,
  /*! @brief 16-bit element*/
  Size_16 = 0x500,
  /*! @brief 32-bit element*/
  Size_32 = 0xA00
};

/*!
  @brief Memory increment
*/
enum class minc{
  MINC_Disabled = 0,
  MINC_Enabled  = 0x80
};

/*!
  @brief Peripheral increment
*/
enum class pinc{
  PINC_Disabled = 0,
  PINC_Enabled  = 0x40
};

/*!
  @brief Circular mode
*/
enum class circ{
  CIRC_Disabled = 0,
  CIRC_Enabled  = 0x20
};

/*!
  @brief Direction
*/
enum class dir{
  /*! @brief Read from peripheral*/
  DIR_ToMemory = 0,
  /*! @brief Read from memory*/
  DIR_ToPeripheral  = 0x10
};

/*!
  @brief Interrupt enabling
*/
enum class isr{
  /*! @brief No interrupts*/
  ISR_None = 0,
  /*! @brief Transfer complete interrupt*/
  ISR_TC = 2,
  /*! @brief Half transfer interrupt*/
  ISR_HT = 4,
  /*! @brief Transfer complete  and half transfer interrupts*/
  ISR_TC_HT = 6
};

} // !namespace configuration::dma


template<uint8_t dmaID, uint8_t channel>
class DMA : public controller::hardware::Registers{

public:

  /*!
    @brief Initialization of DMA channel
    @tparam <enable> enable channel after initialization
    @tparam <minc> memory increment mode
    @tparam <pinc> peripheral increment mode
    @tparam <dir> direction of read/write
    @tparam <circ> circular mode
    @tparam <isr> interruprts to enable
    @tparam <mSize> size of element in memory
    @tparam <pSize> size of peripherals element
  */
  template<bool enable, configuration::dma::minc minc, configuration::dma::pinc pinc, 
           configuration::dma::dir dir, configuration::dma::circ circ, configuration::dma::isr isr,
           configuration::dma::data_size mSize = configuration::dma::data_size::Size_8,
           configuration::dma::data_size pSize = configuration::dma::data_size::Size_8>
  __FORCE_INLINE static void Init(){
    static constexpr uint32_t value = (uint32_t)minc | (uint32_t)pinc |
                                      (uint32_t)circ | (uint32_t)dir  |
                                      (enable ? mask::CCR::EN : 0) | (uint32_t)isr |
                                      ((uint32_t)mSize & 0x300) | ((uint32_t)pSize & 0xC00);
    Registers::_Write<address::CCR, value>();
  }

  /*!
    @brief Set peripherals address
    @tparam <address> address to set
  */
  template<uint32_t peripheralAddress>
  __FORCE_INLINE static void SetPeripheral(){
    Registers::_Write<address::CPAR, peripheralAddress>();
  }

  /*!
    @brief Set peripherals address
    @param [in] address to set
  */
  __FORCE_INLINE static void SetPeripheral(uint32_t peripheralAddress){
    Registers::_Write<address::CPAR>(peripheralAddress);
  }

  /*!
    @brief Set memory address
    @tparam <address> address to set
  */
  template<uint32_t memoryAddress>
  __FORCE_INLINE static void SetMemory(){
    Registers::_Write<address::CMAR, memoryAddress>();
  }
  
  /*!
    @brief Set memory address
    @param [in] address to set
  */
  __FORCE_INLINE static void SetMemory(uint32_t memoryAddress){
    Registers::_Write<address::CMAR>(memoryAddress);
  }

  /*!
    @brief Set count of elements to transfer
    @tparam <count> of elements
  */
  template<size_t count>
  __FORCE_INLINE static void SetCount(){
    Registers::_Write<address::CNTDR, count>();
  }

  /*!
    @brief Set count of elements to transfer
    @param [in] count of elements
  */
  __FORCE_INLINE static void SetCount(size_t count){
    Registers::_Write<address::CNTDR>(count);
  }

  /*!
    @brief Get count of elements to transfer
    @return count of elements
  */
  __FORCE_INLINE static size_t GetCount(){
    return Registers::_Read<address::CNTDR>();
  }

  /*!
    @brief Clear interrupts flag
  */
  __FORCE_INLINE static void ClearFlags(){
    Registers::_Write<address::IFCR, mask::IFCR::CLEAR>();
  }

  /*!
    @brief Disable channel
  */
  __FORCE_INLINE static void Disable(){
    Registers::_Set<address::CCR, 0, mask::CCR::EN>();
  }

  /*!
    @brief Enable channel
  */
  __FORCE_INLINE static void Enable(){
    Registers::_Set<address::CCR, mask::CCR::EN>();
  }

  /*!
    @brief Check current state of channel
    @return state of channel
  */
  __FORCE_INLINE static bool IsEnabled(){
    return bool(Registers::_Read<address::CCR, mask::CCR::EN>());
  }

  /*!
    @brief Check transfer complete flag
    @return state of channel
  */
  __FORCE_INLINE static bool IsTransferComplete(){
    return bool(Registers::_Read<address::ISR, mask::ISR::TC>());
  }

  /*!
    @brief Check transfer complete flag
    @return state of channel
  */
  __FORCE_INLINE static bool IsHalfTransfer(){
    return bool(Registers::_Read<address::ISR, mask::ISR::HT>());
  }

private:

  struct address{
    static constexpr uint32_t
      base  = 0x40020000 + (dmaID == 1 ? 0 : 0x400),
      ISR  = base,
      IFCR = base + 4,
      CCR  = base + 8 + 20*(channel - 1),
      CNTDR  = base + 12 + 20*(channel - 1),
      CPAR  = base + 16 + 20*(channel - 1),
      CMAR  = base + 20 + 20*(channel - 1);
  };

  struct mask{
    struct CCR{
      static constexpr uint32_t
        EN = 1, // Enable
        MINC = 1 << 7, // Peripheral increment
        PINC = 1 << 6, // Circular mode
        DIR = 1 << 4, // Direction
        HTIE = 1 << 2, // Half-transfer complete interrupt enable
        TCIE = 1 <<1; // Transfer complete interrupt enable
    };
    struct ISR{
      static constexpr uint32_t
        TC = 1 << ((channel - 1)*4 + 1),
        HT = 1 << ((channel - 1)*4 + 2);
    };
    struct IFCR{
      static constexpr uint32_t
        CLEAR = 0xF <<  4*(channel - 1);
    };
  };

};

} // !namespace controller

#endif // !_STM32F1_DMA_HPP