#include "pch.h"

#include "Log.h"

namespace NXTN {
	std::mutex Log::mtx;
	time_t Log::tt = time(0);
	tm Log::ltm;

	void Log::Info(const std::string& msg)
	{
		localtime_s(&ltm, &tt);

		mtx.lock();
		printf("%02d:%02d:%02d [Info]  ", ltm.tm_hour, ltm.tm_min, ltm.tm_sec);
		printf(msg.c_str());
		printf("\n");
		mtx.unlock();
	}
}