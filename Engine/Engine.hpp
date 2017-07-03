#pragma once

#include "Base.hpp"
#include "Json.hpp"
#include "Math.hpp"
#include "Object.hpp"

#include "Scene.hpp"
#include "Render.hpp"
#include "Physics.hpp"
#include "Audio.hpp"
#include "Logic.hpp"

#include "System.hpp"
#include "Time.hpp"
#include "Resource.hpp"
#include "Input.hpp"

#include "Application.hpp"


//Example:
/*
#include <Engine.hpp>

// 
class Game : public Application
{
public:

protected:
	void _Init(void) override
	{
	}
	void _Destroy(void) override
	{
	}
	void _BeginFrame(void) override
	{
		m_requireExit = m_requireExit || IsKey(kKeyEscape);
	}
	void _Endrame(void) override
	{
	}
};

void EasyMain()
{
	Game _game;
	_game.MainLoop();
}

*/
