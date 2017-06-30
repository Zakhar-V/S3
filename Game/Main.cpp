#include <Engine.hpp>

//----------------------------------------------------------------------------//
// Game
//----------------------------------------------------------------------------//

class Game : public Application
{
public:


protected:

	void _Init(void) override
	{  
		LOG("Initialize...");
	}
	void _Destroy(void) override
	{ 
		LOG("Destroy...");
	}
	void _BeginFrame(void) override
	{ 
		m_requireExit = m_requireExit || IsKey(kKeyEscape);
	}
	void _Endrame(void) override 
	{ 
	}
};

//----------------------------------------------------------------------------//
// MAIN
//----------------------------------------------------------------------------//

void EasyMain() {
	
	Game _game;
	_game.MainLoop();
}

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
