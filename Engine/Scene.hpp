#pragma once

#include "System.hpp"
#include "Object.hpp"
#include "Math.hpp"
#include "Json.hpp"

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

	enum
	{
		Single = 0x1,
	};

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
	template <class T> T* GetNextComponent(void)
	{
		for (Component* i = m_nextComponent; i; i = i->m_nextComponent)
		{
			if (i->IsTypeOf<T>())
				return static_cast<T*>(i);
		}
		return nullptr;
	}

	//!
	static void Destroy(Component* _component);

	//!
	bool IsEnabled(void);
	//!
	void Enable(bool _enable);

	//!
	virtual void OnEvent(int _event, void* _arg = nullptr) { }

	//!
	bool IsSerializable(void) override { return true; }
	//!
	Json Serialize(void) override;
	//!
	void Deserialize(const Json& _src, class ObjectSolver* _context = nullptr) override;
	//!
	void SolveObjects(ObjectSolver* _context) override;

protected:
	friend class Entity;

	//!
	virtual void _Bind(void) { }
	//!
	virtual void _Unbind(void) { }
	//!
	virtual void _Enable(void) { }
	//!
	virtual void _Disable(void) { }

	//!
	void _OnEntityEnable(bool _enable);
	//!
	virtual void _OnEntityTransformChanged(void) { }
	//!
	virtual void _OnEntityTransformUpdated(void) { }
	//!
	virtual void _OnEntityParentChanged(void) { }

	Entity* m_entity = nullptr;
	Component* m_prevComponent = nullptr;
	Component* m_nextComponent = nullptr;

	bool m_enabled = true;
	bool m_entityEnabled = true;
	void* m_oldAddress = nullptr;
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
	template <class T> T* GetComponent(void) { return static_cast<T*>(GetComponent(T::TypeID)); }

	//!
	Scene* GetScene(void) { return m_scene; }
	//!
	void SetScene(Scene* _scene);

	//!
	Entity* GetParent(void) { return m_parent; }
	//!
	void SetParent(Entity* _parent, bool _keepWorldTransform = true);
	//!
	Entity* GetNextEntity(void) { return m_next; }
	//!
	Entity* GetChild(void) { return m_child; }
	//!
	Entity* AddChild(void);

	//!
	static void Destroy(Entity* _entity);

	//!
	bool IsEnabled(void);
	//!
	void Enable(bool _enable);

	//!
	int GetDepth(void) { return m_depth; }


	//! Add local rotation
	void Rotate(float _r);
	//! Set local rotation
	void SetRotation(float _r);
	//! Add local scale
	void Scale(float _s);
	//! Set local scale
	void SetScale(float _s);
	//! Add local position
	void Translate(const Vector2& _t);
	//! Set local position
	void SetPosition(const Vector2& _t);
	//! Get world transform
	const Transform& GetTransform(void);
	//! Set world transform
	void SetTransform(const Transform& _m);

	//!
	//virtual void OnEvent(int _event, Object* _sender = nullptr, void* _arg = nullptr) { }
	//!
	void SendEvent(int _event, void* _arg = nullptr);
	//!
	void BroadcastEvent(int _event, void* _arg = nullptr);

	//!
	bool IsSerializable(void) override { return true; }
	//!
	Json Serialize(void) override;
	//!
	void Deserialize(const Json& _src, class ObjectSolver* _context = nullptr) override;
	//!
	void SolveObjects(ObjectSolver* _context) override;

protected:
	friend class Scene;

	//!
	void _OnParentEnable(bool _enable);
	//!
	void _UpdateDepth(void);
	//!
	void _InvalidateTransform(void);
	//!
	void _UpdateTransform(void);

	Scene* m_scene = nullptr;

	Component* m_component = nullptr;

	Entity* m_parent = nullptr;
	Entity* m_prev = nullptr;
	Entity* m_next = nullptr;
	Entity* m_child = nullptr;

	bool m_enabled = true;
	bool m_parentEnabled = true;

	int m_depth = 0;

	bool m_transformUpdated = true;
	float m_scale = 1; //!< local scale
	float m_angle = 0; //!< local rotation
	Vector2 m_pos = { 0, 0 }; //!< local position
	Transform m_transform; //!< world transform

	void* m_oldAddress = nullptr;
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
	virtual void _Update(void) { }
	//!
	virtual void _PostUpdate(void) { }
	//!
	virtual void _Render(void) { }

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
	Entity* GetRoot(void) { return m_root; }

protected:
	friend class Entity;
	friend class SceneManager;

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

	//!
	void _Update(void);
	//!
	void _PostUpdate(void);
	//!
	void _Render(void);

	HashMap<uint, SceneSystemPtr> m_systems;
	Array<SceneSystem*> m_systemOrder;

	Entity* m_root = nullptr;
};

//----------------------------------------------------------------------------//
// SceneManager
//----------------------------------------------------------------------------//

#define gSceneManager SceneManager::Instance

class SceneManager : public Module<SceneManager>
{
public:
	//!
	SceneManager(void);
	//!
	~SceneManager(void);

	//!
	bool OnEvent(int _type, void* _arg) override;

	//!
	void SetCurrentScene(Scene* _scene);
	//!
	Scene* GetCurrentScene(void) { return m_currentScene; }

protected:
	ScenePtr m_currentScene;
};

//----------------------------------------------------------------------------//
// 
//----------------------------------------------------------------------------//

