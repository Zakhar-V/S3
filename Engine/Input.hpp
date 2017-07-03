#pragma once

#include "System.hpp"

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
// Input
//----------------------------------------------------------------------------//

#define gInput Input::Instance

class Input : public Module<Input>
{
public:

	//TODO

protected:

};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
