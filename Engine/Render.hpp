#pragma once

#include "Scene.hpp"

//----------------------------------------------------------------------------//
// RenderComponent
//----------------------------------------------------------------------------//

class RenderComponent abstract : public Component
{
public:
	RTTI("RenderComponent");

	Family GetFamily(void) override final { return Family::Render; }

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

	arctic::easy::Sprite m_data; // temp

	void Draw(const Vector2& _camera) override;
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
	void _ProcessFrame(void) override;

	RenderComponent* m_first = nullptr;
	Array<RenderComponent*> m_visibleSet;
};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
