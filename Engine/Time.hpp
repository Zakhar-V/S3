#pragma once

#include "System.hpp"

//----------------------------------------------------------------------------//
// Time
//----------------------------------------------------------------------------//

#define gTime Time::Instance

class Time : public Module<Time>
{
public:
	//!
	bool OnEvent(int _type, void* _arg) override;

	//!
	float Delta(void) { return m_deltaTime; }

protected:
	double m_prevTime = 0;
	float m_deltaTime = 0;
};

//----------------------------------------------------------------------------//
//
//----------------------------------------------------------------------------//
