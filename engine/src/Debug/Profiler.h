#pragma once

#include "pch.h"

#ifdef NXTN_PROFILE
#define NXTN_PROFILER_BEGIN_SESSION(filepath) Instrumentor::Get().BeginSession(filepath);
#define NXTN_PROFILER_END_SESSION() Instrumentor::Get().EndSession();
#define NXTN_PROFILE_NEXT(x) Instrumentor::Get().RecordNext(x);
#define NXTN_PROFILE_SCOPE(name) Profiler timer##__LINE__(name);
#define NXTN_PROFILE_FUNCTION() NXTN_PROFILE_SCOPE(__FUNCSIG__)
#else
#define NXTN_PROFILER_BEGIN_SESSION(filepath)
#define NXTN_PROFILER_END_SESSION()
#define NXTN_PROFILE_NEXT(x)
#define NXTN_PROFILE_SCOPE(name)
#define NXTN_PROFILE_FUNCTION()
#endif

namespace NXTN {
	class Instrumentor
	{
	public:
		void BeginSession(const std::string& filepath = "results.json")
		{
			m_ResultCount = 0;
			m_MaxResult = -1;

			m_OutputStream.open(filepath);
			m_OutputStream << "{\n  \"otherData\": {},\"traceEvents\":[\n";
			m_OutputStream.flush();
		}

		void EndSession()
		{
			if (!m_OutputStream.is_open())
			{
				return;
			}

			m_OutputStream << "\n  ]\n}";
			m_OutputStream.close();

			m_ResultCount = 0;
			m_MaxResult = -1;
		}

		void RecordNext(int maxResult, const std::string& filepath = "results.json")
		{
			BeginSession(filepath);

			m_MaxResult = maxResult;
		}

		void WriteResult(std::string name, long long start, long long end, size_t threadID)
		{
			if (!m_OutputStream.is_open())
			{
				return;
			}

			if (m_MaxResult > 0 && m_ResultCount >= m_MaxResult)
			{
				EndSession();
				return;
			}

			if (m_ResultCount > 0)
			{
				m_OutputStream << ",\n";
			}

			std::replace(name.begin(), name.end(), '"', '\'');

			m_OutputStream << "    {\n";
			m_OutputStream << "      \"cat\":\"function\",\n";
			m_OutputStream << "      \"dur\":" << (end - start) << ",\n";
			m_OutputStream << "      \"name\":\"" << name << "\",\n";
			m_OutputStream << "      \"ph\":\"X\",\n";
			m_OutputStream << "      \"pid\":0,\n";
			m_OutputStream << "      \"tid\":" << threadID << ",\n";
			m_OutputStream << "      \"ts\":" << start << '\n';
			m_OutputStream << "    }";

			m_OutputStream.flush();

			++m_ResultCount;
		}

		// Static getter s.t. this .h file does not need a .cpp file
		static Instrumentor& Get()
		{
			static Instrumentor s_Instrumentor;
			return s_Instrumentor;
		}

	private:
		Instrumentor() : m_ResultCount(0), m_MaxResult(-1) {}

		std::ofstream m_OutputStream;

		int m_ResultCount;
		int m_MaxResult;
	};

	class Profiler
	{
	public:
		Profiler(std::string name)
			: m_Name(name)
		{
			m_StartTime =
				std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
		}
		~Profiler()
		{
			Instrumentor::Get().WriteResult(
				m_Name,
				m_StartTime,
				std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()).time_since_epoch().count(),
				std::hash<std::thread::id>{}(std::this_thread::get_id())
			);
		}

	private:
		std::string m_Name;
		long long m_StartTime;
	};
}
