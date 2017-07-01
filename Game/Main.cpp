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

struct IntRect
{
	int x1, y1, x2, y2;
};
typedef arctic::Vec2Si32 IntVector2;

class Sprite : public RefCounted
{
public:

	struct Frame
	{
		String name;
		arctic::easy::Sprite sprite;
		IntRect rect;
	};

	void SetFramesCount(uint _count)
	{
		m_frames.resize(_count);
	}

	uint GetFramesCount(void)
	{
		return (uint)m_frames.size();
	}

	Frame& GetFrame(uint _index)
	{
		return m_frames[_index];
	}

	void Draw(uint _frame, const IntVector2& _pos, const IntVector2& _camera, float _zoom = 1, float _angle = 0)
	{

	}


protected:
	IntVector2 m_size;
	Array<Frame> m_frames;
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


protected:

	//!
	void _Init(void) override
	{  
		LOG("Initialize...");

		Object::Register<LifeTime>();

		m_currentScene = new Scene;

		EntityPtr _test = new Entity;
		_test->SetScene(m_currentScene);

		SpriteRenderer* _sp = _test->AddComponent<SpriteRenderer>();
		_sp->m_data.Load("Data/test.tga");


		Entity* _child = _test->AddChild();
		SpriteRenderer* _sp2 = _child->AddComponent<SpriteRenderer>();
		_sp2->m_data = _sp->m_data;
		_child->Rotate(-90 * Deg2Rad);
		_child->Scale(.5f);

		LifeTime* _lf = _child->AddComponent<LifeTime>();
		_lf->maxLifeTime = 5;

		_test->SetPosition({ 400, 400 });
		_test->Rotate(90 * Deg2Rad);
		_test->Scale(2);
	}
	//!
	void _Destroy(void) override
	{ 
		LOG("Destroy...");
	}
	//!
	void _BeginFrame(void) override
	{ 
		m_requireExit = m_requireExit || arctic::easy::IsKey(arctic::kKeyEscape);
	}
	//!
	void _ProcessFrame(void) override
	{
		m_currentScene->ProcessFrame();
	}
	//!
	void _EndFrame(void) override
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
