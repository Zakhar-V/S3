#pragma once

#include "System.hpp"
#include "Math.hpp"
//----------------------------------------------------------------------------//
// KeyCode
//----------------------------------------------------------------------------//

using arctic::KeyCode;

//!
String KeyCodeToString(KeyCode _code);
//!
KeyCode KeyCodeFromString(const String& _name);

//----------------------------------------------------------------------------//
// InputEvent
//----------------------------------------------------------------------------//

struct InputEvent
{
	enum Type
	{
		KeyUp = StringUtils::ConstHash("InputEvent::KeyUp"),
		KeyDown = StringUtils::ConstHash("InputEvent::KeyUp"),
		KeyRepeat = StringUtils::ConstHash("InputEvent::KeyRepeat"),
		MouseMove = StringUtils::ConstHash("InputEvent::MouseMove"),
	};

	// ...
};

//----------------------------------------------------------------------------//
// InputAction
//----------------------------------------------------------------------------//

struct InputAction
{
	enum class Type
	{
		Button,
		Mouse,
	};

	Type type = Type::Button;
	KeyCode negative = KeyCode::kKeyNone;
	KeyCode positive = KeyCode::kKeyNone;
	KeyCode negativeAlt = KeyCode::kKeyNone;
	KeyCode positiveAlt = KeyCode::kKeyNone;
	int axis = 0; // x, y, z
	float sensitivity = 1;
	bool invert = false;
	
	float value = 0;
	float prevValue = 0;
};

//----------------------------------------------------------------------------//
// InputSettings
//----------------------------------------------------------------------------//

struct InputSettings
{
	HashMap<String, InputAction> actions;

	bool Load(const char* _filename);
	void Save(const char* _filename);
};

//----------------------------------------------------------------------------//
// Input
//----------------------------------------------------------------------------//

#define gInput Input::Instance

class Input : public Module<Input>
{
public:
	//!
	InputSettings& Settings(void) { return m_settings; }

	//!
	bool IsPressed(KeyCode _code);
	//!
	bool IsPressed(const String& _actionName);
	//!
	float GetAxis(const String& _actionName);

	// TODO: GetAnyKey()

	//!
	const Vector2& CursorPos(void) { return m_cursorPos; }
	//!
	const Vector2& CursorDelta(void) { return m_cursorDelta; }

	//!
	bool OnEvent(int _type, void* _arg) override;

protected:
	//!
	void _Update(void);

	Vector2 m_cursorPos = { 0, 0 };
	Vector2 m_cursorDelta = { 0, 0 };
	//!
	InputSettings m_settings;
};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
