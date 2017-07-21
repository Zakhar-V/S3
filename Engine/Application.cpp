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
	SendEvent(SystemEvent::Start);
	SendEvent(SystemEvent::Startup);
	while (!m_requireExit)
	{
		SendEvent(SystemEvent::BeginFrame);
		SendEvent(SystemEvent::Update);
		SendEvent(SystemEvent::PostUpdate);
		SendEvent(SystemEvent::Render);
		SendEvent(SystemEvent::PostRender);
		SendEvent(SystemEvent::DebugDraw);
		SendEvent(SystemEvent::EndFrame);

		arctic::easy::ShowFrame();
	}
	SendEvent(SystemEvent::Shutdown);
	SendEvent(SystemEvent::Stop);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
