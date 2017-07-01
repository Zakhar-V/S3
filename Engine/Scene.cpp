#include "Scene.hpp"

//----------------------------------------------------------------------------//
// Component
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
Scene* Component::GetScene(void)
{
	return m_entity ? m_entity->GetScene() : nullptr;
}
//----------------------------------------------------------------------------//
void Component::Destroy(Component* _component)
{
	if (_component && _component->m_entity)
		_component->m_entity->RemoveComponent(_component);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Entity
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
Component* Entity::AddComponent(const char* _typename)
{
	ObjectPtr _obj = Object::Create(_typename);
	if (!_obj || !_obj->IsTypeOf<Component>())
	{
		LOG("Error: Unable to add %s component", _typename);
		return nullptr;
	}

	Component* _component = _obj.Cast<Component>();
	LL_LINK(m_component, _component, m_prevComponent, m_nextComponent);

	_component->m_entity = this;
	_component->AddRef();

	if (m_scene)
		m_scene->_AddComponent(_component);

	return _component;
}
//----------------------------------------------------------------------------//
void Entity::RemoveComponent(Component* _component)
{
	if (_component && _component->m_entity == this)
	{
		if (m_scene)
			m_scene->_RemoveComponent(_component);

		LL_UNLINK(m_component, _component, m_prevComponent, m_nextComponent);

		_component->m_entity = nullptr;
		_component->Release();
	}
}
//----------------------------------------------------------------------------//
Component* Entity::GetComponent(uint _type)
{
	for (Component* i = m_component; i; i = i->m_nextComponent)
	{
		if (i->IsTypeOf(_type))
			return i;
	}
	return nullptr;
}
//----------------------------------------------------------------------------//
void Entity::SetScene(Scene* _scene)
{
	if (m_parent && m_parent->m_scene != _scene)
		return;

	if (m_scene == _scene)
		return;

	if (!m_parent && !m_scene)
		AddRef();

	if (m_scene)
		m_scene->_RemoveEntity(this);

	m_scene = _scene;

	if (m_scene)
		m_scene->_AddEntity(this);

	for (Entity* i = m_child; i; i = i->m_next)
		i->SetScene(m_scene);

	if (!m_parent && !m_scene)
		Release();
}
//----------------------------------------------------------------------------//
void Entity::SetParent(Entity* _parent)
{
	if (m_parent == _parent)
		return;

	for (Entity* i = _parent; i; i = i->m_parent)
	{
		if (i == this)
			return;
	}

	if (!m_parent && !m_scene)
		AddRef();

	if (m_parent)
		LL_UNLINK(m_parent->m_child, this, m_prev, m_next);

	m_parent = _parent;

	if (m_parent)
	{
		LL_LINK(m_parent->m_child, this, m_prev, m_next);

		if (m_parent->m_scene != m_scene)
			SetScene(m_parent->m_scene);
	}

	// TODO: update transform

	if (!m_parent && !m_scene)
		Release();
}
//----------------------------------------------------------------------------//
void Entity::Destroy(Entity* _entity)
{
	if (_entity)
	{
		_entity->AddRef();
		_entity->SetParent(nullptr);
		_entity->SetScene(nullptr);
		_entity->Release();
	}
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// SceneSystem
//----------------------------------------------------------------------------//

Array<SceneSystem::Definition> SceneSystem::s_defaultSystems;

//----------------------------------------------------------------------------//
void SceneSystem::AddDefaultSystem(const char* _name, int _priority)
{
	Definition _info;
	_info.id = StringUtils::Hash(_name);
	for (auto& i : s_defaultSystems)
	{
		if (i.id == _info.id)
		{
			if (i.priority != _priority)
			{
				LOG("Change priority for %s form %d to %d", _name, i.priority, _priority);
				i.priority = _priority;
			}
			return;
		}
	}

	_info.name = _name;
	_info.priority = _priority;
	s_defaultSystems.push_back(_info);
}
//----------------------------------------------------------------------------//
int SceneSystem::Priority(uint _type)
{
	for (auto& i : s_defaultSystems)
	{
		if (i.id == _type)
			return i.priority;
	}
	return 0;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Scene
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
Scene::Scene(void)
{
	for (const auto& i : SceneSystem::DefaultSystems())
	{
		GetOrAddSystem(i.name);
	}
}
//----------------------------------------------------------------------------//
Scene::~Scene(void)
{
	while (m_root)
		m_root->SetScene(nullptr);

	for (auto& i : m_systems)
		i.second->_Stop();

	m_systems.clear();
}
//----------------------------------------------------------------------------//
SceneSystem* Scene::GetSystem(uint _type)
{
	auto& _sys = m_systems.find(_type);
	return _sys != m_systems.end() ? _sys->second : nullptr;
}
//----------------------------------------------------------------------------//
SceneSystem* Scene::GetOrAddSystem(const char* _name)
{
	uint _type = StringUtils::Hash(_name);
	auto& _iter = m_systems.find(_type);
	if (_iter != m_systems.end())
		return _iter->second;

	ObjectPtr _obj = Object::Create(_name);
	if (!_obj || !_obj->IsTypeOf<SceneSystem>())
	{
		LOG("Error: Unable to add %s scene system", _name);
		return nullptr;
	}

	SceneSystemPtr _sys = _obj.Cast<SceneSystem>();
	m_systems[_type] = _obj.Cast<SceneSystem>();

	m_systemOrder.push_back(_sys);
	_SortSystems();

	_sys->_Start(this);

	return _sys;
}
//----------------------------------------------------------------------------//
void Scene::_SortSystems(void)
{
	std::sort(m_systemOrder.begin(), m_systemOrder.end(),
		[](SceneSystem* lhs, SceneSystem* rhs)
	{
		return SceneSystem::Priority(lhs->GetTypeID()) > SceneSystem::Priority(rhs->GetTypeID());
	});
}
//----------------------------------------------------------------------------//
void Scene::ProcessFrame(void)
{
	for (auto i : m_systemOrder)
		i->_ProcessFrame();
}
//----------------------------------------------------------------------------//
void Scene::_LinkRootEntity(Entity* _entity)
{
	LL_LINK(m_root, _entity, m_prev, m_next);
}
//----------------------------------------------------------------------------//
void Scene::_UnlinkRootEntity(Entity* _entity)
{
	LL_UNLINK(m_root, _entity, m_prev, m_next);
}
//----------------------------------------------------------------------------//
void Scene::_AddEntity(Entity* _entity)
{
	if (!_entity->m_parent)
		_LinkRootEntity(_entity);

	for (Component* i = _entity->GetFirstComponent(); i; i = i->GetNextComponent())
		_AddComponent(i);
}
//----------------------------------------------------------------------------//
void Scene::_RemoveEntity(Entity* _entity)
{
	if (!_entity->m_parent)
		_UnlinkRootEntity(_entity);

	for (Component* i = _entity->GetFirstComponent(); i; i = i->GetNextComponent())
		_RemoveComponent(i);
}
//----------------------------------------------------------------------------//
void Scene::_AddComponent(Component* _component)
{
	ASSERT(_component != nullptr);

	for (auto i : m_systemOrder)
		i->_AddComponent(_component);
}
//----------------------------------------------------------------------------//
void Scene::_RemoveComponent(Component* _component)
{
	ASSERT(_component != nullptr);

	for (auto i : m_systemOrder)
		i->_RemoveComponent(_component);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
