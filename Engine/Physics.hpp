#pragma once

#include "Scene.hpp"

//----------------------------------------------------------------------------//
// PhysicsComponent
//----------------------------------------------------------------------------//

class PhysicsComponent abstract : public Component
{
public:
	RTTI("PhysicsComponent");

	Family GetFamily(void) override final { return Family::Physics; }

protected:
};

//----------------------------------------------------------------------------//
// Collider
//----------------------------------------------------------------------------//

class Collider : public PhysicsComponent
{
public:

protected:
};

//----------------------------------------------------------------------------//
// RigidBody
//----------------------------------------------------------------------------//

class RigidBody : public PhysicsComponent
{
public:

protected:
};

//----------------------------------------------------------------------------//
// PhysicsWorld
//----------------------------------------------------------------------------//

class PhysicsWorld : public SceneSystem
{
public:
	RTTI("PhysicsWorld");

	static void Register(void);

protected:
	//!
	void _Update(void) override;
	//!
	void _PostUpdate(void) override;
};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
