#pragma once

#include "Base.hpp"

//----------------------------------------------------------------------------//
// Application
//----------------------------------------------------------------------------//

class Application : public Singleton<Application>
{
public:

	Application(void) = default;
	~Application(void) = default;

	void MainLoop(void)
	{
		_Init();
		while (!m_requireExit)
		{
			_BeginFrame();
			_Endrame();
			ShowFrame();
		}
		_Destroy();
	}

protected:

	bool m_requireExit = false;	//!> do set to true for exit

	virtual void _Init(void) { }
	virtual void _Destroy(void) { }
	virtual void _BeginFrame(void) { }
	virtual void _Endrame(void) { }
};

//----------------------------------------------------------------------------//
// 
//----------------------------------------------------------------------------//

