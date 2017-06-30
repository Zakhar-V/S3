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

#include <engine\easy.h>
using namespace arctic;
using namespace arctic::easy;

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
// 
//----------------------------------------------------------------------------//

