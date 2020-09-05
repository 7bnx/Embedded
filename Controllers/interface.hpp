//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Controllers interfaces
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _INTERFACE_HPP
#define _INTERFACE_HPP

/*!
  @brief Controller's peripherals devices
*/
namespace controller{

/*!
  @brief Controllers interfaces
*/
enum class interface{

  /*! @brief Pins or Pinlist*/ 
  parallel,
  
  /*! @brief UART or USART*/ 
  uart,
  
  /*! @brief SPI*/ 
  spi,
  
  /*! @brief I2C*/ 
  i2c
};

} // !namspace controller

#endif // !_INTERFACE_HPP