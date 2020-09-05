//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Hardware-dependent driver for Power Control. STM32F1xx series
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _STM32F1_POWER_HPP
#define _STM32F1_POWER_HPP

#include <cstdint>
#include "stm32f10x.h"
#include "power.hpp"

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief Power Driver for STM32F1 series
*/  
class Power: public common::Power<Power>{

	Power() = delete;

	friend common::Power<Power>;

  /*!
    @brief Set peripherial Power.
    @tparam <uint32_t...periphBase> Variadic template parameter. Base address of enabling peripheria (e.g.: GPIOA_BASE)
  */
	template<uint32_t... periphBase>
	__attribute__ ((always_inline)) inline 
	static void _Set(bool state){
		if constexpr(constexpr auto value = _GetAHBENR<periphBase...>(); value)
			RCC->AHBENR = state ? RCC->AHBENR | value : RCC->AHBENR &(~value);
		if constexpr(constexpr auto value = _GetAPB1ENR<periphBase...>(); value)
			RCC->APB1ENR = state ? RCC->APB1ENR | value : RCC->APB1ENR &(~value);
		if constexpr(constexpr auto value = _GetAPB2ENR<periphBase...>(); value)
			RCC->APB2ENR = state ? RCC->APB2ENR | value : RCC->APB2ENR &(~value);
	}

  /*!
    @brief Set peripherial Power.
    @tparam <List...> Variadic template parameter. List of peripheral devices
  */
	template<typename... List>
	__attribute__ ((always_inline)) inline
	static void _Set(bool state){
	  using expandedList = utils::expand_t<typename List::power...>;

		if constexpr(constexpr auto value = powerValue<powerBus::AHB, expandedList>::value; value)
			RCC->AHBENR = state ? RCC->AHBENR | value : RCC->AHBENR &(~value);
		if constexpr(constexpr auto value = powerValue<powerBus::APB1, expandedList>::value; value)
			RCC->APB1ENR = state ? RCC->APB1ENR | value : RCC->APB1ENR &(~value);
		if constexpr(constexpr auto value = powerValue<powerBus::APB2, expandedList>::value; value)
			RCC->APB2ENR = state ? RCC->APB2ENR | value : RCC->APB2ENR &(~value);
	}

	template<uint32_t... periphBase>
	__attribute__ ((always_inline)) inline
	static constexpr uint32_t _GetAHBENR(){
		return (... | isAHBENR<periphBase>);
	}

	template<uint32_t... periphBase>
	__attribute__ ((always_inline)) inline
	static constexpr uint32_t _GetAPB1ENR(){
		return (...| isAPB1ENR<periphBase>);
	}

	template<uint32_t... periphBase>
	__attribute__ ((always_inline)) inline
	static constexpr uint32_t _GetAPB2ENR(){
		return (...| isAPB2ENR<periphBase>);
	}

	template<uint32_t base>
	static constexpr uint32_t isAHBENR = 
			( base == CRC_BASE 	? RCC_AHBENR_CRCEN :
				base == DMA1_BASE ? RCC_AHBENR_DMA1EN :
				base == SRAM_BASE ? RCC_AHBENR_SRAMEN : 
	#if defined (STM32F10X_HD) || defined  (STM32F10X_CL) || defined  (STM32F10X_HD_VL) || defined  (STM32F10X_XL)
				base == DMA2_BASE ? RCC_AHBENR_DMA2EN :
	#endif

	#if defined (STM32F10X_HD) || defined (STM32F10X_XL)
				base == FSMC_R_BASE ? RCC_AHBENR_FSMCEN :
				base == SDIO_BASE 	? RCC_AHBENR_SDIOEN :
	#endif

	#ifdef STM32F10X_HD_VL
				base == FSMC_R_BASE ? RCC_AHBENR_FSMCEN :
	#endif
	
	#ifdef STM32F10X_CL
				base == ETH_BASE 		? RCC_AHBENR_ETHMACEN:
	#endif
																									0U);

