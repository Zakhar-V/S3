#pragma once

#include "Scene.hpp"
#include "Resource.hpp"

typedef SharedPtr<class Image> ImagePtr;
typedef SharedPtr<class Sprite> SpritePtr;

//----------------------------------------------------------------------------//
// AnimMode
//----------------------------------------------------------------------------//

enum class AnimMode
{
	Default,
	Once,
	Loop,
	PingPong,
};

//!
String AnimModeToString(AnimMode _code);
//!
AnimMode AnimModeFromString(const String& _name);

//----------------------------------------------------------------------------//
// Image
//----------------------------------------------------------------------------//

class Image : public Resource
{
public:
	RTTI("Image");

	typedef arctic::easy::Sprite Data;

	//!
	Image(void);

	//!
	Data& GetData(void) { return m_data; }

	//!
	void Load(const char* _filename) override;

protected:
	Data m_data;
};

//----------------------------------------------------------------------------//
// Sprite
//----------------------------------------------------------------------------//

class Sprite : public Resource
{
public:
	RTTI("Sprite");

	struct Animation
	{
		String name;
		uint start = 0;
		uint end = 0;
		float fps = 10;
		AnimMode mode = AnimMode::Once;
	};
	
	//!
	Sprite(void);
	//!
	~Sprite(void);

	//!
	Array<ImagePtr>& Images(void) { return m_images; }
	//!
	Image* GetImageByIndex(uint _index) { return m_images[_index]; }

	//!
	HashMap<String, uint>& Aliases(void) { return m_aliases; }
	//!
	Image* GetImageByName(const String& _name);

	//!
	HashMap<String, Animation>& Animations(void) { return m_animations; }
	//!
	Animation* GetAnimation(const String& _name);

	//!
	void Load(const char* _filename) override;

protected:
	Array<ImagePtr> m_images;
	HashMap<String, uint> m_aliases;
	HashMap<String, Animation> m_animations;
};

//----------------------------------------------------------------------------//
// RenderComponent
//----------------------------------------------------------------------------//

class RenderComponent abstract : public Component
{
public:
	RTTI("RenderComponent");

	Family GetFamily(void) override final { return Family::Render; }

	//!
	virtual void Update(void) { }
	//!
	virtual void Draw(const Vector2& _camera) { }

protected:
	friend class RenderWorld;

	RenderComponent* m_prevRenderComponent = nullptr;
	RenderComponent* m_nextRenderComponent = nullptr;
};

//----------------------------------------------------------------------------//
// SpriteRenderer
//----------------------------------------------------------------------------//

class SpriteRenderer : public RenderComponent
{
public:
	RTTI("SpriteRenderer");

	//!
	void SetSprite(Sprite* _sprite);
	//!
	void SetSprite(const String& _name);

	// TODO: more control

	//!
	void Play(const String& _name, AnimMode _mode = AnimMode::Default);
	//!
	void SetAnimation(const String& _name);
	//!
	void SetAnimationSpeed(float _speed);
	//!
	void SetAnimationMode(AnimMode _mode);
	//!
	void Play(bool _play = true);
	//!
	void Stop(bool _stop = true);

	//!
	void Update(void) override;
	//!
	void Draw(const Vector2& _camera) override;

	//!
	Json Serialize(void) override;
	//!
	void Deserialize(const Json& _src, class ObjectSolver* _context = nullptr) override;

protected:

	SpritePtr m_sprite;
	Sprite::Animation m_anim;
	uint m_currentFrame = 0;
	float m_animTime = 0;

	float m_animSpeed = 1;
	AnimMode m_animMode = AnimMode::Default;

	bool m_play = false;
};

//----------------------------------------------------------------------------//
// RenderWorld
//----------------------------------------------------------------------------//

class RenderWorld : public SceneSystem
{
public:
	RTTI("RenderWorld");

	static void Register(void);

protected:
	//!
	void _Start(Scene* _scene) override;
	//!
	void _Stop(void) override;
	//!
	void _AddComponent(Component* _component) override;
	//!
	void _RemoveComponent(Component* _component) override;
	//!
	void _Update(void) override;
	//!
	void _PostUpdate(void) override;
	//!
	void _Render(void) override;

	RenderComponent* m_first = nullptr;
	Array<RenderComponent*> m_visibleSet;
};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
