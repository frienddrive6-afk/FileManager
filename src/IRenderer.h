#pragma once

#include <string>

#include "NavigationState.h"

using namespace std;

/// @brief сласс от которого наследуются классы рендеринга TUI и GUI
class IRenderer
{
public:

    
    virtual ~IRenderer() = default;

    /// @brief рисует интерфейс
    virtual void Draw(const NavigationState& state) = 0;

    /// @brief показывает сообщение
    virtual void ShowMessage(const string& message) = 0;

    /// @brief возвращает введенную пользователем строку
    virtual string ReadInput() = 0;

};
