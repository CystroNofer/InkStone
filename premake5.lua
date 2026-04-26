workspace "InkStone"
	architecture "x64"
	startproject "engine"

	configurations{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.architecture}/%{cfg.buildcfg}"

include "external/GLAD"
include "external/imgui"

project "engine"
	location "engine"
	-- kind "StaticLib"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "%{prj.name}/src/pch.cpp"

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"external/stb_image/**.h",
		"external/stb_image/**.cpp"
	}

	includedirs{
		"%{prj.name}/src",
		"external/GLAD/include",
		"external/GLFW/include",
		"external/imgui/",
		"external/stb_image/"
	}
	
	syslibdirs{
		"external/GLFW/lib"
	}

	links{
		"opengl32.lib",
		"glfw3_mt.lib",
		"GLAD",
		"ImGUI"
	}

	filter "system:windows"
		systemversion "latest"

		defines{
			"NXTN_PLATFORM_WINDOWS",
			"NXTN_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "NXTN_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "NXTN_RELEASE"
		runtime "Release"
		optimize "on"

-- project "InkStoneEditor"
-- 	location "InkStoneEditor"
-- 	kind "ConsoleApp"
-- 	language "C++"
-- 	cppdialect "C++20"
-- 	staticruntime "on"
	
-- 	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
-- 	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

-- 	files{
-- 		"%{prj.name}/src/**.h",
-- 		"%{prj.name}/src/**.cpp"
-- 	}

-- 	includedirs{
-- 		"InkStone"
-- 	}

-- 	links{
-- 		"InkStone"
-- 	}

-- 	filter "system:windows"
-- 		systemversion "latest"

-- 		defines{
-- 			"NXTN_PLATFORM_WINDOWS"
-- 		}

-- 	filter "configurations:Debug"
-- 		defines "NXTN_DEBUG"
-- 		symbols "on"

-- 	filter "configurations:Release"
-- 		defines "NXTN_RELEASE"
-- 		optimize "on"

-- project "Sandbox"
-- 	location "Sandbox"
-- 	kind "ConsoleApp"
-- 	language "C++"
-- 	cppdialect "C++20"
-- 	staticruntime "on"
	
-- 	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
-- 	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

-- 	files{
-- 		"%{prj.name}/src/**.h",
-- 		"%{prj.name}/src/**.cpp"
-- 	}

-- 	includedirs{
-- 		"InkStone"
-- 	}

-- 	links{
-- 		"InkStone"
-- 	}

-- 	filter "system:windows"
-- 		systemversion "latest"

-- 		defines{
-- 			"NXTN_PLATFORM_WINDOWS"
-- 		}

-- 	filter "configurations:Debug"
-- 		defines "NXTN_DEBUG"
-- 		symbols "on"

-- 	filter "configurations:Release"
-- 		defines "NXTN_RELEASE"
-- 		optimize "on"
		