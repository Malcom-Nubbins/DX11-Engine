#pragma once
#include "KeyCodes.h"

class EventArgs
{
public:
	typedef EventArgs TBase;

	EventArgs() {}
};

class KeyEvent : public EventArgs
{
public:
	enum class KeyState
	{
		Released = 0,
		Pressed = 1
	};

	KeyEvent(KeyCode::Key key, unsigned int character, KeyState state, bool ctrl, bool shift, bool alt)
		: Key(key)
		, Char(character)
		, State(state)
		, Ctrl(ctrl)
		, Shift(shift)
		, Alt(alt)
	{}

	KeyCode::Key Key;
	unsigned int Char;
	KeyState State;
	bool Ctrl;
	bool Shift;
	bool Alt;
};

class MouseMotionEvent : public EventArgs
{
public:

	MouseMotionEvent(bool leftMouseButton, bool middleMouseButton, bool rightMouseButton, bool ctrl, bool shift, int x, int y)
		: LeftMouseButton(leftMouseButton)
		, MiddleMouseButton(middleMouseButton)
		, RightMouseButton(rightMouseButton)
		, Ctrl(ctrl)
		, Shift(shift)
		, X(x)
		, Y(y)
		, RelX(0)
		, RelY(0)
	{}

	bool LeftMouseButton;
	bool MiddleMouseButton;
	bool RightMouseButton;
	bool Ctrl;
	bool Shift;

	int X;
	int Y;
	int RelX;
	int RelY;
};

class MouseButtonEvent : public EventArgs
{
public:
	enum class MouseButton
	{
		None = 0,
		Left = 1,
		Right = 2,
		Middle = 3
	};

	enum class ButtonState
	{
		Released = 0,
		Pressed = 1
	};

	MouseButtonEvent(MouseButton buttonID, ButtonState state, bool leftMouseButton, bool middleMouseButton, bool rightMouseButton, bool ctrl, bool shift, int x, int y)
		: Button(buttonID)
		, State(state)
		, LeftMouseButton(leftMouseButton)
		, MiddleMouseButton(middleMouseButton)
		, RightMouseButton(rightMouseButton)
		, Ctrl(ctrl)
		, Shift(shift)
		, X(x)
		, Y(y)
	{}

	MouseButton Button;
	ButtonState State;
	bool LeftMouseButton;
	bool MiddleMouseButton;
	bool RightMouseButton;
	bool Ctrl;
	bool Shift;

	int X;
	int Y;
};

class MouseWheelEvent : EventArgs
{
public:

	MouseWheelEvent(float wheelDelta, bool leftMouseButton, bool middleMouseButton, bool rightMouseButton, bool ctrl, bool shift, int x, int y)
		: WheelDelta(wheelDelta)
		, LeftMouseButton(leftMouseButton)
		, MiddleMouseButton(middleMouseButton)
		, RightMouseButton(rightMouseButton)
		, Ctrl(ctrl)
		, Shift(shift)
		, X(x)
		, Y(y)
	{}

	float WheelDelta;
	bool LeftMouseButton;
	bool MiddleMouseButton;
	bool RightMouseButton;
	bool Ctrl;
	bool Shift;

	int X;
	int Y;
};

class UpdateEvent : public EventArgs
{
public:

	UpdateEvent(double deltaTime, double totalTime)
		: ElapsedTime(deltaTime)
		, TotalTime(totalTime)
	{}

	double ElapsedTime;
	double TotalTime;
};

class RenderEvent : public EventArgs
{
public:

	RenderEvent(double deltaTime, double totalTime)
		: ElapsedTime(deltaTime)
		, TotalTime(totalTime)
	{}

	double ElapsedTime;
	double TotalTime;
};