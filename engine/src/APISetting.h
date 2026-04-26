#pragma once

#include "Core.h"

namespace NXTN {
	class APISetting
	{
	public:
		static inline GraphicsAPI GetGraphicsAPI() { return s_GraphicsAPI; }
		static inline WindowAPI   GetWindowAPI()   { return s_WindowAPI; }

	private:
		static GraphicsAPI s_GraphicsAPI;
		static WindowAPI   s_WindowAPI;
	};
}

