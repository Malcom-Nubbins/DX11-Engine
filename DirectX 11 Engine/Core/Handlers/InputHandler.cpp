#include "InputHandler.h"

InputHandler::InputHandler()
{
}

InputHandler::~InputHandler()
{
}

POINT InputHandler::GetMousePos(HWND hwnd)
{
	POINT currentMousePos;

	if (!GetCursorPos(&currentMousePos))
	{
		throw std::exception("Can't get current mouse position.");
	}

	if (hwnd != NULL)
	{
		if (!ScreenToClient(hwnd, &currentMousePos))
		{
			throw std::exception("Can't project from screen to window.");
		}
	}

	return currentMousePos;
}

void InputHandler::SetMousePos(HWND hwnd, POINT pos)
{
	if (hwnd != nullptr)
	{
		if (!ClientToScreen(hwnd, &pos))
		{
			throw std::exception("Can't project from screen to window");
		}
	}

	if(!SetCursorPos(pos.x, pos.y))
	{
		throw std::exception("Can't set cursor pos");
	}
}

bool InputHandler::IsKeyDown(Keys key)
{
	BYTE keyboardState[256];

	// WinAPI call to get the keyboard state.
	GetKeyboardState(keyboardState);

	// Check whether the key has been pressed.
	return (keyboardState[key] & 0x80) > 0;

	return false;
}
