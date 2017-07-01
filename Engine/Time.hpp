#pragma once

#include "Base.hpp"

//----------------------------------------------------------------------------//
// Time
//----------------------------------------------------------------------------//

#define gTime Time::Instance

class Time : public Singleton<Time>
{
public:
	//!
	void Update(void);

	//!
	float Delta(void) { return m_deltaTime; }

protected:
	double m_prevTime = 0;
	float m_deltaTime = 0;
};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
