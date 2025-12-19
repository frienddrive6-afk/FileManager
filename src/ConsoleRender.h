#pragma once

#include "IRenderer.h"
#include <iostream>
#include <string>

using namespace std;


/// @brief класс рендеринга в консоль
class ConsoleRender : public IRenderer
{
public:
    
    /// @brief рисует интерфейс
    /// @param state переменная состояния
    virtual void Draw(const NavigationState& state) override;           //override говорит что мытод будет переписан в этом классе

	/// @brief показывает сообщение
	/// @param message текст сообщения
    virtual void ShowMessage(const string& message) override;           //override говорит что мытод будет переписан в этом классе

	/// @return введенная пользователем строка
    virtual string ReadInput() override;                                //override говорит что мытод будет переписан в этом классе
};