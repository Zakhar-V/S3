#pragma once

#include "System.hpp"
#include "Time.hpp"
#include "Resource.hpp"
#include "Input.hpp"

//----------------------------------------------------------------------------//
// Modules
//----------------------------------------------------------------------------//

//!
class Modules : public System
{
public:

protected:

	Time m_time;
	ResourceCache m_resources;
	Input m_input;
};

//----------------------------------------------------------------------------//
// Application
//----------------------------------------------------------------------------//

//!
class Application : public Modules, Singleton<Application>
{
public:
	//!
	Application(void);
	//!
	~Application(void);

	//!
	void MainLoop(void);

protected:

	bool m_requireExit = false;	//!> do set to true for exit
};

//----------------------------------------------------------------------------//
// 
//----------------------------------------------------------------------------//

