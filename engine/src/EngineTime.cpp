#include "pch.h"

#include "EngineTime.h"

namespace NXTN {
	std::chrono::time_point<std::chrono::steady_clock> Time::m_LastTimePoint;
	float Time::m_DeltaTime = 0.0f;

	void Time::InitTime()
	{
		m_LastTimePoint = std::chrono::steady_clock::now();
	}

	void Time::Update()
	{
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		m_DeltaTime = (now - m_LastTimePoint).count() / 1e9f;
		m_LastTimePoint = now;
	}
}
