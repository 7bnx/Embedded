# IPin

Pin common interface

## Methods
 
Implemented methods

|Num | Method                                  | Description                                               |
| -  | --------------------------------------- | --------------------------------------------------------- |
| 1  | void Init()                             | Initialization of pin's register, according to config     |
| 2  | void DeInit()                           | Reset pin to default configuration                        |
| 3  | void LowPower()                         | Configure pin to low power mode                           |
| 4  | void High()                             | Set pin to high-state                                     |
| 5  | void Low()                              | Set pin to low-state                                      |
| 6  | void Set(bool state)                    | Set the state of pin                                      |
| 7  | void Toggle()                           | Set pin opposite state                                    |
| 8  | bool Get()                              | Get the input state of pin                                |

## Pin properties

The Pin has the folloings properties

|Num | Property                                | Description                                            |
| -  | --------------------------------------- | ------------------------------------------------------ |
| 1  | interface                               | Interface of connection                                |
| 2  | mode::get                               | Get configuration of pin                               |
| 3  | mode::set<config, remap = remapDefault> | Set configuration of pin                               |
| 4  | mode::same< sourcePin >                 | Get source config and set it to destination pin        |
| 5  | mode::reconfigure<portID, pinNumber, config, remap = remapDefault> | Reconfigre all parameters of pin|

## Usage

```c++
...
  using pinLed1 = Pin::PC_13::mode::set<configuration::pin::Output_High>;
  using pinLed2 = Pin::PC_A0::mode::same<pinLed1>;
  pinLed1::Init();
  pinLed2::Init();
        ...
  pinLed1::Toggle();
  pinLed2::Low();
...
```