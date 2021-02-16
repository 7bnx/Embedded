# Simple Button driver 

Implementation of driver for Single Button.

Language 'C++17'.

## Methods

|Num | Method                                  | Description                                                                  |
| -  | --------------------------------------- | ---------------------------------------------------------------------------- |
| 1  | static void Handler()                   | Call it in super loop, or task. Each call increments Buttons's state counter |
| 2  | static void Enable(bool state = true)   | Enables or disables Button                                                   |
| 3  | static bool IsEnabled()                 | Returns the enable state of Button                                           |
| 4  | static bool IsPressed()                 | Returns the press state of Button                                            |
| 5  | static bool IsPressedLong()             | Returns the long press state of Button                                       |
| 6  | static  void ResetCallbacks()           | Clears Callbacks functions                                                   |
| 7  | operator bool() const                   | Use in conditional statements. E.g. : if (button) {...}                      |

## Callbacks

This driver implements the following events:

|Num | Event                                   | Description                                                         |
| -  | --------------------------------------- | ------------------------------------------------------------------- |
| 1  | CallbackPressed                         | Executes when the button is pressed                                 |
| 2  | CallbackPressedLong                     | Executes when the button is long pressed                            |
| 3  | CallbackReleased                        | Executes when the button is released                                |

## Template

There are 4 parameters in button's template

```c++
template<class Pin, bool isActiveLow = false, 
         size_t ticksDebounce = 30, size_t ticksLongPress = 1500>
```

|Num | Parameter                               | Description                                                         |
| -  | --------------------------------------- | ------------------------------------------------------------------- |
| 1  | Pin                                     | Hardware-dependent input. Must implements field 'bool Pin::Get()'   |
| 2  | isActiveLow                             | if true, button goes pressed, when Pin input-state is LOW           |
| 3  | ticksDebounce                           | number of ticks to filter input state                               |
| 4  | ticksLongPress                          | number of ticks to activate the Long Press Event                    |

## Usage

```c++
...
  using btn = Button<pinBtn, true, 50, 3000>;
        ... 
  while (1){
    btn::Handler();
    if (btn::IsPressed()) 
        // do something;
  }
...
```

```c++
...
  void Start(){...};
  void Stop(){...};
        ...
  Button<pinStart, false, 100, 10000> btnEvent;
        ...    
  btnEvent.CallbackPressed = Start;
  btnEvent.CallbackReleased = Stop;
  while (1){
    btnEvent.Handler();  
  }
...
```

```c++
...
  void LongPress(){...};
        ...
  template<class Pin, bool isActiveLow = true, 
           size_t countDebounce = 10, size_t countLongPress = 2000>
  using btnTemplate = Button<Pin, activeState, countDebounce, countLongPress>;
        ...
  btnTemplate<pinBtn1> btn1;
  btnTemplate<pinBtn2> btn2;
        ... 
  btn2.CallbackPressedLong = LongPress; 
  while (1){
    btn1.Handler();
    btn2.Handler();
    if (btn1)
        // do something;
  }
...
```
