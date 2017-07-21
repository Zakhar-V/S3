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
bool Component::IsEnabled(void)
{
	return m_enabled && m_entityEnabled;
}
//----------------------------------------------------------------------------//
void Component::Enable(bool _enable)
{
	bool _oldState = IsEnabled();
	m_enabled = _enable;
	bool _newState = IsEnabled();
	if (_newState != _oldState)
		_newState ? _Enable() : _Disable();
}
//----------------------------------------------------------------------------//
void Component::_OnEntityEnable(bool _enable)
{
	bool _oldState = IsEnabled();
	m_entityEnabled = _enable;
	bool _newState = IsEnabled();
	if (_newState != _oldState)
		_newState ? _Enable() : _Disable();
}
//----------------------------------------------------------------------------//
Json Component::Serialize(void)
{
	Json _dst;
	_dst["Type"] = GetTypeName();
	_dst["Address"] = (uintptr_t)(void*)this;
	_dst["Enabled"] = m_enabled;

	return _dst;
}
//----------------------------------------------------------------------------//
void Component::Deserialize(const Json& _src, class ObjectSolver* _context)
{
	m_oldAddress = (void*)(uintptr_t)_src["Address"];
	Enable(_src["Enabled"]);

	if (_context)
	{
		_context->AddObject(this, m_oldAddress);
	}
}
//----------------------------------------------------------------------------//
void Component::SolveObjects(ObjectSolver* _context)
{
	// Do nothing
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Entity
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
Component* Entity::AddComponent(const char* _typename)
{
	Component* _component = nullptr;

	TypeInfo* _typeinfo = Object::GetTypeInfo(_typename);
	if (_typeinfo && _typeinfo->Factory)
	{
		if (_typeinfo->flags & Component::Single)
		{
			Component* _exists = GetComponent(_typeinfo->type);
			if (_exists)
			{
				LOG("Error: %s is already attached to Entity", _typename);
				return _exists;
			}
		}

		ObjectPtr _obj = Object::Create(_typename);
		if (!_obj || !_obj->IsTypeOf<Component>())
		{
			LOG("Error: Unable to add %s component", _typename);
			return nullptr;
		}

		_component = _obj.Cast<Component>();
		_component->AddRef();
	}
	else
	{
		LOG("Error: Factory for %s not found", _typename);
		return nullptr;
	}

	LL_LINK(m_component, _component, m_prevComponent, m_nextComponent);

	_component->m_entity = this;
	_component->_Bind();

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

		_component->_Unbind();
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
void Entity::SetParent(Entity* _parent, bool _keepWorldTransform)
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

	Transform _transform = GetTransform();

	if (m_parent)
	{
		LL_UNLINK(m_parent->m_child, this, m_prev, m_next);
	}
	else if(m_scene)
		m_scene->_UnlinkRootEntity(this);

	m_parent = _parent;

	if (m_parent)
	{
		LL_LINK(m_parent->m_child, this, m_prev, m_next);

		if (m_parent->m_scene != m_scene)
			SetScene(m_parent->m_scene);
	}
	else if (m_scene)
		m_scene->_LinkRootEntity(this);

	if(_keepWorldTransform)
		SetTransform(_transform);

	_OnParentEnable(m_parent ? m_parent->IsEnabled() : true);
	_UpdateDepth();

	for (Component* i = m_component; i; i = i->m_nextComponent)
		i->_OnEntityParentChanged();

	if (!m_parent && !m_scene)
		Release();
}
//----------------------------------------------------------------------------//
Entity* Entity::FindChild(const String& _name, bool _recursive)
{
	for (Entity* i = m_child; i; i = i->m_next)
	{
		if (i->m_name == _name)
			return i;
	}

	if (_recursive)
	{
		for (Entity* i = m_child; i; i = i->m_next)
		{
			Entity* _child = i->FindChild(_name, true);
			if (_child)
				return _child;
		}
	}
	return nullptr;
}
//----------------------------------------------------------------------------//
Entity* Entity::AddChild(void)
{
	EntityPtr _child = new Entity;
	_child->SetParent(this, false);
	return _child;
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
bool Entity::IsEnabled(void)
{
	return m_enabled && m_parentEnabled;
}
//----------------------------------------------------------------------------//
void Entity::Enable(bool _enable)
{
	bool _oldState = IsEnabled();
	m_enabled = _enable;
	bool _newState = IsEnabled();
	if (_newState != _oldState)
	{
		for (Component* i = m_component; i; i = i->m_nextComponent)
			i->_OnEntityEnable(_newState);

		for (Entity* i = m_child; i; i = i->m_next)
			i->_OnParentEnable(_newState);
	}
}
//----------------------------------------------------------------------------//
void Entity::_OnParentEnable(bool _enable)
{
	bool _oldState = IsEnabled();
	m_parentEnabled = _enable;
	bool _newState = IsEnabled();
	if (_newState != _oldState)
	{
		for (Component* i = m_component; i; i = i->m_nextComponent)
			i->_OnEntityEnable(_newState);
		for (Entity* i = m_child; i; i = i->m_next)
			i->_OnParentEnable(_newState);
	}
}
//----------------------------------------------------------------------------//
void Entity::_UpdateDepth(void)
{
	m_depth = m_parent ? (m_parent->m_depth + 1) : 0;
	for (Entity* i = m_child; i; i = i->m_next)
		i->_UpdateDepth();
}
//----------------------------------------------------------------------------//
void Entity::Rotate(float _r)
{
	m_angle += _r;
	_InvalidateTransform();
}
//----------------------------------------------------------------------------//
void Entity::SetRotation(float _r)
{
	m_angle = _r;
	_InvalidateTransform();
}
//----------------------------------------------------------------------------//
void Entity::Scale(float _s)
{
	m_scale *= _s;
	_InvalidateTransform();
}
//----------------------------------------------------------------------------//
void Entity::SetScale(float _s)
{
	m_scale = _s;
	_InvalidateTransform();
}
//----------------------------------------------------------------------------//
void Entity::Translate(const Vector2& _t)
{
	m_pos += _t;
	_InvalidateTransform();
}
//----------------------------------------------------------------------------//
void Entity::SetPosition(const Vector2& _t)
{
	m_pos = _t;
	_InvalidateTransform();
}
//----------------------------------------------------------------------------//
const Transform& Entity::GetTransform(void)
{
	_UpdateTransform();
	return m_transform;
}
//----------------------------------------------------------------------------//
void Entity::SetTransform(const Transform& _m)
{
	Transform _local = _m;
	if (m_parent)
		_local = m_parent->GetTransform().Inverse() * _local;

	m_scale = _local.Scale();
	m_angle = _local.Angle() * Deg2Rad;
	m_pos = _local.Pos();

	_InvalidateTransform();
}
//----------------------------------------------------------------------------//
void Entity::_InvalidateTransform(void)
{
	if (m_transformUpdated)
	{
		m_transformUpdated = false;

		for (Component* i = m_component; i; i = i->m_nextComponent)
			i->_OnEntityTransformChanged();

		for (Entity* i = m_child; i; i = i->m_next)
			i->_InvalidateTransform();
	}
}
//----------------------------------------------------------------------------//
void Entity::_UpdateTransform(void)
{
	if (!m_transformUpdated)
	{
		m_transformUpdated = true;

		Transform _local(m_pos.x, m_pos.y, m_scale, m_angle);
		if (m_parent)
			m_transform = m_parent->GetTransform() * _local;
		else
			m_transform = _local;

		for (Component* i = m_component; i; i = i->m_nextComponent)
			i->_OnEntityTransformUpdated();
	}
}
//----------------------------------------------------------------------------//
void Entity::SendEvent(int _event, void* _arg)
{
	if (IsEnabled())
	{
		for (Component* i = m_component; i; i = i->m_nextComponent)
		{
			if (i->m_enabled)
				i->OnEvent(_event, _arg);
		}
	}
}
//----------------------------------------------------------------------------//
void Entity::BroadcastEvent(int _event, void* _arg)
{
	if (IsEnabled())
	{
		SendEvent(_event, _arg);

		for (Entity* i = m_child; i; i = i->m_next)
		{
			if(i->m_enabled)
				i->BroadcastEvent(_event, _arg);
		}
	}
}
//----------------------------------------------------------------------------//
EntityPtr Entity::Clone(void)
{
	EntityPtr _clone = new Entity;

	_clone->SetTransform(GetTransform());
	_clone->m_name = m_name;
	// TODO: flags & layers

	for (Component* i = m_component; i; i = i->m_nextComponent)
	{
		Component* _dst = _clone->AddComponent(i->GetTypeName());
		_dst->Clone(i);
	}

	for (Entity* i = m_child; i; i = i->m_next)
	{
		EntityPtr _child = i->Clone();
		_child->SetParent(_clone);
	}

	_clone->SetScene(m_scene); //

	return _clone;
}
//----------------------------------------------------------------------------//
Json Entity::Serialize(void)
{
	Json _dst;

	_dst["Type"] = GetTypeName();
	_dst["Name"] = m_name;
	_dst["Address"] = (uintptr_t)(void*)this;
	_dst["Enabled"] = m_enabled;
	_dst["Position"] = m_pos;
	_dst["Scale"] = m_scale;
	_dst["Angle"] = m_angle * Rad2Deg;
	//TODO: save name and local ID

	{
		Json& _components = _dst["Components"];
		for (Component* i = m_component; i; i = i->m_nextComponent)
		{
			if (i->IsSerializable())
				_components.Push(i->Serialize());
		}
	}

	{
		Json& _children = _dst["Children"];
		for (Entity* i = m_child; i; i = i->m_next)
		{
			if (i->IsSerializable())
				_children.Push(i->Serialize());
		}
	}

	return _dst;
}
//----------------------------------------------------------------------------//
void Entity::Deserialize(const Json& _src, class ObjectSolver* _context)
{
	// TODO: cleanup

	m_name = _src["Name"];
	m_oldAddress = (void*)(uintptr_t)_src["Address"];
	Enable(_src["Enabled"]);
	SetPosition(_src["Position"]);
	SetScale(_src["Scale"]);
	SetRotation(_src["Angle"].AsFloat() * Deg2Rad);

	if (_context)
	{
		_context->AddObject(this, m_oldAddress);
	}

	{									 
		const Json& _components = _src["Components"];
		for (uint i = _components.Size(); i--;)
		{
			const Json& _desc = _components[i];
			Component* _component = AddComponent(_desc["Type"].AsString().c_str());
			if (_component)
				_component->Deserialize(_desc, _context);
		}
	}

	{
		const Json& _children = _src["Children"];
		for (uint i = 0; i < _children.Size(); ++i)
		{
			const Json& _desc = _children[i];
			//Entity* _child = AddChild(_desc["Type"].AsString().c_str());
			Entity* _child = AddChild();
			if (_child)
				_child->Deserialize(_desc, _context);
		}
	}
}
//----------------------------------------------------------------------------//
void Entity::SolveObjects(ObjectSolver* _context)
{
	// Do nothing
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
void Scene::_Update(void)
{
	for (auto i : m_systemOrder)
		i->_Update();
}
//----------------------------------------------------------------------------//
void Scene::_PostUpdate(void)
{
	for (auto i : m_systemOrder)
		i->_PostUpdate();
}
//----------------------------------------------------------------------------//
void Scene::_Render(void)
{
	for (auto i : m_systemOrder)
		i->_Render();
}
//----------------------------------------------------------------------------//
void Scene::_PostRender(void)
{
	for (auto i : m_systemOrder)
		i->_PostRender();
}
//----------------------------------------------------------------------------//
void Scene::_DebugDraw(void)
{
	for (auto i : m_systemOrder)
		i->_DebugDraw();
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
// SceneManager
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
SceneManager::SceneManager(void)
{

}
//----------------------------------------------------------------------------//
SceneManager::~SceneManager(void)
{

}
//----------------------------------------------------------------------------//
bool SceneManager::OnEvent(int _type, void* _arg)
{
	switch (_type)
	{
	case SystemEvent::Update:
		if (m_currentScene)
			m_currentScene->_Update();
		break;

	case SystemEvent::PostUpdate:
		if (m_currentScene)
			m_currentScene->_PostUpdate();
		break;

	case SystemEvent::Render:
		if (m_currentScene)
			m_currentScene->_Render();
		break;

	case SystemEvent::PostRender:
		if (m_currentScene)
			m_currentScene->_PostRender();
		break;

	case SystemEvent::DebugDraw:
		if (m_currentScene)
			m_currentScene->_DebugDraw();
		break;

	}
	return false;
}
//----------------------------------------------------------------------------//
void SceneManager::SetCurrentScene(Scene* _scene)
{
	if (m_currentScene)
	{
		//TODO: pause
	}

	m_currentScene = _scene;

	if (m_currentScene)
	{
		//TODO: activate
	}
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
