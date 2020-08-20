//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Circular buffer 
//  TODO:         
//----------------------------------------------------------------------------------

#ifndef _CIRCULAR_BUFFER_HPP
#define _CIRCULAR_BUFFER_HPP

#include <cstddef>


/*!
  @file
  @brief Circular buffer
*/

/*!
  @brief Namespace for data containers.
*/
namespace container{

/*!
  @brief Class of Circular Buffer.
  @tparam <T> buffer's type
  @tparam <size> number of elements in buffer
*/ 
template<typename T, size_t size>
class CircularBuffer{
public:

	/*!
    @brief Check the emptyness of buffer
  */
  //__attribute__ ((always_inline)) inline 
  bool IsEmpty() const{return  !static_cast<bool>(count);}

	/*!
    @brief Pop the head element of buffer
  */
  const T& Pop(){
    auto currentHead = head;
    if (count){
      count--;
      head = head + 1 >= size ? 0 : head + 1;
    }
    return const_cast<const T&>(buffer[currentHead]);
  }

	/*!
    @brief Pop the elements to destination array
    @param [out] destination pointer to output array
    @param [in] length number of elements to pop
    @return number of poped elements
  */
  size_t Pop(T* destination, size_t length){
    if (length > count) length = count;
    auto currentHead = head;
    head = head + length >= size ? 0 : head + length;
    count -= length;
    for (size_t i = 0; i < length; ++i){
      if (currentHead >= size) {
        currentHead = 0;
      }
      destination[i] = buffer[currentHead++];
    }
    return length;
  }

	/*!
    @brief Read the head element of buffer
  */
  const T& Front() const { return (const T&)buffer[head]; }

	/*!
    @brief Fill the whole buffer with element
    @param [in] element to fill
  */
  void Fill(const T& element){
    for (size_t i = 0; i < size; i++)
      buffer[i] = element;
    head = tail = 0;
    count = size;
  }

	/*!
    @brief Push element to buffer
    @param [in] element to push
    @return if false, then buffer was overflowed
  */
  bool Push(const T &element){
    buffer[tail] = element;
    tail = tail + 1 >= size ? 0 : tail + 1;
    if (count < size){
      count++;
      return true;
    } 
    head = tail;
    return false;
  }

	/*!
    @brief Push array of elements to buffer
    @param [in] pointer to array of elements
    @param [in] number of elements to push
    @return if false, then buffer was overflowed
  */
  bool Push(const T* elements, size_t length){
    size_t countToOverflow = GetCountToOverflow();
    size_t index = length > size ? length - size : 0;
    count += length;
    if (count > size) count = size;
    for (; index < length; ++index) {
      auto currentTail = tail;
      if (tail >= size) {
        currentTail = tail = 0;
      } else {tail = tail + 1 >= size ? 0 : tail + 1;}
      buffer[currentTail] = elements[index];
    }
    if (length >= countToOverflow) {
      head = tail;
      return false;
    }
    return true;
  }

	/*!
    @brief Read element of buffer
    @param [in] index of element in buffer
  */
  const T& operator[](size_t index){
    index = (index + head) % size;
    return const_cast<const T&>(buffer[index]);
  }

	/*!
    @brief Get size of buffer
  */
  //__attribute__ ((always_inline)) inline 
  size_t GetSize() const { return size; }

	/*!
    @brief Get current number of elements in buffer
  */
  //__attribute__ ((always_inline)) inline 
  size_t GetCount()const { return count; }

	/*!
    @brief Clear the buffer
  */
  //__attribute__ ((always_inline)) inline 
  void Clear() { head = tail = count = 0; }

  /*!
    @brief Get the number of elements till tail or end of the buffer
  */
  //__attribute__ ((always_inline)) inline 
  size_t GetCountToBufferLastIndex() const { 
    
    return head >= tail ?  size - head : tail - head; }

  /*!
    @brief Get the number of elements till buffer will be overflowed
  */
  //__attribute__ ((always_inline)) inline 
  size_t GetCountToOverflow() const { return size - count; }

  /*!
    @brief Set buffer's head to last index(tail or 0)
  */
  void SetHeadToLastIndex(){
    if (head > tail) {
      count = count + head - size;
      head = 0;
    }
    else {
      head = tail;
      count = 0;
    }
  }

  /*!
    @brief Add value to tail index
  */
  void AddToTail(size_t valueToAdd){
    tail = (tail + valueToAdd) % size;
    count = count + valueToAdd >= size ? size : count + valueToAdd;
    if (!GetCountToOverflow()) head = tail;
  }

  /*!
    @brief Get address of buffer's head
  */
  const auto GetHeadAddress(){return &buffer[head];}

private:
  volatile T buffer[size] {};
  size_t head = 0;
  size_t tail = 0;
  size_t count = 0;
};

} //! namspace container

#endif //!_CIRCULAR_BUFFER_HPP