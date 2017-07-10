#include "Physics.hpp"

//----------------------------------------------------------------------------//
// Collider
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
Collider::Collider(void)
{
}
//----------------------------------------------------------------------------//
Collider::~Collider(void)
{
}
//----------------------------------------------------------------------------//
void Collider::SetCenter(const Vector2& _center)
{
	if (m_center != _center)
	{
		m_center = _center;
		_UpdateShape();
	}
}
//----------------------------------------------------------------------------//
void Collider::_Bind(void)
{
	m_body = m_entity->GetComponent<Body>();
}
//----------------------------------------------------------------------------//
void Collider::_Unbind(void)
{
	ASSERT(!m_fixture);

	m_body = nullptr;
}
//----------------------------------------------------------------------------//
void Collider::_Enable(void)
{
	if (m_fixture)
		_UpdateFilter();
}
//----------------------------------------------------------------------------//
void Collider::_Disable(void)
{
	if (m_fixture)
		_UpdateFilter();
}
//----------------------------------------------------------------------------//
void Collider::_Create(void)
{
	ASSERT(m_body && m_body->_Handle() && !m_fixture);

	b2FixtureDef _def;
	_def.shape = _Shape();
	_def.userData = this;
	_def.friction = m_friction;
	_def.restitution = m_restitution;
	_def.density = m_density;
	_def.isSensor = m_trigger;

	m_fixture = m_body->_Handle()->CreateFixture(&_def);
	_UpdateFilter();
}
//----------------------------------------------------------------------------//
void Collider::_Destroy(void)
{
	ASSERT(m_body && m_body->_Handle() && m_fixture);

	m_body->_Handle()->DestroyFixture(m_fixture);
	m_fixture = nullptr;
}
//----------------------------------------------------------------------------//
void Collider::_UpdateFilter(void)
{
	ASSERT(m_fixture);

	b2Filter _filter;
	if (!IsEnabled())
	{
		_filter.groupIndex = 0;
		_filter.maskBits = 0;
		_filter.categoryBits = 0;
	}

	m_fixture->SetFilterData(_filter);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// CircleCollider
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void CircleCollider::SetRadius(float _radius)
{
	m_radius = _radius;
	_UpdateShape();
}
//----------------------------------------------------------------------------//
void CircleCollider::_UpdateShape(void)
{
	m_shape.m_p = { m_center.x, m_center.y };
	m_shape.m_radius = m_radius * m_entity->GetTransform().Scale();
}
//----------------------------------------------------------------------------//
void CircleCollider::_DebugDraw(void)
{

}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// BoxCollider
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void BoxCollider::SetRect(const Rect& _rect)
{
	m_rect = _rect;
	_UpdateShape();
}
//----------------------------------------------------------------------------//
void BoxCollider::_UpdateShape(void)
{
	float _scale = m_entity->GetTransform().Scale() * .5f;
	m_shape.SetAsBox(m_rect.Width() * _scale, m_rect.Height() * _scale, { m_center.x, m_center.y }, 0);
}
//----------------------------------------------------------------------------//
void BoxCollider::_DebugDraw(void)
{

}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// EdgeCollider
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void EdgeCollider::Set(const Vector2& _v0, const Vector2& _v1)
{
	m_points[0] = _v0;
	m_points[1] = _v1;
	ApplyPoints();
}
//----------------------------------------------------------------------------//
void EdgeCollider::ApplyPoints(void)
{
	_UpdateShape();
}
//----------------------------------------------------------------------------//
void EdgeCollider::_UpdateShape(void)
{
	float _scale = m_entity->GetTransform().Scale() * .5f;

	Vector2 _v0 = m_points[0] * _scale + m_center;
	Vector2 _v1 = m_points[1] * _scale + m_center;
	m_shape.Set({ _v0.x, _v0.y }, { _v1.x, _v1.y });
}
//----------------------------------------------------------------------------//
void EdgeCollider::_DebugDraw(void)
{

}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// PolygonCollider
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void PolygonCollider::_UpdateShape(void)
{

}
//----------------------------------------------------------------------------//
void PolygonCollider::_DebugDraw(void)
{

}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// ChainCollider
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void ChainCollider::_UpdateShape(void)
{

}
//----------------------------------------------------------------------------//
void ChainCollider::_DebugDraw(void)
{

}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Body
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void Body::_Bind(void)
{
	for (Collider* i = m_entity->GetComponent<Collider>(); i; i = i->GetNextComponent<Collider>())
		i->m_body = this;
}
//----------------------------------------------------------------------------//
void Body::_Unbind(void)
{
	ASSERT(!m_body);

	for (Collider* i = m_entity->GetComponent<Collider>(); i; i = i->GetNextComponent<Collider>())
		i->m_body = nullptr;
}
//----------------------------------------------------------------------------//
void Body::_Enable(void)
{
	if (m_body)
		m_body->SetActive(true);
}
//----------------------------------------------------------------------------//
void Body::_Disable(void)
{
	if (m_body)
		m_body->SetActive(false);
}
//----------------------------------------------------------------------------//
void Body::_Create(void)
{
	ASSERT(m_world && !m_body);

	b2BodyDef _def;
	_def.userData = this;
	_def.position = { 0, 0 };
	_def.angle = 0;
	_def.linearVelocity = { 0, 0 };
	_def.angularVelocity = 0;
	_def.linearDamping = 0;
	_def.angularDamping = 0;
	_def.allowSleep = true;
	_def.awake = true;
	_def.fixedRotation = false;
	_def.bullet = false;
	_def.type = b2_staticBody;
	_def.active = IsEnabled();
	_def.gravityScale = 1;

	m_body = m_world->_Handle()->CreateBody(&_def);
	//TODO set transform

	for (Collider* i = m_entity->GetComponent<Collider>(); i; i = i->GetNextComponent<Collider>())
		i->_Create();
}
//----------------------------------------------------------------------------//
void Body::_Destroy(void)
{
	ASSERT(m_world && m_body);

	for (Collider* i = m_entity->GetComponent<Collider>(); i; i = i->GetNextComponent<Collider>())
		i->_Destroy();

	m_world->_Handle()->DestroyBody(m_body);
	m_body = nullptr;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// PhysicsWorld
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void PhysicsWorld::Register(void)
{
	//Object::Register<PhysicsComponent>();
	Object::Register<CircleCollider>();
	Object::Register<BoxCollider>();
	Object::Register<PolygonCollider>();
	Object::Register<EdgeCollider>();
	Object::Register<ChainCollider>();
	Object::Register<Body>(Component::Single);

	Object::Register<PhysicsWorld>();
	AddDefaultSystem(TypeName, PHYSICS_PRIORITY);
}
//----------------------------------------------------------------------------//
PhysicsWorld::PhysicsWorld(void) :
	m_world(b2Vec2(0, -9.8f))
{
	
}
//----------------------------------------------------------------------------//
PhysicsWorld::~PhysicsWorld(void)
{

}
//----------------------------------------------------------------------------//
void PhysicsWorld::_Start(Scene* _scene)
{

}
//----------------------------------------------------------------------------//
void PhysicsWorld::_Stop(void)
{

}
//----------------------------------------------------------------------------//
void PhysicsWorld::_AddComponent(Component* _component)
{
	if (_component->IsTypeOf<Body>())
	{
		Body* _body = static_cast<Body*>(_component);
		LL_LINK(m_firstBody, _body, m_prevBody, m_nextBody);

		_body->m_world = this;
		//if (_body->IsEnabled())
			_body->_Create();
	}
	else if (_component->IsTypeOf<Collider>())
	{
		//Collider* _collider = static_cast<Collider*>(_component);
		// do nothing
	}
}
//----------------------------------------------------------------------------//
void PhysicsWorld::_RemoveComponent(Component* _component)
{
	if (_component->IsTypeOf<Body>())
	{
		Body* _body = static_cast<Body*>(_component);
		LL_UNLINK(m_firstBody, _body, m_prevBody, m_nextBody);

		if (_body->IsEnabled())
			_body->_Destroy();
		_body->m_world = nullptr;
	}
	else if (_component->IsTypeOf<Collider>())
	{
		Collider* _collider = static_cast<Collider*>(_component);
		if (_collider->m_body)
			_collider->_Destroy();
	}
}
//----------------------------------------------------------------------------//
void PhysicsWorld::_Update(void)
{
}
//----------------------------------------------------------------------------//
void PhysicsWorld::_PostUpdate(void)
{
}
//----------------------------------------------------------------------------//
void PhysicsWorld::BeginContact(b2Contact* _contact)
{

}
//----------------------------------------------------------------------------//
void PhysicsWorld::EndContact(b2Contact* _contact)
{

}
//----------------------------------------------------------------------------//
void PhysicsWorld::PreSolve(b2Contact* _contact, const b2Manifold* _oldManifold)
{

}
//----------------------------------------------------------------------------//
void PhysicsWorld::PostSolve(b2Contact* _contact, const b2ContactImpulse* _impulse)
{

}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
