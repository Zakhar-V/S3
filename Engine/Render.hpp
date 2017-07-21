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

	//!\return scale factor for rendering
	float GetScale(void) { return m_scale; }
	//!
	const Vector2& Size(void) { return m_size; }

	//!
	void Load(const char* _filename) override;

protected:
	Array<ImagePtr> m_images;
	HashMap<String, uint> m_aliases;
	HashMap<String, Animation> m_animations;
	float m_scale = 1;
	Vector2 m_size = { 0, 0 };
	Vector2 m_pivot = { 0, 0 };
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
	virtual void Draw(const Vector2& _camera, float _zoom = 1) { }

protected:
	friend class RenderWorld;

	RenderComponent* m_prevRenderComponent = nullptr;
	RenderComponent* m_nextRenderComponent = nullptr;
};

//----------------------------------------------------------------------------//
// Camera
//----------------------------------------------------------------------------//

class Camera : public RenderComponent
{
public:
	RTTI("Camera");

	//!
	struct Params
	{
		Vector2 pos = { 0, 0 };
		float zoom = 1;
	};


	//!
	const Vector2& Position(void);
	//!
	float Zoom(void);

	//!
	void SetBackgroundColor(const Color4ub& _color) { m_backgroundColor = _color; }
	//!
	const Color4ub& GetBackgroundColor(void) { return m_backgroundColor; }

	//!
	void Clone(Component* _src) override;

	//!
	Json Serialize(void) override;
	//!
	void Deserialize(const Json& _src, class ObjectSolver* _context = nullptr) override;

protected:
	friend class RenderWorld;

	Params m_params;
	Color4ub m_backgroundColor = { 0, 0, 0, 0xff };
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
	void Draw(const Vector2& _camera, float _zoom = 1) override;

	//!
	void Clone(Component* _src) override;

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
	Color4ub m_color = { 0xff, 0xff, 0xff, 0xff };
};

//----------------------------------------------------------------------------//
// RenderWorld
//----------------------------------------------------------------------------//

class RenderWorld : public SceneSystem
{
public:
	RTTI("RenderWorld");
	//!
	static void Register(void);

	//!
	void SetCamera(Camera* _camera);
	//!
	Camera* GetCamera(void) { return m_camera; }
	//!
	const Vector2& GetCameraPos(void) { return m_cameraPos; }
	//!
	float GetCameraZoom(void) { return m_cameraZoom; }

	//!
	Vector2 ScreenToWorld(const Vector2& _screen);
	//!
	Vector2 WorldToScreen(const Vector2& _world);

protected:
	friend class Camera;
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
	//!
	void _DebugDraw(void) override;

	RenderComponent* m_first = nullptr;
	Array<RenderComponent*> m_visibleSet;

	Camera* m_camera = nullptr;
	Vector2 m_cameraPos = { 0, 0 };
	float m_cameraZoom = 1;
};

//----------------------------------------------------------------------------//
// DebugDraw
//----------------------------------------------------------------------------//

struct DebugDraw
{
	enum Flags
	{
		// [GRAPHICS]

		Bound = 0x1,

		// [PHYSICS]

		Collider = 0x2,
		Contact = 0x4,
		ContactNormal = 0x8,


		// [AUDIO]
		AudioSource = 0x10,
	};
};

//----------------------------------------------------------------------------//
// Renderer
//----------------------------------------------------------------------------//

#define gRenderer Renderer::Instance

class Renderer : public Module<Renderer>
{
public:
	//!
	Renderer(void);
	//!
	~Renderer(void);

	//!
	bool OnEvent(int _type, void* _arg) override;
	
	//!
	void SetScreenSize(uint _width, uint _height);
	//!
	uint ScreenWidth(void) { return m_screenWidth; }
	//!
	uint ScreenHeight(void) { return m_screenHeight; }
	//!
	Color4ub* BackBuffer(void) { return m_backBuffer; }

	//!
	void SetDebugDraw(uint _flags) { m_debugDraw = _flags; }
	//!
	uint GetDebugDraw(void) { return m_debugDraw; }

	//!
	void SetBackgroundColor(const Color4ub& _color) { m_backgroundColor = _color; }
	//!
	const Color4ub& GetBackgroundColor(void) { return m_backgroundColor; }

	//!
	void SetCamera(const Vector2& _pos, float _zoom = 1);
	//!
	void DrawSprite(Sprite* _sprite, uint _frame,uint _targetHeight, const Transform& _transform, const Color4ub& _color = { 0xff, 0xff, 0xff, 0xff });
	
	//!
	void DrawLine(const Vector2& _start, const Vector2& _end, const Color4ub& _color);
	//!
	void DrawWireCircle(const Vector2& _center, float _radius, const Color4ub& _color);
	//!
	void DrawCross(const Vector2& _center, float _size, const Color4ub& _color);

protected:
	//!
	void _GetBackBuffer(void);

	uint m_debugDraw = 0xffffffff;

	Color4ub m_backgroundColor = { 0, 0, 0, 0 };
	
	Color4ub* m_backBuffer = nullptr;
	uint m_screenWidth = 0;
	uint m_screenHeight = 0;

	Vector2 m_cameraPos = { 0, 0 };
	float m_cameraZoom = 1;
};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
