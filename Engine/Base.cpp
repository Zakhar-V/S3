#include "Base.hpp"

//----------------------------------------------------------------------------//
// StringUtils
//----------------------------------------------------------------------------//

const String StringUtils::EmptyString;

//----------------------------------------------------------------------------//
uint StringUtils::Hash(const char* _str, uint _hash)
{
	if (!_str)
		_str = "";
	while (*_str)
		_hash = ((_hash >> 1) + ((_hash & 1) << 15) + Lower(*_str++)) & 0xffff;
	return _hash;
}
//----------------------------------------------------------------------------//
String StringUtils::Format(const char* _fmt, ...)
{
	va_list _args;
	va_start(_args, _fmt);
	String _str = FormatV(_fmt, _args);
	va_end(_args);
	return _str;
}
//----------------------------------------------------------------------------//
String StringUtils::FormatV(const char* _fmt, va_list _args)
{
	// TODO: use another implementation
	char _buffer[4096];
	vsnprintf(_buffer, sizeof(_buffer), _fmt, _args);
	return _buffer;
}
//----------------------------------------------------------------------------//
int StringUtils::Cmpi(const char* _str1, const char* _str2)
{
	return stricmp(_str1, _str2);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// RefCounted
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
RefCounted::RefCounted(void)
	: m_rc(new Counter)
{
	m_rc->object = this;
}
//----------------------------------------------------------------------------//
RefCounted::~RefCounted(void)
{
	ASSERT(m_rc == nullptr);
}
//----------------------------------------------------------------------------//
void RefCounted::AddRef(void)
{
	m_rc->ref++;
}
//----------------------------------------------------------------------------//
void RefCounted::Release(void)
{
	if (!--m_rc->ref)
		_DeleteThis();
}
//----------------------------------------------------------------------------//
void RefCounted::_DeleteThis(void)
{
	m_rc->object = nullptr;
	m_rc->Release();
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Object
//----------------------------------------------------------------------------//

HashMap<uint, Object::TypeInfo> Object::s_types;

//----------------------------------------------------------------------------//
Object::TypeInfo* Object::GetOrCreateTypeInfo(const char* _name)
{
	uint _type = StringUtils::Hash(_name);
	auto _iter = s_types.find(_type);
	if (_iter != s_types.end())
		return &_iter->second;

	LOG("Register %s(0x%04x) typeinfo", _name, _type);

	auto& _typeInfo = s_types[_type];
	_typeInfo.type = _type;
	_typeInfo.name = _name;

	return &_typeInfo;
}
//----------------------------------------------------------------------------//
Object::TypeInfo* Object::GetTypeInfo(uint _type)
{
	auto _iter = s_types.find(_type);
	if (_iter != s_types.end())
		return &_iter->second;
	return nullptr;
}
//----------------------------------------------------------------------------//
ObjectPtr Object::Create(const char* _name)
{
	TypeInfo* _typeinfo = GetTypeInfo(_name);
	if (_typeinfo && _typeinfo->Factory)
		return _typeinfo->Factory();

	LOG("Error: Factory for %s not found", _name);
	return nullptr;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
