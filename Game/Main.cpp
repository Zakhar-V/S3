#include <Engine.hpp>

//----------------------------------------------------------------------------//
// 
//----------------------------------------------------------------------------//

#define gPlayerState PlayerState::Instance

struct PlayerState : public Singleton<PlayerState>
{
	float health = 100;
};

//----------------------------------------------------------------------------//
// 
//----------------------------------------------------------------------------//

class LifeTime : public LogicComponent
{
public:
	RTTI("LifeTime");

	float lifeTime = 0;
	float maxLifeTime = 0;

	void Update(void)
	{
		lifeTime += gTime->Delta();

		if (lifeTime >= maxLifeTime)
			Entity::Destroy(GetEntity());
	}
};

//----------------------------------------------------------------------------//
// Game
//----------------------------------------------------------------------------//

class Game : public Application
{
public:

	bool OnEvent(int _type, void* _arg) override
	{
		switch (_type)
		{
		case SystemEvent::Startup:
			_Startup();
			break;
		case SystemEvent::Shutdown:
			_Shutdown();
			break;
		case SystemEvent::BeginFrame:
			_BeginFrame();
			break;
		case SystemEvent::Update:
			_Update();
			break;
		case SystemEvent::Render:
			_Render();
			break;
		case SystemEvent::EndFrame:
			_EndFrame();
			break;
		}
		return false;
	}

protected:

	//!
	void _Startup(void)
	{  
		LOG("Startup...");

		gResources->SetDataPath("Data/");

		Object::Register<LifeTime>();


		m_currentScene = new Scene;

		EntityPtr _test = new Entity;
		_test->SetScene(m_currentScene);
		_test->Scale(5);

		SpriteRenderer* _sp = _test->AddComponent<SpriteRenderer>();
		_sp->SetSprite("Sprites/Test");
		_sp->Play("Run", AnimMode::PingPong);

 
		Entity* _child = _test->AddChild();
		_child->Scale(.5f);
		_child->Translate({ 50, 0 });

		SpriteRenderer* _sp2 = _child->AddComponent<SpriteRenderer>();
		_sp2->SetSprite("Sprites/Test");
		_sp2->Play("Idle");

		LifeTime* _lf = _child->AddComponent<LifeTime>();
		_lf->maxLifeTime = 5;
	}
	//!
	void _Shutdown(void)
	{ 
		LOG("Shutdown...");
	}
	//!
	void _BeginFrame(void)
	{ 
		m_requireExit = m_requireExit || arctic::easy::IsKey(arctic::kKeyEscape);
	}
	//!
	void _Update(void)
	{
		m_currentScene->Update();
	}
	//!
	void _Render(void)
	{
		m_currentScene->Render();
	}
	//!
	void _EndFrame(void)
	{ 
	}

	ScenePtr m_currentScene;
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
