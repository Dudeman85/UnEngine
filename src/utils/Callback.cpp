#include "utils/Callback.h"

#include <iostream>

template<typename Function, typename... Args>
CallbackWrapper::CallbackWrapper(Function&& CallbackFunction, Args&&... Arguments)
    : Callback(std::bind(std::forward<Function>(CallbackFunction), std::forward<Args>(Arguments)...))
{

}

CallbackWrapper::~CallbackWrapper()
{

}

void CallbackWrapper::Call()     // Call the callback function, does not destroy the class
{
    Callback();
}

void callbackFunction(int x, int y) {
    std::cout << "Callback called with arguments: " << x << " and " << y << std::endl;
}