workspace "InkStone"
	architecture "x64"
	startproject "Sandbox"

	configurations{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.architecture}/%{cfg.buildcfg}"

include "InkStone/include/GLAD"
include "InkStone/include/imgui"

project "InkStone"
	location "InkStone"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "%{prj.name}/pch.cpp"

	files{
		"%{prj.name}/%{prj.name}.h",
		"%{prj.name}/pch.h",
		"%{prj.name}/pch.cpp",
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/include/stb_image/**.h",
		"%{prj.name}/include/stb_image/**.cpp"
	}

	includedirs{
		"%{prj.name}",
		"%{prj.name}/include/EnTT/include",
		"%{prj.name}/include/GLAD/include",
		"%{prj.name}/include/GLFW/include",
		"%{prj.name}/include/imgui/",
		"%{prj.name}/include/stb_image/"
	}
	
	syslibdirs{
		"%{prj.name}/include/GLFW/lib"
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

project "InkStoneEditor"
	location "InkStoneEditor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"InkStone"
	}

	links{
		"InkStone"
	}

	filter "system:windows"
		systemversion "latest"

		defines{
			"NXTN_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "NXTN_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines "NXTN_RELEASE"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs{
		"InkStone"
	}

	links{
		"InkStone"
	}

	filter "system:windows"
		systemversion "latest"

		defines{
			"NXTN_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "NXTN_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines "NXTN_RELEASE"
		optimize "on"
		