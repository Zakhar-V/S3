#include "Logic.hpp"

//----------------------------------------------------------------------------//
// LogicSystem
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void LogicSystem::Register(void)
{
	//Object::Register<LogicComponent>();

	Object::Register<LogicSystem>();
	AddDefaultSystem(TypeName, LOGIC_PRIORITY);
}
//----------------------------------------------------------------------------//
void LogicSystem::_Start(Scene* _scene)
{

}
//----------------------------------------------------------------------------//
void LogicSystem::_Stop(void)
{

}
//----------------------------------------------------------------------------//
void LogicSystem::_AddComponent(Component* _component)
{

	if (_component->IsTypeOf<LogicComponent>())
	{
		LogicComponent* _child = static_cast<LogicComponent*>(_component);

		_child->AddRef();
		LL_LINK(m_first, _child, m_prevLogicComponent, m_nextLogicComponent);
		_child->Start();
	}
}
//----------------------------------------------------------------------------//
void LogicSystem::_RemoveComponent(Component* _component)
{
	if (_component->IsTypeOf<LogicComponent>())
	{
		LogicComponent* _child = static_cast<LogicComponent*>(_component);
	
		_child->Destroy();
		LL_UNLINK(m_first, _child, m_prevLogicComponent, m_nextLogicComponent);
		_child->Release();
	}
}
//----------------------------------------------------------------------------//
void LogicSystem::_Update(void)
{
	for (LogicComponent* i = m_first; i;)
	{
		LogicComponent* _next = i->m_nextLogicComponent;
		i->Update();
		i = _next;
	}
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//

