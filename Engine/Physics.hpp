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
// PhysicsWorld
//----------------------------------------------------------------------------//

class PhysicsWorld : public SceneSystem
{
public:
	RTTI("PhysicsWorld");

	static void Register(void);

protected:
	void _ProcessFrame(void) override;
};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
