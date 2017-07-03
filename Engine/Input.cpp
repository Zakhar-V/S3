#include "Input.hpp"
#include "Json.hpp"
#include "Resource.hpp"

//----------------------------------------------------------------------------//
// KeyCode
//----------------------------------------------------------------------------//

const HashMap<String, KeyCode> gKeyCodeNames =
{
	{ "None", KeyCode::kKeyNone }, ///< Indicates absence of any key, like SQL null
	//{ "Unknown", kKeyUnknown },  ///< Indicates an unidentified key
	{ "Left", KeyCode::kKeyLeft },
	{ "Right", KeyCode::kKeyRight },
	{ "Up", KeyCode::kKeyUp },
	{ "Down", KeyCode::kKeyDown },
	{ "Backspace", KeyCode::kKeyBackspace },  ///< ASCII Backspace
	{ "Tab", KeyCode::kKeyTab }, ///< ASCII Horizontal Tab
	{ "Enter", KeyCode::kKeyEnter }, // ASCII Carriage 
	{ "Home", KeyCode::kKeyHome },
	{ "End", KeyCode::kKeyEnd },
	{ "Page Up", KeyCode::kKeyPageUp },
	{ "Page Down", KeyCode::kKeyPageDown },
	{ "Shift", KeyCode::kKeyShift },
	{ "Left Shift", KeyCode::kKeyLeftShift },
	{ "Right Shift", KeyCode::kKeyRightShift },
	{ "Ctrl", KeyCode::kKeyControl },
	{ "Left Ctrl", KeyCode::kKeyLeftControl },
	{ "Right Ctrl", KeyCode::kKeyRightControl },
	{ "Alt", KeyCode::kKeyAlt },
	{ "Left Alt", KeyCode::kKeyLeftAlt },
	{ "Right Alt", KeyCode::kKeyRightAlt },
	{ "Escape", KeyCode::kKeyEscape }, // ASCII Escape
	{ "Space", KeyCode::kKeySpace }, // ' ' ASCII Space
	{ "'", KeyCode::kKeyApostrophe }, // '/'' ASCII Apostrophe
	{ ",", KeyCode::kKeyComma }, // ','
	{ "-", KeyCode::kKeyMinus }, // '-'
	{ ".", KeyCode::kKeyPeriod }, // '.'
	{ "/", KeyCode::kKeySlash }, // '//' ASCII Slash
	{ "0", KeyCode::kKey0 }, // '0'
	{ "1", KeyCode::kKey1 }, // '1'
	{ "2", KeyCode::kKey2 }, // '2'
	{ "3", KeyCode::kKey3 }, // '3'
	{ "4", KeyCode::kKey4 }, // '4'
	{ "5", KeyCode::kKey5 }, // '5'
	{ "6", KeyCode::kKey6 }, // '6'
	{ "7", KeyCode::kKey7 }, // '7'
	{ "8", KeyCode::kKey8 }, // '8'
	{ "9", KeyCode::kKey9 }, // '9'
	{ ";", KeyCode::kKeySemicolon }, // ';'
	{ "Pause", KeyCode::kKeyPause },
	{ "=", KeyCode::kKeyEquals }, // '='
	{ "Num Lock", KeyCode::kKeyNumLock },
	{ "Scroll Lock", KeyCode::kKeyScrollLock },
	{ "Caps Lock" , KeyCode::kKeyCapsLock },
	{ "A", KeyCode::kKeyA },// 'A'
	{ "B", KeyCode::kKeyB }, // 'B'
	{ "C", KeyCode::kKeyC }, // 'C'
	{ "D", KeyCode::kKeyD }, // 'D'
	{ "E", KeyCode::kKeyE }, // 'E'
	{ "F", KeyCode::kKeyF }, // 'F'
	{ "G", KeyCode::kKeyG }, // 'G'
	{ "H", KeyCode::kKeyH }, // 'H'
	{ "I", KeyCode::kKeyI }, // 'I'
	{ "J", KeyCode::kKeyJ }, // 'J'
	{ "K", KeyCode::kKeyK }, // 'K'
	{ "L", KeyCode::kKeyL }, // 'L'
	{ "M", KeyCode::kKeyM }, // 'M'
	{ "N", KeyCode::kKeyN }, // 'N'
	{ "O", KeyCode::kKeyO }, // 'O'
	{ "P", KeyCode::kKeyP }, // 'P'
	{ "Q", KeyCode::kKeyQ }, // 'Q'
	{ "R", KeyCode::kKeyR }, // 'R'
	{ "S", KeyCode::kKeyS }, // 'S'
	{ "T", KeyCode::kKeyT },// 'T'
	{ "U", KeyCode::kKeyU }, // 'U'
	{ "V", KeyCode::kKeyV }, // 'V'
	{ "W", KeyCode::kKeyW }, // 'W'
	{ "X", KeyCode::kKeyX }, // 'X'
	{ "Y", KeyCode::kKeyY }, // 'Y'
	{ "Z", KeyCode::kKeyZ }, // 'Z' 
	{ "[", KeyCode::kKeyLeftSquareBracket },  // '['	
	{ "\\", KeyCode::kKeyBackslash },  // '\'
	{ "]", KeyCode::kKeyRightSquareBracket },  // ']'
	{ "`", KeyCode::kKeyGraveAccent }, // '`'	
	{ "F1", KeyCode::kKeyF1 },
	{ "F2", KeyCode::kKeyF2 },
	{ "F3", KeyCode::kKeyF3 },
	{ "F4", KeyCode::kKeyF4 },
	{ "F5", KeyCode::kKeyF5 },
	{ "F6", KeyCode::kKeyF6 },
	{ "F7", KeyCode::kKeyF7 },
	{ "F8", KeyCode::kKeyF8 },
	{ "F9", KeyCode::kKeyF9 },
	{ "F10", KeyCode::kKeyF10 },
	{ "F11", KeyCode::kKeyF11 },
	{ "F12", KeyCode::kKeyF12 },
	{ "Numpad 0", KeyCode::kKeyNumpad0 },
	{ "Numpad 1", KeyCode::kKeyNumpad1 },
	{ "Numpad 2", KeyCode::kKeyNumpad2 },
	{ "Numpad 3", KeyCode::kKeyNumpad3 },
	{ "Numpad 4", KeyCode::kKeyNumpad4 },
	{ "Numpad 5" , KeyCode::kKeyNumpad5 },
	{ "Numpad 6" , KeyCode::kKeyNumpad6 },
	{ "Numpad 7", KeyCode::kKeyNumpad7 },
	{ "Numpad 8", KeyCode::kKeyNumpad8 },
	{ "Numpad 9", KeyCode::kKeyNumpad9 },
	{ "Numpad /", KeyCode::kKeyNumpadSlash },
	{ "Numpad *", KeyCode::kKeyNumpadAsterisk },
	{ "Numpad -", KeyCode::kKeyNumpadMinus },
	{ "Numpad +", KeyCode::kKeyNumpadPlus },
	{ "Numpad .", KeyCode::kKeyNumpadPeriod },
	// { "Numpad Enter", KeyCode::kKeyNumpadEnter }, // Missing in windows (?)
	{ "Print Screen", KeyCode::kKeyPrintScreen },
	{ "Insert", KeyCode::kKeyInsert },
	{ "Delete", KeyCode::kKeyDelete }, // ASCII Delete
	{ "Selection Sign", KeyCode::kKeySectionSign },  // ASCII Section sign	 "§"
	{ "Mouse Unknown", KeyCode::kKeyMouseUnknown },
	{ "Mouse Left", KeyCode::kKeyMouseLeft },
	{ "Mouse Right", KeyCode::kKeyMouseRight },
	{ "Mouse Wheel", KeyCode::kKeyMouseWheel },
};

