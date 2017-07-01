#pragma once

//----------------------------------------------------------------------------//
// Includes
//----------------------------------------------------------------------------//

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS

#include <stdint.h>
#include <stdarg.h>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <algorithm>

#include <engine\easy.h>
//using namespace arctic;
//using namespace arctic::easy;

//----------------------------------------------------------------------------//
// Debug
//----------------------------------------------------------------------------//

#ifdef _DEBUG
#include <cassert>
#define ASSERT(...) assert(##__VA_ARGS__)
#else
#define ASSERT(...)
#endif

#define LOG(msg, ...) {printf(msg, ##__VA_ARGS__); printf("\n");}

//----------------------------------------------------------------------------//
// Typedefs
//----------------------------------------------------------------------------//

typedef unsigned int uint;
typedef int8_t int8;
typedef uint8_t uint8;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64;
typedef uint64_t uint64;

typedef std::string String;
template <class T> using Array = std::vector<T>;
template <class T> using List = std::list<T>;
template <class T, class U> using HashMap = std::unordered_map<T, U>;
template <class T, class U> using Pair = std::pair<T, U>;
template <class T> using InitializerList = std::initializer_list<T>;

//----------------------------------------------------------------------------//
// Linked list
//----------------------------------------------------------------------------//

#define LL_LINK(HEAD, NODE, PREV, NEXT) {\
	NODE->NEXT = HEAD; \
	if (HEAD) \
		HEAD->PREV = NODE; \
	HEAD = NODE; }

#define LL_UNLINK(HEAD, NODE, PREV, NEXT) {\
		if (NODE->PREV) \
			NODE->PREV->NEXT = NODE->NEXT; \
		else \
		{ \
			ASSERT(NODE == HEAD); \
			HEAD = NODE->NEXT; \
		} \
		if (NODE->NEXT) \
			NODE->NEXT->PREV = NODE->PREV; \
		NODE->PREV = nullptr; \
		NODE->NEXT = nullptr; }

//----------------------------------------------------------------------------//
// StringUtils
//----------------------------------------------------------------------------//

struct StringUtils
{
	//!
	static constexpr bool IsAlpha(char _ch) { return (_ch >= 'A' && _ch <= 'Z') || (_ch >= 'a' && _ch <= 'z') || ((uint8)_ch >= 0xc0); }
	//!
	static constexpr char Lower(char _ch) { return IsAlpha(_ch) ? (_ch | 0x20) : _ch; }
	//!
	static constexpr char Upper(char _ch) { return IsAlpha(_ch) ? (_ch & ~0x20) : _ch; }

	//!\return BSD checksum
	static constexpr uint ConstHash(const char* _str, uint _hash = 0) { return *_str ? ConstHash(_str + 1, ((_hash >> 1) + ((_hash & 1) << 15) + Lower(*_str)) & 0xffff) : _hash; }
	//!
	static uint Hash(const char* _str, uint _hash = 0);

	//!
	static String Format(const char* _fmt, ...);
	//!
	static String FormatV(const char* _fmt, va_list _args);

	//!
	static int Cmpi(const char* _str1, const char* _str2);

	//!
	static const String EmptyString;
};

//----------------------------------------------------------------------------//
// NonCopyable
//----------------------------------------------------------------------------//

class NonCopyable
{
public:
	NonCopyable(void) = default;
	~NonCopyable(void) = default;

private:
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator = (const NonCopyable&) = delete;
};

//----------------------------------------------------------------------------//
// Singleton
//----------------------------------------------------------------------------//

//!
template <class T> class Singleton
{
public:
	//!
	Singleton(void)
	{
		ASSERT(s_instance == nullptr);
		s_instance = static_cast<T*>(this);
	}
	//!
	~Singleton(void)
	{
		s_instance = nullptr;
	}

	//!
	static T* Get(void)
	{
		return s_instance;
	}

	static T* const& Instance;

protected:
	static T* s_instance;
};

template <class T> T* Singleton<T>::s_instance = nullptr;
template <class T> T* const& Singleton<T>::Instance = Singleton<T>::s_instance;

//----------------------------------------------------------------------------//
// RefCounted
//----------------------------------------------------------------------------//

//!
class RefCounted
{
public:
	//!
	struct Counter
	{
		RefCounted* object;
		int ref = 0;
		int weak = 1;

		//! Increments the counter of weak references
		void AddRef(void)
		{
			++weak;
		}
		//! Decrements the counter of weak references
		void Release(void)
		{
			if (!--weak && !ref)
				delete this;
		}
	};

	//!
	RefCounted(void);
	//!
	virtual ~RefCounted(void);

	//! Increments the counter of strong references
	void AddRef(void);
	//! Decrements the counter of strong references
	void Release(void);

protected:
	//! Delete this object. You can overload this function for another behavior on deletion.
	virtual void _DeleteThis(void);

	Counter* m_rc;
};

