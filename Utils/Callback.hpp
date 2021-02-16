//----------------------------------------------------------------------------------
//  Author:       Semyon Ivanov
//  e-mail:       agreement90@mail.ru
//  github:       https://github.com/7bnx/Embedded
//  Description:  Lambda callback
//  TODO:
//----------------------------------------------------------------------------------

#ifndef _CALLBACK_H
#define _CALLBACK_H

/*!
  @file
  @brief Callback with 1 parameters in capture-list.
*/

/*!
  @brief Namespace for utils
*/
namespace utils{

/*!
  @brief Uses for events and triggers
*/
class Callback {

public:
  /*!
    @brief Constructor. Sets callback to epmty function.
  */
  Callback() {function = [](void*) {};}

  /*!
    @brief Assign function to Callback
    @tparam <T> new function
  */
  template<class T>
  void operator=(T func) {
    static_assert((sizeof(parameters) >= sizeof(T)),  
    "Too much variables in capture list");
    // Save the pointer to function, that call func
    function = [](void* param) {
      (*(T*)param)();
    };
      // Copy func value to parameters
      for (unsigned i = 0; i < sizeof(T); i++) {
        *(((char*)(parameters))+i) = *(((char*)(&func))+i);
      }
  }

  /*!
    @brief Call callback
  */
  void operator()() {
    // Call by function pointer with parameters
    if (function != nullptr)
      function(parameters);
  }

  /*!
    @brief Reset Callback
  */
  __attribute__ ((always_inline)) inline
  void Reset() { function = [](void *){}; }

private:
  void (*function)(void*);
  void* parameters[1] { nullptr };

};

} //!namespace utils

#endif // !_CALLBACK_H