#include "pch.h"

#include "Log.h"

namespace NXTN {
	std::mutex Log::mtx;
	time_t Log::tt = std::time(nullptr);
	tm Log::ltm;

	void Log::Info(const std::string& msg)
	{
		Log::tt = std::time(nullptr);
		localtime_s(&ltm, &tt);

		mtx.lock();
		printf("%02d:%02d:%02d [Info]  ", ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
		printf(msg.c_str());
		printf("\n");
		mtx.unlock();
	}
}