//----------------------------------------------------------------------------//
String KeyCodeToString(KeyCode _code)
{
	for (const auto& i : gKeyCodeNames)
	{
		if (i.second == _code)
			return i.first;
	}
	return "Unknown";
}
//----------------------------------------------------------------------------//
KeyCode KeyCodeFromString(const String& _name)
{
	auto _it = gKeyCodeNames.find(_name);
	return _it != gKeyCodeNames.end() ? _it->second : KeyCode::kKeyUnknown;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// InputSettings
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool InputSettings::Load(const char* _filename)
{
	LOG("Load InputSettings \"%s\"", _filename);
	actions.clear();

	String _path = gResources->MakePath(_filename, "json");
	if (!gResources->FileExists(_path))
		return false;

	Json _desc;
	if (!_desc.Load(_path.c_str()))
		return false;

	for (auto i = _desc.Begin(); i != _desc.End(); ++i)
	{
		const Json& _src = i->second;
		InputAction _action;

		if (_src["Type"].AsString() == "Mouse")
			_action.type = InputAction::Type::Mouse;
		else
			_action.type = InputAction::Type::Button;

		_action.negative = KeyCodeFromString(_src["Negative"]);
		_action.negativeAlt = KeyCodeFromString(_src["NegativeAlt"]);
		_action.positive = KeyCodeFromString(_src["Positive"]);
		_action.positiveAlt = KeyCodeFromString(_src["PositiveAlt"]);


		_action.axis = _src["Axis"];
		_action.sensitivity = _src["Sensitivity"];
		_action.invert = _src["Invert"];

		actions[i->first] = _action;
	}
	return true;
}
//----------------------------------------------------------------------------//
void InputSettings::Save(const char* _filename)
{
	LOG("Save InputSettings \"%s\"", _filename);
	Json _desc;

	for (const auto& i : actions)
	{
		const InputAction& _action = i.second;
		Json& _dst = _desc.GetOrAdd(i.first);

		_dst["Type"] = _action.type == InputAction::Type::Mouse ? "Mouse" : "Button";

		_dst["Negative"] = KeyCodeToString(_action.negative);
		_dst["NegativeAlt"] = KeyCodeToString(_action.negativeAlt);
		_dst["Positive"] = KeyCodeToString(_action.positive);
		_dst["PositiveAlt"] = KeyCodeToString(_action.positiveAlt);

		_dst["Axis"] = _action.axis;
		_dst["Sensitivity"] = _action.sensitivity;
		_dst["Invert"] = _action.invert;
	}

	_desc.Save(gResources->MakePath(_filename, "json").c_str());
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Input
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool Input::IsPressed(KeyCode _code)
{
	return arctic::easy::IsKey(_code);
}
//----------------------------------------------------------------------------//
bool Input::IsPressed(const String& _actionName)
{
	return GetAxis(_actionName) != 0;
}
//----------------------------------------------------------------------------//
float Input::GetAxis(const String& _actionName)
{
	const auto& _it = m_settings.actions.find(_actionName);
	return _it != m_settings.actions.end() ? _it->second.value : 0;
}
//----------------------------------------------------------------------------//
bool Input::OnEvent(int _type, void* _arg)
{
	switch (_type)
	{
	case SystemEvent::Startup:
	{
		m_cursorPos.x = (float)arctic::easy::MousePos().x;
		m_cursorPos.y = (float)arctic::easy::MousePos().y;
		m_cursorDelta.x = (float)arctic::easy::MouseMove().x;
		m_cursorDelta.y = (float)arctic::easy::MouseMove().y;

	} break;

	case SystemEvent::BeginFrame:
		_Update();
		break;
	};
	return false;
}
//----------------------------------------------------------------------------//
void Input::_Update(void)
{
	m_cursorPos.x = (float)arctic::easy::MousePos().x;
	m_cursorPos.y = (float)arctic::easy::MousePos().y;
	m_cursorDelta.x = (float)arctic::easy::MouseMove().x;
	m_cursorDelta.y = (float)arctic::easy::MouseMove().y;

	for (auto& i : m_settings.actions)
	{
		InputAction& _action = i.second;

		float _value = 0;

		if (_action.type == InputAction::Type::Button)
		{
			if (IsPressed(_action.negative) || IsPressed(_action.negativeAlt))
				_value -= 1;
			if (IsPressed(_action.positive) || IsPressed(_action.positiveAlt))
				_value += 1;
		}
		else
		{
			switch (_action.axis)
			{
			case 0:
				_value = (float)arctic::easy::MousePos().x;
				break;
			case 1:
				_value = (float)arctic::easy::MousePos().y;
				break;
			case 2:
				_value = (float)arctic::easy::MouseWheelDelta();
				break;
			}
		}

		if (_action.invert)
			_value = -_value;
		_value *= _action.sensitivity;

		_action.prevValue = _action.value;
		_action.value = _value;
		if (_action.prevValue != _action.value)
		{
			// TODO: send event
			LOG("%s %f %f", i.first.c_str(), _action.prevValue, _action.value);
		}
	}
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
