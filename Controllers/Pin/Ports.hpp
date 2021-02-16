//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Ports constructor 
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _PORTS_HPP
#define _PORTS_HPP

#include <cstddef>

namespace controller::constructor{

#define PIN(adapter, ID, Port, Number)\
using P##ID##_##Number = typename adapter::Pin<Port, Number>

#define PORT_16(ID, port)\
template<typename adapter>\
struct Port##ID##_16{\
\
  Port##ID##_16()=delete;\
\
  static constexpr size_t min = 0;\
  static constexpr size_t max = 15;\
  static constexpr size_t size = 16;\
\
  PIN(adapter, ID, port, 0);\
  PIN(adapter, ID, port, 1);\
  PIN(adapter, ID, port, 2);\
  PIN(adapter, ID, port, 3);\
  PIN(adapter, ID, port, 4);\
  PIN(adapter, ID, port, 5);\
  PIN(adapter, ID, port, 6);\
  PIN(adapter, ID, port, 7);\
  PIN(adapter, ID, port, 8);\
  PIN(adapter, ID, port, 9);\
  PIN(adapter, ID, port, 10);\
  PIN(adapter, ID, port, 11);\
  PIN(adapter, ID, port, 12);\
  PIN(adapter, ID, port, 13);\
  PIN(adapter, ID, port, 14);\
  PIN(adapter, ID, port, 15);\
\
};

class Ports{

public:

  PORT_16(A,0);
  PORT_16(B,1);
  PORT_16(C,2);
  PORT_16(D,3);
  PORT_16(E,4);
  PORT_16(F,5);
  PORT_16(G,6);
  PORT_16(H,7);
  PORT_16(I,8);
  PORT_16(J,9);

  PORT_16(0,0);
  PORT_16(1,1);
  PORT_16(2,2);
  PORT_16(3,3);
  PORT_16(4,4);
  PORT_16(5,5);
  PORT_16(6,6);
  PORT_16(7,7);
  PORT_16(8,8);
  PORT_16(9,9);

};

} // ! namespace controller::constructor

#undef PIN
#undef PORT_16

#endif //! _PORTS_HPP