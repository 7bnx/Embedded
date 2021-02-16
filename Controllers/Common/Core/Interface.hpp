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

  /*! @brief Pin*/ 
  SERIAL,
  
  /*! @brief Pins or Pinlist*/ 
  PARALLEL,

  /*! @brief UART or USART*/ 
  UART,
  
  /*! @brief SPI*/ 
  SPI,
  
  /*! @brief I2C*/ 
  I2C
};

} // !namspace controller

#endif // !_INTERFACE_HPP