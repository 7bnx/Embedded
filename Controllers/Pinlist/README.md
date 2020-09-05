# Pinlist
Operate with list of pins. The pins can belong to different ports

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
| 2  | void Reset()                                      | Reset pins to default state                               |
| 3  | void ToLowPowerMode()                             | Set pins in list to low power mode                        |
| 4  | void High()                                       | Set all pins in list to High-level                        |
| 5  | void Low()                                        | Set all pins in list to Low-level                         |
| 6  | void Write(uint32_t value)                        | Write value to pins in list                               |
| 7  | void Write<*pinNumber*>(bool state)                 | Set pin-level to state                                    |
| 8  | operator= (uint32_t value)                        | Write value to pins in list                               |
| 9  | uint32_t Read()                                   | Read the state of pins in list                            |
| 10 | operator uint32_t()                               | Read the state of pins in list                            |

### Properties

|Num | Property     | Description                                               |
| -  | ------------ | --------------------------------------------------------- |
| 1  | size         | Number of pins in list                                    |
| 2  | interface    | Parallel interface                                        |


## Static Asserts

|Num | Assert            | Description                                                                      |
| -  | ----------------- | -------------------------------------------------------------------------------- |
| 1  | pin number > 16   | Number of pin should be less, then 16 (0 ... 15)                                 |
| 2  | base port address | Wrong base address of port. E.g.: GPIOA_BASE, GPIOB_BASE                         |
| 3  | unique pins       | Pin list has the repetitive numbers (equals ports base address && number of pin) |

## Usage

```cpp
...

using namespace controller;
...
using list1 = Pinlist<
                      Pin<GPIOA_BASE, 1, pinConfiguration::Output_Low_50MHz>, //A1
                      Pin<GPIOA_BASE, 0, pinConfiguration::Output_Low_50MHz>, //A0
                      Pin<GPIOB_BASE, 9, pinConfiguration::Output_Low_50MHz>  //B9
                    >;

Power::Enable<list1>(); // Enable Clock for GPIOA and GPIOB
list1::Init();
list1::Write(4); // A1 = 1, A0 = 0, B9 = 0
list1::Write(3); // A1 = 0, A0 = 1, B9 = 1
list1::Write(2); // A1 = 0, A0 = 1, B9 = 0
list1::Write<0>(true); // A1 = 0, A0 = 1, B9 = 1
using A2 = Pin<GPIOA_BASE, 2, pinConfiguration::Output_Low_50MHz>;

using list2 = Pinlist<
                      A2,
                      list1
                     >;

list2::Write(13); // A2 = 1,  A1 = 1, A0 = 0, B9 = 1

Pinlist<list1, A2> list3;
list3 = 12; // A1 = 1, A0 = 1, B9 = 0, A2 = 0
...
```