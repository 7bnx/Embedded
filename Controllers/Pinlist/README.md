# Pinlist
List of pins. Common Interface

## Template

```c++
template<typename... Pins>
```

|Num | Parameter    | Description                                                                                   |
| -  | ------------ | --------------------------------------------------------------------------------------------- |
| 1  | Pins...      | Variadic template parameter. List of comma-separated single-pins or other pinlist.            |


## Interface

### Methods

|Num | Method                                            | Description                                               |
| -  | ------------------------------------------------- | --------------------------------------------------------- |
| 1  | void Init()                                       | Initialize list of pins                                   |
| 2  | void DeInit()                                     | Reset pins to default state                               |
| 3  | void LowPower()                                   | Set pins in list to low power mode                        |
| 4  | void Write(uint32_t value)                        | Write value to pins in list                               |
| 5  | void Write<uint32_t value>()                      | Write const value to pins in list                         |
| 6  | void High()                                       | Set all pins in list to High-level                        |
| 7  | void Low()                                        | Set all pins in list to Low-level                         |
| 8  | void SetPin< pinNumber >(bool state)              | Set pin-level to state                                    |
| 9  | uint32_t Read()                                   | Get states of pins in list                                |
| 10 | bool ReadPin< pinNumber >()                       | Get state of one pin in list                              |
| 11 | get_pin< pinNumber >                              | Read the state of pins in list                            |

### Properties

|Num | Property                      | Description                                                                              |
| -  | ----------------------------- | ---------------------------------------------------------------------------------------- |
| 1  | size                          | Number of pins in list                                                                   |
| 2  | interface                     | Interface of connection                                                                  |
| 3  | get_pin< pinNumber >          | Get pin from list by number                                                              |
| 4  | pop_front_pins< pinNumber >   | Get pinlist without poped pins from front                                                |
| 5  | pop_back_pins< pinNumber >    | Get pinlist without poped pins from back                                                 |
| 6  | mode::same< sourcePin >       | Set config of source to all pins in list                                                 |
| 7  | mode::set< config >           | Set config to all pins in list                                                           |
| 8  | generate::complement          | Complement all ports, used in Pinlist with rest pins                                     |
| 9  | generate::row< numberPinEnd > | Generate row of pins from pinlist[0] - with 'number' and configuration till numberPinEnd |

## Usage

```cpp
...

using namespace controller;
...
using list1 = Pinlist<
                      Pin::PA_1::mode::set<configuration::pin::Output_Low_50MHz>, //A1
                      Pin::PA_0::mode::set<configuration::pin::Output_Low_50MHz>, //A0
                      Pin::PB_9::mode::set<configuration::pin::Output_Low_50MHz>  //B9
                    >;

Power::Enable<list1>(); // Enable Clock for GPIOA and GPIOB
list1::Init();
list1::Write(4); // A1 = 1, A0 = 0, B9 = 0
list1::Write(3); // A1 = 0, A0 = 1, B9 = 1
list1::Write(2); // A1 = 0, A0 = 1, B9 = 0
list1::Write<0>(true); // A1 = 0, A0 = 1, B9 = 1
using A2 = Pin::PA_2::mode::set<configuration::pin::Output_Low_50MHz>;

using list2 = Pinlist<
                      A2,
                      list1
                     >;

list2::Write(13); // A2 = 1,  A1 = 1, A0 = 0, B9 = 1

...
```