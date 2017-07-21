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
// Player
//----------------------------------------------------------------------------//

class Player : public LogicComponent
{
public:
	RTTI("Player");

	void Clone(Component* _src) override
	{
		LogicComponent::Clone(_src);
		if (_src->IsTypeOf<Player>())
		{
			Player* _player = static_cast<Player*>(_src);
		    // ...
		}
	}

	//!
	Json Serialize(void) override
	{
		Json _dst = LogicComponent::Serialize();
		
		// ...

		return _dst;
	}
	//!
	void Deserialize(const Json& _src, class ObjectSolver* _context = nullptr) override
	{
		LogicComponent::Deserialize(_src, _context);

		// ...
	}

	void Update(void) override
	{
		float _horz = gInput->GetAxis("MoveX");
		float _vert = gInput->GetAxis("MoveY");
		float _len = _horz * _horz + _vert * _vert;
		if (_len > 0)
		{
			_len = 1 / sqrtf(_len);
			_vert *= _len;
			_horz *= _len;
		}
		_vert *= m_speed * gTime->Delta();
		_horz *= m_speed * gTime->Delta();

		GetEntity()->Translate({ _horz, _vert });

		//gInput->CursorPos();

		Vector2 _center = GetEntity()->GetTransform().Pos();
		Vector2 _target = GetScene()->GetSystem<RenderWorld>()->ScreenToWorld(gInput->CursorPos());

		float _angle = atan2(_target.x - _center.x, _target.y - _center.y);
		GetEntity()->SetRotation(_angle);
		
		GetScene()->GetSystem<RenderWorld>()->GetCamera()->GetEntity()->SetPosition({ _center.x - gRenderer->ScreenWidth() / 2, _center.y - gRenderer->ScreenHeight() / 2 }); // WTF?

		//gRenderer->DrawCross(m_currentScene->GetSystem<RenderWorld>()->ScreenToWorld(gInput->CursorPos()), 20, {0xff, 0, 0, 0xff});

	}

protected:

	float m_speed = 300;
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
		case SystemEvent::DebugDraw:
			_DebugDraw();
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
		Object::Register<Player>();


		m_currentScene = new Scene;
		gSceneManager->SetCurrentScene(m_currentScene);

		gRenderer->SetDebugDraw(0);
		
		Entity* _player = m_currentScene->Spawn("Objects/Clone");
		_player->FindChild("Fire")->Enable(false);
		Entity* _camera = _player->AddChild("Camera");
		_camera->AddComponent<Camera>();
		_camera->Translate({ gRenderer->ScreenWidth() / -2.f, gRenderer->ScreenHeight() / -2.f });
		_camera->SetParent(nullptr);
		_player->AddComponent("Player");


		/*{
			Json _test;
			_test.Load(gResources->MakePath("Sprites/Test", "json").c_str());
			_test.Save(gResources->MakePath("Sprites/Test2", "json").c_str());
		}*/

		/*{

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
		}*/


		/*{
			EntityPtr _map = new Entity;
			_map->SetScene(m_currentScene);

			m_currentScene->Spawn("Objects/Tile1", Transform(0, 0, 1, 0))->SetParent(_map);
			m_currentScene->Spawn("Objects/Tile2", Transform(837, 0, 1, 0))->SetParent(_map);

			_map->Serialize().Save(gResources->MakePath("Maps/0", "json").c_str());
		}*/




		/*{

			EntityPtr _test = new Entity;
			_test->SetScene(m_currentScene);
			_test->SetName("Clone");
			//_test->Scale(2);
			//_test->Rotate(45 * Deg2Rad);

			SpriteRenderer* _sp = _test->AddComponent<SpriteRenderer>();
			_sp->SetSprite("Sprites/Clone");
			_sp->Enable(true);

			CircleCollider* _collider = _test->AddComponent<CircleCollider>();
			_collider->SetRadius(45);

			Body* _body = _test->AddComponent<Body>();



			Entity* _child = _test->AddChild();
			//_child->Scale(2.5f);
			_child->SetName("Fire");
			_child->Translate({ 15, 75 });

			Rotate* _rot = _test->AddComponent<Rotate>();
			_rot->speed = 1;


			SpriteRenderer* _sp2 = _child->AddComponent<SpriteRenderer>();
			_sp2->SetSprite("Sprites/Clone_Fire");
			//_sp2->Play("Run", AnimMode::Default);
			//_sp2->SetAnimationSpeed(.25f);

			//LifeTime* _lf = _child->AddComponent<LifeTime>();
			//_lf->maxLifeTime = 5;
			//_lf->Enable(false);

			//_test->Serialize().Save(gResources->MakePath("Objects/Clone", "json").c_str());

			EntityPtr _clone = _test->Clone();
			_clone->Serialize().Save(gResources->MakePath("Objects/Clone", "json").c_str());

			_clone->Translate({ 200, 0 });
		}*/

		/*{
			EntityPtr _test = new Entity;
			_test->SetScene(m_currentScene);

			

			m_currentScene->Spawn("Objects/Clone", Transform(0, 0, 2, 0))->SetParent(_test);
			m_currentScene->Spawn("Objects/Clone", Transform(200, 300, 2, 0))->SetParent(_test);

			_test->Serialize().Save(gResources->MakePath("Objects/Map", "json").c_str());
		}*/
		m_currentScene->Spawn("Objects/Clone", Transform(200, 200, 1, 0));
		m_currentScene->Spawn("Maps/0");


		/*{
			Json _desc;
			_desc.Load(gResources->MakePath("entity", "json").c_str());
			_desc.Save(gResources->MakePath("entity3", "json").c_str());

			EntityPtr _test = new Entity;
			_test->Deserialize(_desc);

			_test->SetScene(m_currentScene);
			_test->SetPosition({ 500, 500 });

			_test->Serialize().Save(gResources->MakePath("entity2", "json").c_str());
		} */


		// create ground
		/*{
			EntityPtr _ground = new Entity;
			//PhysicsShape* _shape = _ground->AddComponent<PhysicsShape>();
			//_shape->Create(PhysicsShape::Type::Box,);

			EdgeCollider* _collider = _ground->AddComponent<EdgeCollider>();
			_collider->Set({ 10, 10 }, { gRenderer->ScreenWidth() - 20.f, 10 });
			Body* _body = _ground->AddComponent<Body>();

			_ground->SetScene(m_currentScene);

			//_ground->Enable(false);
		}

		// create rigidbody
		{
			EntityPtr _test = new Entity;
			
			CircleCollider* _collider = _test->AddComponent<CircleCollider>();
			_collider->SetRadius(50);

			Body* _body = _test->AddComponent<Body>();
			
			_test->SetPosition({ 400, 400 });
			_test->SetScene(m_currentScene);
		}*/
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
	}
	//!
	void _Render(void)
	{
	}
	//!
	void _DebugDraw(void)
	{
		gRenderer->DrawCross(m_currentScene->GetSystem<RenderWorld>()->ScreenToWorld(gInput->CursorPos()), 20, {0xff, 0, 0, 0xff});
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
