#pragma once

#include "Scene.hpp"

//----------------------------------------------------------------------------//
// AudioComponent
//----------------------------------------------------------------------------//

class AudioComponent abstract : public Component
{
public:
	RTTI("AudioComponent");

	Family GetFamily(void) override final { return Family::Audio; }

protected:
};

//----------------------------------------------------------------------------//
// AudioWorld
//----------------------------------------------------------------------------//

class AudioWorld : public SceneSystem
{
public:
	RTTI("AudioWorld");

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
