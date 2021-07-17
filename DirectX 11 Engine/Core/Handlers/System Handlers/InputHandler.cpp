#include "InputHandler.h"

InputHandler::KeyboardState InputHandler::m_CurrFrameState[256];
InputHandler::KeyboardState InputHandler::m_LastFrameState[256];

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

bool InputHandler::IsKeyUp(Keys key)
{
	bool wasKeyDownPreviousFrame = (m_LastFrameState[key] & 0x80) > 0;
	bool isKeyDownThisFrame = (m_CurrFrameState[key] & 0x80) > 0;

	bool wasKeyReleased = wasKeyDownPreviousFrame && !isKeyDownThisFrame;

	return wasKeyReleased;
}

bool InputHandler::IsKeyDown(Keys key)
{
	return (m_CurrFrameState[key] & 0x80) > 0;
}

void InputHandler::UpdateInputState()
{
	memcpy(m_LastFrameState, m_CurrFrameState, 256);

	GetKeyboardState(m_CurrFrameState);
}
