#pragma once

#include "Scene.hpp"

//----------------------------------------------------------------------------//
// LogicComponent
//----------------------------------------------------------------------------//

class LogicComponent abstract : public Component
{
public:
	RTTI("LogicComponent");

	Family GetFamily(void) override final { return Family::Logic; }

	//!
	virtual void Start(void) { }
	//!
	virtual void Destroy(void) { }
	//!
	virtual void Update(void) { }

protected:
	friend class LogicSystem;

	LogicComponent* m_prevLogicComponent = nullptr;
	LogicComponent* m_nextLogicComponent = nullptr;
};

//----------------------------------------------------------------------------//
// LogicSystem
//----------------------------------------------------------------------------//

class LogicSystem : public SceneSystem
{
public:
	RTTI("LogicSystem");

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

	LogicComponent* m_first = nullptr;
};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
