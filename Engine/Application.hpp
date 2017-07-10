#pragma once

#include "System.hpp"
#include "Time.hpp"
#include "Resource.hpp"
#include "Render.hpp"
#include "Input.hpp"

//----------------------------------------------------------------------------//
// Modules
//----------------------------------------------------------------------------//

//!
class Modules : public NonCopyable
{
public:

protected:

	Time m_time;
	ResourceCache m_resources;
	Input m_input;
	Renderer m_renderer;
	SceneManager m_sceneManager;
};

//----------------------------------------------------------------------------//
// Application
//----------------------------------------------------------------------------//

//!
class Application : public Modules, public Module<Application>
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

