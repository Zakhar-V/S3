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

protected:
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
	void _ProcessFrame(void) override;
};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
