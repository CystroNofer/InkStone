#pragma once

#include "pch.h"

//#define NXTN_PROFILE
#include "Debug/Profiler.h"

// (__declspec)
#ifdef NXTN_PLATFORM_WINDOWS
	//#ifdef NXTN_BUILD_DLL
	//	#define NXTN_API __declspec(dllexport)
	//#else
	//	#define NXTN_API __declspec(dllimport)
	//#endif
#else
	#error Platform Not Supported
#endif // NXTN_PLATFORM_WINDOWS

// Error
#ifdef NXTN_DEBUG
	#define NXTN_ERROR __debugbreak();
#else
	#define NXTN_ERROR exit(-1);
#endif // NXTN_DEBUG

#define BITMASK(x) (1 << x)

namespace NXTN {
	enum class GraphicsAPI
	{
		None = 0, OpenGL
	};

	enum class WindowAPI
	{
		None = 0, GLFW
	};
}
