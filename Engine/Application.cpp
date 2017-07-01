#include "Application.hpp"
#include "Scene.hpp"
#include "Render.hpp"
#include "Physics.hpp"
#include "Audio.hpp"
#include "Logic.hpp"

//----------------------------------------------------------------------------//
// Application
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
Application::Application(void)
{
	RenderWorld::Register();
	PhysicsWorld::Register();
	AudioWorld::Register();
	LogicSystem::Register();
}
//----------------------------------------------------------------------------//
Application::~Application(void)
{

}
//----------------------------------------------------------------------------//
void Application::MainLoop(void)
{
	_Init();
	while (!m_requireExit)
	{
		m_time.Update();

		_BeginFrame();
		_ProcessFrame();
		_EndFrame();

		arctic::easy::ShowFrame();
	}
	_Destroy();
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