//!
template <class T> static void AddRef(T* _ptr)
{
	if (_ptr)
		_ptr->AddRef();
}
//!
template <class T> static void Release(T* _ptr)
{
	if (_ptr)
		_ptr->Release();
}
//!
template <class T, class U> static void Assign(T*& _lhs, U* _rhs)
{
	AddRef(_rhs);
	Release(_lhs);
	_lhs = static_cast<T*>(_rhs);
}

//----------------------------------------------------------------------------//
// SharedPtr
//----------------------------------------------------------------------------//

template <class T> class SharedPtr
{
public:
	//!
	SharedPtr(void) = default;
	//!
	SharedPtr(const SharedPtr& _ptr) :
		SharedPtr(_ptr.m_ptr)
	{
	}
	//!
	SharedPtr(SharedPtr&& _ptr) :
		m_ptr(_ptr.m_ptr)
	{
		_ptr.m_ptr = nullptr;
	}
	//!
	SharedPtr(const T* _ptr) :
		m_ptr(const_cast<T*>(_ptr))
	{
		AddRef(m_ptr);
	}
	//!
	~SharedPtr(void)
	{
		Release(m_ptr);
	}

	//!
	SharedPtr& operator = (const SharedPtr& _rhs)
	{
		Assign(m_ptr, _rhs.m_ptr);
		return *this;
	}
	//!
	SharedPtr& operator = (SharedPtr&& _rhs)
	{
		std::swap(m_ptr, _rhs.m_ptr);
		return *this;
	}
	//!
	SharedPtr& operator = (const T* _rhs)
	{
		Assign(m_ptr, const_cast<T*>(_rhs));
		return *this;
	}

	//!	Implicit cast
	operator T* (void) const
	{
		return Get();
	}

	//!
	T& operator * (void) const
	{
		ASSERT(m_ptr != nullptr);
		return *Get();
	}
	//!
	T* operator -> (void) const
	{
		ASSERT(m_ptr != nullptr);
		return Get();
	}

	//!
	T* Get(void) const
	{
		return const_cast<T*>(m_ptr);
	}
	//!
	template <class X> X* Cast(void) const
	{
		return static_cast<X*>(const_cast<T*>(m_ptr));
	}

private:
	T* m_ptr = nullptr;
};

//----------------------------------------------------------------------------//
// Object
//----------------------------------------------------------------------------//

// !
#define RTTI(TYPE) \
	enum : uint { TypeID = StringUtils::ConstHash(TYPE) }; \
	uint GetTypeID(void) override { return TypeID; } \
	bool IsTypeOf(uint _type) override { return _type == TypeID || __super::IsTypeOf(_type); } \
	template <class T> bool IsTypeOf(void) { return IsTypeOf(T::TypeID); } \
	static constexpr const char* TypeName = TYPE; \
	const char* GetTypeName(void) override { return TypeName; }

// !
typedef SharedPtr<class Object> ObjectPtr;

// !
class Object : public RefCounted
{
public:
	// !
	enum : uint { TypeID = StringUtils::ConstHash("Object") };
	// !
	virtual uint GetTypeID(void) { return TypeID; }
	// !
	virtual bool IsTypeOf(uint _type) { return _type == TypeID; }
	// !
	template <class T> bool IsTypeOf(void) { return IsTypeOf(T::TypeID); }
	// !
	static constexpr const char* TypeName = "Object";
	// !
	virtual const char* GetTypeName(void) { return TypeName; }

	// !
	typedef SharedPtr<Object>(*FactoryPfn)(void);
	// !
	template <class T> static SharedPtr<Object> Factory() { return new T; }

	// !
	struct TypeInfo
	{
		uint type;
		const char* name;
		FactoryPfn Factory = nullptr;
		uint flags = 0; //!< type-specific flags

		TypeInfo* SetFactory(FactoryPfn _factory) { Factory = _factory; return this; }
		TypeInfo* SetFlags(uint _flags) { flags = _flags; return this; }
		TypeInfo* AddFlags(uint _flags) { flags |= _flags; return this; }
		bool HasAnyOfFlags(uint _flags) { return (flags & _flags) != 0; }
	};

	//!
	static TypeInfo* GetOrCreateTypeInfo(const char* _name);
	//!
	static TypeInfo* GetTypeInfo(uint _type);
	//!
	static TypeInfo* GetTypeInfo(const char* _name) { return GetTypeInfo(StringUtils::Hash(_name)); }
	//!
	template <class T> static TypeInfo* GetOrCreateTypeInfo(void) { return GetOrCreateTypeInfo(T::TypeName); }
	//!
	static ObjectPtr Create(const char* _name);
	//!
	template <class T> static SharedPtr<T> Create(void) { return Create(T::TypeName).Cast<T>(); }
	//!
	template <class T> static TypeInfo* Register(uint _flags = 0)
	{
		TypeInfo* _info = GetOrCreateTypeInfo<T>();
		_info->Factory = &Object::Factory<T>;
		_info->AddFlags(_flags);
		return _info;
	}

private:
	static HashMap<uint, TypeInfo> s_types;
};

//----------------------------------------------------------------------------//
// 
//----------------------------------------------------------------------------//

