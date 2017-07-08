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

	//!
	Json Serialize(void) override
	{
		Json _dst = LogicComponent::Serialize();
		_dst["MaxLifeTime"] = maxLifeTime;
		_dst["LifeTime"] = lifeTime;

		return _dst;
	}
	//!
	void Deserialize(const Json& _src, class ObjectSolver* _context = nullptr) override
	{
		LogicComponent::Deserialize(_src, _context);
		maxLifeTime = _src["MaxLifeTime"];
		lifeTime = _src["LifeTime"];
	}
};

//----------------------------------------------------------------------------//
// 
//----------------------------------------------------------------------------//

class Rotate : public LogicComponent
{
public:
	RTTI("Rotate");

	float speed = 1;

	void Update(void)
	{
		GetEntity()->Rotate(gTime->Delta() * speed * PI);
	}

	//!
	Json Serialize(void) override
	{
		Json _dst = LogicComponent::Serialize();
		_dst["Speed"] = speed;

		return _dst;
	}
	//!
	void Deserialize(const Json& _src, class ObjectSolver* _context = nullptr) override
	{
		LogicComponent::Deserialize(_src, _context);
		speed = _src["Speed"];
	}
};

//----------------------------------------------------------------------------//
// PlayerController
//----------------------------------------------------------------------------//

class PlayerController
{
public:

protected:
};

//----------------------------------------------------------------------------//
// Game
//----------------------------------------------------------------------------//

InputSettings gDefaultInputSettings;


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
		_LoadInputSettings();

		Object::Register<LifeTime>();
		Object::Register<Rotate>();


		m_currentScene = new Scene;

		{
			Json _test;
			_test.Load(gResources->MakePath("Sprites/Test", "json").c_str());
			_test.Save(gResources->MakePath("Sprites/Test2", "json").c_str());
		}

		{

			EntityPtr _test = new Entity;
			_test->SetScene(m_currentScene);
			//_test->Scale(1.5f);

			SpriteRenderer* _sp = _test->AddComponent<SpriteRenderer>();
			_sp->SetSprite("test");
			_sp->Enable(true);


			Entity* _child = _test->AddChild();
			_child->Scale(2.5f);
			_child->Translate({ 250, 250 });
			Rotate* _rot = _child->AddComponent<Rotate>();
			_rot->speed = 1;


			SpriteRenderer* _sp2 = _child->AddComponent<SpriteRenderer>();
			_sp2->SetSprite("Sprites/Test");
			_sp2->Play("Run", AnimMode::Default);
			_sp2->SetAnimationSpeed(.25f);

			LifeTime* _lf = _child->AddComponent<LifeTime>();
			_lf->maxLifeTime = 5;
			_lf->Enable(false);

			_test->Serialize().Save(gResources->MakePath("entity", "json").c_str());
		}

		{
			Json _desc;
			_desc.Load(gResources->MakePath("entity", "json").c_str());
			_desc.Save(gResources->MakePath("entity3", "json").c_str());

			EntityPtr _test = new Entity;
			_test->Deserialize(_desc);

			_test->SetScene(m_currentScene);
			_test->SetPosition({ 500, 500 });

			_test->Serialize().Save(gResources->MakePath("entity2", "json").c_str());
		}
	}
	//!
	void _Shutdown(void)
	{ 
		LOG("Shutdown...");

		gInput->Settings().Save("Input");
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


	//!
	void _LoadInputSettings(void)
	{
		if (!gDefaultInputSettings.Load("InputDefaults") || gDefaultInputSettings.actions.empty())
		{
			// MoveX
			{
				InputAction _action;
				_action.negative = KeyCode::kKeyA;
				_action.negativeAlt = KeyCode::kKeyLeft;
				_action.positive = KeyCode::kKeyD;
				_action.positiveAlt = KeyCode::kKeyRight;

				gDefaultInputSettings.actions["MoveX"] = _action;
			}
			// MoveY
			{
				InputAction _action;
				_action.negative = KeyCode::kKeyS;
				_action.negativeAlt = KeyCode::kKeyDown;
				_action.positive = KeyCode::kKeyW;
				_action.positiveAlt = KeyCode::kKeyUp;

				gDefaultInputSettings.actions["MoveY"] = _action;
			}

			//! Jump
			{
				InputAction _action;
				_action.positive = KeyCode::kKeySpace;

				gDefaultInputSettings.actions["Jump"] = _action;
			}

			//! Fire1
			{

				InputAction _action;
				_action.positive = KeyCode::kKeyMouseLeft;

				gDefaultInputSettings.actions["Fire1"] = _action;
			}
			//! Fire2
			{

				InputAction _action;
				_action.positive = KeyCode::kKeyMouseRight;

				gDefaultInputSettings.actions["Fire2"] = _action;
			}
			//! MouseX
			{

				InputAction _action;
				_action.type = InputAction::Type::Mouse;
				_action.axis = 0;

				gDefaultInputSettings.actions["MouseX"] = _action;
			}
			//! MouseY
			{

				InputAction _action;
				_action.type = InputAction::Type::Mouse;
				_action.axis = 1;

				gDefaultInputSettings.actions["MouseY"] = _action;
			}
			//! MouseWheel
			{

				InputAction _action;
				_action.type = InputAction::Type::Mouse;
				_action.axis = 2;

				gDefaultInputSettings.actions["MouseWheel"] = _action;
			}

			gDefaultInputSettings.Save("InputDefaults");
		}

		if (!gInput->Settings().Load("Input") || gInput->Settings().actions.empty())
		{
			gInput->Settings() = gDefaultInputSettings;
		}
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
