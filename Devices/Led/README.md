# Simple Led driver 

Implementation of Led object for uC.

Language 'C++17'.

## Methods

|Num | Method                   | Description                                                                           |
| -  | -------------------------| ------------------------------------------------------------------------------------- |
| 1  | static void Handler()    | Led Handler. Call it in super loop, or task. Each call increments Led's state counter |
| 2  | static void Blink(size_t timeOn, size_t timeOff, size_t qtyBlinks) | Turns LED to "Blink mode". Led immediately goes to ON |
| 3  | static void BlinkRow(size_t timeOn, size_t timeOff, size_t qtyBlinks, size_t timePause, size_t qtyBlinksRow) | Turns LED to "BlinkRow mode". It makes pause between blinking sequence |
| 4  | static void Set(bool state)       | Turns LED to preferred state. Resets Blink and BlinkRow parameters           |
| 5  | static void On()                  | Turns LED to permanent ON. Resets Blink and BlinkRow parameters              |
| 6  | static void Off()                 | Turns LED's to permanent OFF. Resets Blink and BlinkRow parameters           |
| 7  | static void Toggle()              | Changes emitting state of LED. Resets Blink and BlinkRow parameters          |
| 8  | static bool IsOn()                | Returns current emitting state of LED                                        |
| 9  | static void Enable(bool state)    | Enables or disables LED. When disabled, another methods take no effect       |
| 9  | static IsEnable()                 | Returns the enable-state of LED                                              |
| 10 | operator bool()                   | Use in conditional statements. E.g. : if (led) {...}                         |
| 11 | bool operator= (bool &state)      | Assigning the state to the emitting-state of led                             |

## Template

There are 2 parameters in led's template

```c++
template<class Pin, bool isOnHigh = true>
```

|Num | Parameter  | Description                                                                   |
| -  | -----------| ----------------------------------------------------------------------------- |
| 1  | Pin        | Hardware-dependent output. Must implements field 'bool Pin::Set(bool state)   |
| 2  | isOnHigh   | if true - Led goes on, when pin is High                                       |

## Usage

```c++
...
  using led = Led<pinLed, true>;
        ... 
  led::Blink(1, 3, 4);
  while (1){
    led::Handler();
    // Led out: | _ _ _ | _ _ _ | _ _ _ | _ _ _
    // | - Led ON
    // _ - Led OFF
  }
...
```

```c++
...
  Led<pinLed, false> led;
        ...    
    
  while (1){
        ...
    led.Toggle(); 
        ...
  }
...
```

```c++
...
  template<class Pin, bool activeState = High>
  using ledTemplate = Led<Pin, activeState, countDebounce, countLongPress>;
        ...
  ledTemplate<pinLedRed> ledRed;
  ledTemplate<pinLedGreen> ledGreen;
        ... 
  ledGreen.On();
  ledRed::BlinkRow(1, 3, 4, 5, 2);
  while (1){
    ledRed.Handler();
    // ledRed out: | _ _ _ | _ _ _ | _ _ _ | _ _ _  .....  | _ _ _ | _ _ _ | _ _ _ | _ _ _
    // | - Led ON
    // _ - Led OFF
    // . - pause
  }
...
```