#include "pch.h"

#include "Time.h"

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
		m_DeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_LastTimePoint).count() / 1000.0f;
		m_LastTimePoint = now;
	}
}
