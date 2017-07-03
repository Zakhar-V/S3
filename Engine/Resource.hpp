#pragma once

#include "Object.hpp"
#include "System.hpp"

typedef SharedPtr<class Resource> ResourcePtr;
#define gResources ResourceCache::Instance

//----------------------------------------------------------------------------//
// Resource
//----------------------------------------------------------------------------//

//!
class Resource abstract : public Object
{
public:
	RTTI("Resource");

	//!
	void SetName(const String& _name) { m_name = _name; }
	//!
	const String& GetName(void) { return m_name; }

	//!
	virtual void Load(const char* _filename) { }

protected:
	String m_name;
};

//----------------------------------------------------------------------------//
// ResourceCache
//----------------------------------------------------------------------------//

//!
class ResourceCache : public Module<ResourceCache>
{
public:
	//!
	ResourceCache(void);
	//!
	~ResourceCache(void);

	//!
	bool OnEvent(int _type, void* _arg) override;

	//!
	void SetDataPath(const String& _dir);
	//!
	const String& GetDataPath(void) { return m_dataPath; }
	//!
	String MakePath(const String& _name, const String& _ext);

	//!
	Resource* GetResource(const char* _type, const String& _name, uint _typeid = 0);
	//!
	template <class T> T* GetResource(const String& _name)
	{
		return static_cast<T*>(GetResource(T::TypeName, _name, T::TypeID));
	}

protected:

	String m_dataPath;
	HashMap<uint, HashMap<uint, ResourcePtr>> m_resources;
};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
