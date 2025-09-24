#pragma once

#include <functional>

class CallbackWrapper
{
public:
    template<typename Function, typename... Args>
    CallbackWrapper(Function&& CallbackFunction, Args&&... Arguments);
    ~CallbackWrapper();

    virtual void Call();

protected:
    std::function<void()> Callback;
};

void callbackFunction(int x, int y);