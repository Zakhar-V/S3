#pragma once

#include "Base.hpp"
#include "Time.hpp"

//----------------------------------------------------------------------------//
// Application
//----------------------------------------------------------------------------//

class Application : public Singleton<Application>
{
public:

	Application(void);
	~Application(void);

	void MainLoop(void);

protected:

	bool m_requireExit = false;	//!> do set to true for exit

	virtual void _Init(void) { }
	virtual void _Destroy(void) { }
	virtual void _BeginFrame(void) { }
	virtual void _ProcessFrame(void) { }
	virtual void _EndFrame(void) { }

	Time m_time;
};

//----------------------------------------------------------------------------//
// 
//----------------------------------------------------------------------------//

