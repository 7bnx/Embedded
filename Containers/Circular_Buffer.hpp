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
#include "../Controllers/Common/Compiler/Compiler.h"

/*!
  @file
  @brief Circular buffer
*/

/*!
  @brief Namespace for data containers
*/
namespace container{

/*!
  @brief Class of Circular Buffer
  @tparam <T> buffer's type
  @tparam <size> number of elements in buffer
*/ 
template<typename T, size_t size>
class CircularBuffer{
public:

  /*!
    @brief Check the emptyness of buffer
  */
  __FORCE_INLINE bool IsEmpty() const{ return  !static_cast<bool>(count); }

  /*!
    @brief Pop the head element of buffer
  */
  const T& Pop(){
    auto currentHead = head;
    if (count){
      count--;
      head = _IncrementValue(head);
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
    auto currentHead = head;
    if (length > count) length = count;
    count -= length;
    head = _AddValue(head, length);
    for (size_t i = 0; i < length; ++i){
      if (currentHead >= size) currentHead = 0;
      destination[i] = buffer[currentHead++];
    }
    return length;
  }

  /*!
    @brief Read the head element of buffer
  */
  __FORCE_INLINE const T& Front() const { return (const T&)buffer[head]; }

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
    @brief Fill part of buffer with element
    @param [in] element to fill
  */
  void Fill(const T& element, size_t number){
    number = number > size ? size : number;
    for (size_t i = 0; i < number; i++){
      buffer[tail] = element;
      tail = _IncrementValue(tail);
    }
    if ((count + number) > size){
      head = tail;
      count = size;
    } else count += number;
  }

  /*!
    @brief Push element to buffer
    @param [in] element to push
    @return if false, then buffer was overflowed
  */
  bool Push(const T &element){
    buffer[tail] = element;
    tail = _IncrementValue(tail);
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
    size_t index = length > size ? length - size : 0;

    for (; index < length; ++index) {
      buffer[tail] = elements[index];
      tail = _IncrementValue(tail);
    }
    
    count += length;
    if (count > size) {
      count = size;
      head = tail;
      return true;
    } 
    return false;
  }

  /*!
    @brief Read element of buffer
    @param [in] index of element in buffer
  */
  __FORCE_INLINE const T& operator[](size_t index){
    index = _AddValue(head + index);
    return const_cast<const T&>(buffer[index]);
  }

  /*!
    @brief Get size of buffer
  */
  __FORCE_INLINE size_t GetSize() const { return size; }

  /*!
    @brief Get current number of elements in buffer
  */
  __FORCE_INLINE size_t GetCount()const { return count; }

  /*!
    @brief Flush buffer
  */
  __FORCE_INLINE void Flush(){ head = tail = count = 0; }

  /*!
    @brief Get the number of elements till tail or end of the buffer
  */
  __FORCE_INLINE size_t GetCountToBufferLastIndex() const { return head >= tail ?  size - head : tail - head; }

  /*!
    @brief Get the difference between size of buffer and current tail index
  */
  __FORCE_INLINE size_t GetCountToBuffersEnd() const { return size - tail; }

  /*!
    @brief Get the number of elements till buffer will be overflowed
  */
  __FORCE_INLINE size_t GetCountToOverflow() const { return size - count; }

  /*!
    @brief Set buffer's head to last index(tail or 0)
  */
  void SetHeadToLastIndex(){
    if (head >= tail) {
      count += head - size;
      head = 0;
    }
    else {
      head = tail;
      count = 0;
    }
  }

  /*!
    @brief Set buffer's head to tail
  */
  void SetHeadToTailIndex(){ head = tail; }

  /*!
    @brief Add value to tail index
  */
  void AddToTail(size_t valueToAdd){
    tail = _AddValue(tail, valueToAdd);
    count = (count + valueToAdd) > size ? size : (count + valueToAdd);
    if (!GetCountToOverflow()) head = tail;
  }

  /*!
    @brief Add value to head index
  */
  void AddToHead(size_t valueToAdd){
    head = _AddValue(head, valueToAdd);
    count = (valueToAdd >= count) ? 0 : (count - valueToAdd);
  }

  /*!
    @brief Get address of buffer's head
  */
  __FORCE_INLINE const auto GetHeadAddress(){ return &buffer[head]; }

  /*!
    @brief Get address of buffer's tail
  */
  __FORCE_INLINE const auto GetTailAddress(){ return &buffer[tail]; }

private:

  size_t head = 0;
  size_t tail = 0;
  size_t count = 0;
  volatile T buffer[size] {};
  
  __FORCE_INLINE size_t _IncrementValue(size_t value){
    if constexpr ((size & (size - 1)) == 0) 
      return (++value) & (size - 1);
    else 
      return ((value + 1) >= size) ? 0 : (value + 1);
  }

  __FORCE_INLINE size_t _AddValue(size_t value, size_t add){
    if constexpr ((size & (size - 1)) == 0) 
      return (value + add) & (size - 1);
    else 
      return (value + add) % size;
  }

};

} //! namspace container

#endif //!_CIRCULAR_BUFFER_HPP