	template<uint32_t base>
	static constexpr uint32_t isAPB1ENR = 
		 (	base == BKP_BASE 		? RCC_APB1ENR_BKPEN :
				base == PWR_BASE 		? RCC_APB1ENR_PWREN :
				base == TIM2_BASE 	? RCC_APB1ENR_TIM2EN :
				base == TIM3_BASE 	? RCC_APB1ENR_TIM3EN :
				base == I2C1_BASE 	? RCC_APB1ENR_I2C1EN :
				base == USART2_BASE ? RCC_APB1ENR_USART2EN :
				base == WWDG_BASE 	? RCC_APB1ENR_WWDGEN : 

	#if !defined (STM32F10X_LD_VL) && !defined (STM32F10X_MD_VL) && !defined (STM32F10X_HD_VL)
				base ==  CAN1_BASE 	? RCC_APB1ENR_CAN1EN :
	#endif

	#if !defined (STM32F10X_LD) && !defined (STM32F10X_LD_VL)
				base == TIM4_BASE 	? RCC_APB1ENR_TIM4EN :
				base == SPI2_BASE 	? RCC_APB1ENR_SPI2EN :
				base == USART3_BASE ? RCC_APB1ENR_USART3EN :
				base ==  I2C2_BASE 	? RCC_APB1ENR_I2C2EN : 
	#endif

	#if defined (STM32F10X_HD) || defined  (STM32F10X_CL) || defined (STM32F10X_XL)
				base == TIM5_BASE 	? RCC_APB1ENR_TIM5EN :
				base == TIM6_BASE 	? RCC_APB1ENR_TIM6EN :
				base == TIM7_BASE 	? RCC_APB1ENR_TIM7EN :
				base == SPI3_BASE 	? RCC_APB1ENR_SPI3EN :
				base == UART4_BASE 	? RCC_APB1ENR_UART4EN :
				base == UART5_BASE 	? RCC_APB1ENR_UART5EN : 
				base == DAC_BASE 		? RCC_APB1ENR_DACEN : 
	#endif

	#if defined (STM32F10X_LD_VL) || defined  (STM32F10X_MD_VL) || defined  (STM32F10X_HD_VL)
				base == TIM6_BASE ? RCC_APB1ENR_TIM6EN :
				base == TIM7_BASE ? RCC_APB1ENR_TIM7EN :
				base == DAC_BASE 	? RCC_APB1ENR_DACEN :
				base == CEC_BASE 	? RCC_APB1ENR_CECEN :

	#endif

	#ifdef STM32F10X_HD_VL
				base == TIM5_BASE 	? RCC_APB1ENR_TIM5EN :
				base == TIM12_BASE 	? RCC_APB1ENR_TIM12EN :
				base == TIM13_BASE 	? RCC_APB1ENR_TIM13EN :
				base == TIM14_BASE 	? RCC_APB1ENR_TIM14EN : 
				base == SPI3_BASE 	? RCC_APB1ENR_SPI3EN :
				base == UART4_BASE 	? RCC_APB1ENR_UART4EN :
				base == UART5_BASE 	? RCC_APB1ENR_UART5EN : 
	#endif

	#ifdef STM32F10X_CL
				base == CAN2_BASE ? RCC_APB1ENR_CAN2EN :
	#endif

	#ifdef STM32F10X_XL
				base == TIM12_BASE ? RCC_APB1ENR_TIM12EN :
				base == TIM13_BASE ? RCC_APB1ENR_TIM13EN :
				base == TIM14_BASE ? RCC_APB1ENR_TIM14EN : 
	#endif
																									0U);

	template<uint32_t base>
	static constexpr uint32_t isAPB2ENR = 
		 ( base == ADC1_BASE 	? RCC_APB2ENR_ADC1EN :
			base == AFIO_BASE 	? RCC_APB2ENR_AFIOEN :
			base == GPIOA_BASE 	? RCC_APB2ENR_IOPAEN : 
			base == GPIOB_BASE 	? RCC_APB2ENR_IOPBEN :
			base == GPIOC_BASE 	? RCC_APB2ENR_IOPCEN :
			base == GPIOD_BASE 	? RCC_APB2ENR_IOPDEN :
			base == SPI1_BASE 	? RCC_APB2ENR_SPI1EN :
			base == TIM1_BASE 	? RCC_APB2ENR_TIM1EN :
			base == USART1_BASE ? RCC_APB2ENR_USART1EN : 
	
	#if !defined (STM32F10X_LD_VL) && !defined (STM32F10X_MD_VL) && !defined (STM32F10X_HD_VL)
				base == ADC2_BASE ? RCC_APB2ENR_ADC2EN :
	#endif

	#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
				base == TIM15_BASE ? RCC_APB2ENR_TIM15EN :
				base == TIM16_BASE ? RCC_APB2ENR_TIM16EN :
				base == TIM16_BASE ? RCC_APB2ENR_TIM17EN : 
	#endif

	#if !defined (STM32F10X_LD) && !defined (STM32F10X_LD_VL)
				base == GPIOE_BASE ? RCC_APB2ENR_IOPEEN :	
	#endif

	#if defined (STM32F10X_HD) || defined (STM32F10X_XL)
				base == GPIOF_BASE ? RCC_APB2ENR_IOPFEN : 
				base == GPIOG_BASE ? RCC_APB2ENR_IOPGEN :
				base == TIM8_BASE ? RCC_APB2ENR_TIM8EN :
				base == ADC3_BASE ? RCC_APB2ENR_ADC3EN :
  #endif          

	#ifdef STM32F10X_HD_VL
				base == GPIOF_BASE ? RCC_APB2ENR_IOPFEN : 
				base == GPIOG_BASE ? RCC_APB2ENR_IOPGEN :
	#endif

	#ifdef STM32F10X_XL
				base == TIM9_BASE ? RCC_APB2ENR_TIM9EN :
				base == TIM10_BASE ? RCC_APB2ENR_TIM10EN :
				base == TIM11_BASE ? RCC_APB2ENR_TIM11EN : 
	#endif
																									0U);

	enum class powerBus{
		AHB,
		APB1,
		APB2
	};

	template<powerBus bus, auto value>
  static constexpr uint32_t busValue = bus == powerBus::AHB  ?  isAHBENR<value> : 
																			 bus == powerBus::APB1 ?  isAPB1ENR<value> :
																			 bus == powerBus::APB2 ?	isAPB2ENR<value> : 0U;

	template<powerBus bus, typename Head>
	struct powerValue{
		static constexpr uint32_t value = busValue<bus, Head::value>;
	};
	
	template<powerBus bus, typename Head, typename... List>
	struct powerValue<bus, utils::Typelist<Head, List...>>{
		using next = utils::Typelist<List...>;
		static constexpr uint32_t frontValue = Head::value;
		static constexpr uint32_t value = busValue<bus, frontValue> | powerValue<bus,next>::value;
	};
	
	template<powerBus bus>
	struct powerValue<bus, utils::Typelist<>>{
		static constexpr uint32_t value = 0U;
	};

	template<powerBus bus, typename... List>
	static constexpr uint32_t powerValue_v = powerValue<bus, utils::Typelist<List...>>::value;
};

} // !controller

#endif // !_STM32F1_POWER_HPP