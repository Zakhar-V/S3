#include "Resource.hpp"

//----------------------------------------------------------------------------//
// ResourceCache
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
ResourceCache::ResourceCache(void)
{
}
//----------------------------------------------------------------------------//
ResourceCache::~ResourceCache(void)
{
	m_resources.clear();
}
//----------------------------------------------------------------------------//
void ResourceCache::SetDataPath(const String& _dir)
{
	m_dataPath = _dir;
	if (!m_dataPath.empty() && !strchr("\\/", m_dataPath.back()))
		m_dataPath += '/';

	LOG("Data location: \"%s\"", m_dataPath.c_str());
}
//----------------------------------------------------------------------------//
String ResourceCache::MakePath(const String& _name, const String& _ext)
{
	return m_dataPath + _name + "." + _ext;
}
//----------------------------------------------------------------------------//
Resource* ResourceCache::GetResource(const char* _type, const String& _name, uint _typeid)
{
	if (!_typeid)
		_typeid = StringUtils::Hash(_type);
	uint _id = StringUtils::Hash(_name.c_str());
	auto& _cache = m_resources[_typeid];

	auto& _exists = _cache.find(_id);
	if (_exists != _cache.end())
		return _exists->second;

	Object::TypeInfo* _typeinfo = Object::GetOrCreateTypeInfo(_type);
	if (!_typeinfo->Factory)
	{
		LOG("Error: Unable to create %s \"%s\"", _type, _name.c_str());
		return nullptr;
	}

	ResourcePtr _res = _typeinfo->Factory().Cast<Resource>();
	ASSERT(_res != nullptr);

	_cache[_id] = _res;

	_res->SetName(_name);
	_res->Load(_name.c_str());

	return _res;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
