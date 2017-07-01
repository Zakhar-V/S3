#pragma once

#include "Base.hpp"

typedef SharedPtr<class Component> ComponentPtr;
typedef SharedPtr<class Entity> EntityPtr;
typedef SharedPtr<class SceneSystem> SceneSystemPtr;
typedef SharedPtr<class Scene> ScenePtr;

//----------------------------------------------------------------------------//
// Component
//----------------------------------------------------------------------------//

class Component abstract : public Object
{
public:
	RTTI("Component");

	//!
	enum class Family
	{
		Render,
		Physics,
		Audio,
		Logic,
		__Count,
	};
	//!
	virtual Family GetFamily(void) = 0;

	//!
	Entity* GetEntity(void) { return m_entity; }
	//!
	Scene* GetScene(void);

	//!
	Component* GetNextComponent(void) { return m_nextComponent; }

	//!
	static void Destroy(Component* _component);

protected:
	friend class Entity;

	Entity* m_entity = nullptr;
	Component* m_prevComponent = nullptr;
	Component* m_nextComponent = nullptr;
};

//----------------------------------------------------------------------------//
// Entity
//----------------------------------------------------------------------------//

class Entity : public Object
{
public:
	RTTI("Entity");

	//!
	Component* AddComponent(const char* _typename);
	//!
	template <class T> T* AddComponent(void) { return static_cast<T*>(AddComponent(T::TypeName)); }
	//!
	void RemoveComponent(Component* _component);

	//!
	Component* GetFirstComponent(void) { return m_component; }
	//!
	Component* GetComponent(uint _type);
	//!
	template <class T> T* GetComponent(void) { retrun static_cast<T*>(GetComponent(T::TypeID)); }

	//!
	Scene* GetScene(void) { return m_scene; }
	//!
	void SetScene(Scene* _scene);

	//!
	Entity* GetParent(void) { return m_parent; }
	//!
	void SetParent(Entity* _parent);
	//!
	Entity* GetNextEntity(void) { return m_next; }

	//!
	static void Destroy(Entity* _entity);

protected:
	friend class Scene;

	Scene* m_scene = nullptr;

	Component* m_component = nullptr;

	Entity* m_parent = nullptr;
	Entity* m_prev = nullptr;
	Entity* m_next = nullptr;
	Entity* m_child = nullptr;

	//TODO: transform
};

//----------------------------------------------------------------------------//
// SceneSystem
//----------------------------------------------------------------------------//

#define LOGIC_PRIORITY 0
#define PHYSICS_PRIORITY -10
#define RENDER_PRIORITY -20
#define AUDIO_PRIORITY -30

class SceneSystem : public Object
{
public:
	RTTI("SceneSystem");

	struct Definition
	{
		uint id;
		const char* name;
		int priority;
	};

	//!
	static void AddDefaultSystem(const char* _name, int _priority);
	//!
	static const Array<Definition>& DefaultSystems(void) { return s_defaultSystems; }
	//!
	static int Priority(uint _type);

protected:
	friend class Scene;
	//!
	virtual void _Start(Scene* _scene) { m_scene = _scene; }
	//!
	virtual void _Stop(void) { m_scene = nullptr; }
	//!
	virtual void _AddComponent(Component* _component) { }
	//!
	virtual void _RemoveComponent(Component* _component) { }

	//!
	virtual void _ProcessFrame(void) { }

	Scene* m_scene = nullptr;
	int m_priority = 0;

	static Array<Definition> s_defaultSystems;
};

//----------------------------------------------------------------------------//
// Scene
//----------------------------------------------------------------------------//

class Scene : public Object
{
public:
	RTTI("Scene");

	Scene(void);
	~Scene(void);

	//!
	SceneSystem* GetSystem(uint _type);
	//!
	template <class T> T* GetSystem(void) { return static_cast<T*>(GetSystem(T::TypeID)); }
	//!
	SceneSystem* GetOrAddSystem(const char* _name);
	//!
	template <class T> T* GetOrAddSystem(void) { return static_cast<T*>(GetSystem(T::TypeName)); }
	//
	void _SortSystems(void);

	//!
	void ProcessFrame(void);

	//!
	Entity* GetRoot(void) { return m_root; }

protected:
	friend class Entity;

	//!
	void _LinkRootEntity(Entity* _entity);
	//!
	void _UnlinkRootEntity(Entity* _entity);

	//!
	void _AddEntity(Entity* _entity);
	//!
	void _RemoveEntity(Entity* _entity);

	//!
	void _AddComponent(Component* _component);
	//!
	void _RemoveComponent(Component* _component);

	HashMap<uint, SceneSystemPtr> m_systems;
	Array<SceneSystem*> m_systemOrder;

	Entity* m_root = nullptr;
};

//----------------------------------------------------------------------------//
// 
//----------------------------------------------------------------------------//

