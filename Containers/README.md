# Containers
Classes to store data

[Circular buffer](#Circular-buffer)

## Circular buffer
A fixed-sized buffer, that connected end-to-end. Stream usage, e.g.: UART

### Template

```c++
template<typename T, size_t size>
```

|Num | Parameter    | Description                     |
| -  | ------------ | ------------------------------- |
| 1  | T            | Type of elements in buffer      |
| 2  | size         | Number of elements in buffer    |


### Interface

|Num | Method                                            | Description                                               |
| -  | ------------------------------------------------- | --------------------------------------------------------- |
| 1  | bool IsEmpty()                                    | Return true, if buffer is empty(head == tail)             |
| 2  | const T& Pop()                                    | Pop the head-element of buffer                            |
| 3  | size_t Pop(T* destination, size_t length)         | Pop the number of elements to destination array           |
| 4  | const T& Front()                                  | Read the head-element of buffer                           |
| 5  | void Fill(const T& element)                       | Fill the whole buffer with element                        |
| 6  | bool Push(const T &element)                       | Push element to buffer. Return false, if overflowed       |
| 7  | bool Push(const T *const elements, size_t length) | Push array of elements. Return false, if overflowed       |
| 8  | const T& operator[]                               | Read the element of buffer by index                       |
| 9  | size_t GetSize()                                  | Get size of buffer                                        |
| 10 | size_t GetCount()                                 | Get current number of elements in buffer                  |
| 11 | void Clear()                                      | Clear the buffer (set head = tail = 0)                    |
| 12 | size_t GetCountToBufferLastIndex()                | Get the number of elements till tail or end of the buffer |
| 13 | size_t GetCountToOverflow()                       | Get the number of elements till buffer will be overflowed |
| 14 | void SetHeadToLastIndex()                         | Set buffer's head to last index(tail or 0)                |
| 15 | void AddToTail(size_t valueToAdd)                 | Add value to tail index)                                  |
| 16 | const T* const GetHeadAddress()                   | Get address of buffer's head                              |

### Usage

```cpp
...
static const uint8_t array[] = {0,1,2,3,4,5,6,7,8,9};
container::CircularBuffer<uint8_t, 5> buffer;
...
buffer.Fill(5); // buffer : {5, 5, 5, 5, 5}
buffer.Push(array[0]); // buffer : {0, 5, 5, 5, 5}
buffer.Push(&array[1], 3); // buffer : {0, 1, 2, 3, 5}
auto e = buffer.Front(); // e : 5
e = buffer.Pop(); // e : 5
e = buffer.Pop(); // e : 0
...
```