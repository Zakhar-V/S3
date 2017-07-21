#pragma once

#include "Scene.hpp"
#include "Box2D\Box2D.h"

//----------------------------------------------------------------------------//
// Defs
//----------------------------------------------------------------------------//

class Collider;
class Body;
class Joint;

//----------------------------------------------------------------------------//
// PhysicsComponent
//----------------------------------------------------------------------------//

class PhysicsComponent abstract : public Component
{
public:
	RTTI("PhysicsComponent");

	Family GetFamily(void) override final { return Family::Physics; }

protected:

	class PhysicsWorld * m_world = nullptr;
};

//----------------------------------------------------------------------------//
// Collider
//----------------------------------------------------------------------------//

class Collider abstract : public PhysicsComponent
{
public:
	RTTI("Collider");

	//!
	Collider(void);
	//!
	~Collider(void);

	//!
	void SetCenter(const Vector2& _center);
	//!
	const Vector2& GetCenter(void) { return m_center; }

	//!
	b2Fixture* _Handle(void) { return m_fixture; }
	//!
	virtual void _DebugDraw(void);

	//!
	void Clone(Component* _src) override;

	//!
	Json Serialize(void) override;
	//!
	void Deserialize(const Json& _src, class ObjectSolver* _context = nullptr) override;

protected:
	friend class Body;
	friend class PhysicsWorld;

	//!
	void _Bind(void) override;
	//!
	void _Unbind(void) override;
	//!
	void _Enable(void) override;
	//!
	void _Disable(void) override;

	//!
	void _Create(void);
	//!
	void _Destroy(void);
	//!
	void _UpdateFilter(void);

	//!
	virtual b2Shape* _Shape(void) = 0;
	//!
	virtual void _UpdateShape(void) = 0;

	Body* m_body = nullptr;
	b2Fixture* m_fixture = nullptr;

	Vector2 m_center = { 0, 0 };
	float m_friction = .2f;
	float m_restitution = 0;
	float m_density = 0;
	bool m_trigger = false;
};

//----------------------------------------------------------------------------//
// CircleCollider
//----------------------------------------------------------------------------//

class CircleCollider : public Collider
{
public:
	RTTI("CircleCollider");

	//!
	void SetRadius(float _radius);

	//!
	void _DebugDraw(void) override;

	//!
	void Clone(Component* _src) override;

	//!
	Json Serialize(void) override;
	//!
	void Deserialize(const Json& _src, class ObjectSolver* _context = nullptr) override;

protected:
	//!
	b2Shape* _Shape(void) override { return &m_shape; }
	//!
	void _UpdateShape(void) override;

	float m_radius = 1;
	b2CircleShape m_shape;
};

//----------------------------------------------------------------------------//
// BoxCollider
//----------------------------------------------------------------------------//

class BoxCollider : public Collider
{
public:
	RTTI("BoxCollider");

	//!
	void SetRect(const Rect& _rect);

	//!
	void _DebugDraw(void) override;

protected:
	//!
	b2Shape* _Shape(void) override { return &m_shape; }
	//!
	void _UpdateShape(void) override;

	Rect m_rect = Rect::Identity;
	b2PolygonShape m_shape;
};

//----------------------------------------------------------------------------//
// EdgeCollider
//----------------------------------------------------------------------------//

class EdgeCollider : public Collider
{
public:
	RTTI("EdgeCollider");

	//!
	void Set(const Vector2& _v0, const Vector2& _v1);
	//!
	uint NumPoints(void) { return 2; }
	//!
	Vector2* Points(void) { return m_points; }
	//!
	Vector2& Point(uint _index) { ASSERT(_index < 2); return m_points[_index]; }
	//! 
	void ApplyPoints(void);

	//!
	void _DebugDraw(void) override;

protected:
	//!
	b2Shape* _Shape(void) override { return &m_shape; }
	//!
	void _UpdateShape(void) override;

	Vector2 m_points[2];
	b2EdgeShape m_shape;
};

//----------------------------------------------------------------------------//
// PolygonCollider
//----------------------------------------------------------------------------//

class PolygonCollider : public Collider
{
public:
	RTTI("PolygonCollider");

	//!
	void _DebugDraw(void) override;

protected:
	//!
	b2Shape* _Shape(void) override { return &m_shape; }
	//!
	void _UpdateShape(void) override;

	b2PolygonShape m_shape;
};

//----------------------------------------------------------------------------//
// ChainCollider
//----------------------------------------------------------------------------//

class ChainCollider : public Collider
{
public:
	RTTI("ChainCollider");

	//!
	void _DebugDraw(void) override;

protected:
	//!
	b2Shape* _Shape(void) override { return &m_shape; }
	//!
	void _UpdateShape(void) override;

	b2ChainShape m_shape;
};

//----------------------------------------------------------------------------//
// Body
//----------------------------------------------------------------------------//

class Body : public PhysicsComponent
{
public:
	RTTI("Body");

	enum class Type
	{
		Static, 
		Kinematic,
		Dynamic,
	};

	//!
	b2Body* _Handle(void) { return m_body; }

protected:
	friend class PhysicsWorld;

	//!
	void _Bind(void) override;
	//!
	void _Unbind(void) override;
	//!
	void _Enable(void) override;
	//!
	void _Disable(void) override;
	
	//!
	void _Create(void);
	//!
	void _Destroy(void);
	//!
	void _DebugDraw(void);

	Body* m_prevBody = nullptr;
	Body* m_nextBody = nullptr;

	b2Body* m_body = nullptr;
};

//----------------------------------------------------------------------------//
// PhysicsWorld
//----------------------------------------------------------------------------//

class PhysicsWorld : public SceneSystem, public b2ContactListener
{
public:
	RTTI("PhysicsWorld");

	//!
	static void Register(void);

	//!
	PhysicsWorld(void);
	//!
	~PhysicsWorld(void);

	//!
	b2World* _Handle(void) { return &m_world; }

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
	void _DebugDraw(void) override;

	//!
	void BeginContact(b2Contact* _contact) override;
	//!
	void EndContact(b2Contact* _contact) override;
	//!
	void PreSolve(b2Contact* _contact, const b2Manifold* _oldManifold) override;
	//!
	void PostSolve(b2Contact* _contact, const b2ContactImpulse* _impulse) override;


	Body* m_firstBody = nullptr;
	b2World m_world;
};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
