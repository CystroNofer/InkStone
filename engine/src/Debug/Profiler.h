#pragma once

#include "pch.h"
#include <thread>

#ifdef NXTN_PROFILE
#define NXTN_PROFILER_BEGIN_SESSION(filepath) ::NXTN::Instrumentor::Get().BeginSession(filepath);
#define NXTN_PROFILER_END_SESSION() ::NXTN::Instrumentor::Get().EndSession();
#define NXTN_PROFILE_NEXT(x) ::NXTN::Instrumentor::Get().RecordNext(x);
#define NXTN_PROFILE_JOIN_IMPL(left, right) left##right
#define NXTN_PROFILE_JOIN(left, right) NXTN_PROFILE_JOIN_IMPL(left, right)
#define NXTN_PROFILE_SCOPE(name) ::NXTN::Profiler NXTN_PROFILE_JOIN(nxtnProfiler_, __LINE__)(name);
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
			std::lock_guard<std::mutex> lock(m_Mutex);
			BeginSessionLocked(filepath);
		}

		void EndSession()
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			EndSessionLocked();
		}

		void RecordNext(int maxResult, const std::string& filepath = "results.json")
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			BeginSessionLocked(filepath);
			m_MaxResult = maxResult;
		}

		void WriteResult(const std::string& name, long long start, long long end, size_t threadID)
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			if (!m_OutputStream.is_open())
			{
				return;
			}

			if (m_ResultCount > 0)
			{
				m_OutputStream << ",\n";
			}

			m_OutputStream << "    {\n";
			m_OutputStream << "      \"cat\":\"function\",\n";
			m_OutputStream << "      \"dur\":" << (end - start) << ",\n";
			m_OutputStream << "      \"name\":\"";
			for (char character : name)
			{
				switch (character)
				{
				case '"': m_OutputStream << "\\\""; break;
				case '\\': m_OutputStream << "\\\\"; break;
				case '\b': m_OutputStream << "\\b"; break;
				case '\f': m_OutputStream << "\\f"; break;
				case '\n': m_OutputStream << "\\n"; break;
				case '\r': m_OutputStream << "\\r"; break;
				case '\t': m_OutputStream << "\\t"; break;
				default:
					if (static_cast<unsigned char>(character) < 0x20)
					{
						constexpr char hexDigits[] = "0123456789abcdef";
						m_OutputStream << "\\u00"
							<< hexDigits[static_cast<unsigned char>(character) >> 4]
							<< hexDigits[static_cast<unsigned char>(character) & 0x0f];
					}
					else
					{
						m_OutputStream << character;
					}
					break;
				}
			}
			m_OutputStream << "\",\n";
			m_OutputStream << "      \"ph\":\"X\",\n";
			m_OutputStream << "      \"pid\":0,\n";
			m_OutputStream << "      \"tid\":" << threadID << ",\n";
			m_OutputStream << "      \"ts\":" << start << '\n';
			m_OutputStream << "    }";

			++m_ResultCount;
			if (m_MaxResult > 0 && m_ResultCount >= m_MaxResult)
			{
				EndSessionLocked();
			}
			else
			{
				m_OutputStream.flush();
			}
		}

		// Static getter s.t. this .h file does not need a .cpp file
		static Instrumentor& Get()
		{
			static Instrumentor s_Instrumentor;
			return s_Instrumentor;
		}

		~Instrumentor()
		{
			EndSession();
		}

	private:
		Instrumentor() : m_ResultCount(0), m_MaxResult(-1) {}

		void BeginSessionLocked(const std::string& filepath)
		{
			EndSessionLocked();
			m_OutputStream.clear();
			m_OutputStream.open(filepath, std::ios::out | std::ios::trunc);
			if (m_OutputStream.is_open())
			{
				m_OutputStream << "{\n  \"otherData\": {},\"traceEvents\":[\n";
				m_OutputStream.flush();
			}
		}

		void EndSessionLocked()
		{
			if (m_OutputStream.is_open())
			{
				m_OutputStream << "\n  ]\n}";
				m_OutputStream.close();
			}
			m_ResultCount = 0;
			m_MaxResult = -1;
		}

		std::mutex m_Mutex;
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
