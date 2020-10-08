workspace "rofv"
	architecture "x64"
	startproject "rofv"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Projects
group "Dependencies"
	include "vendor/GLFW"
	include "vendor/Glad"
	include "vendor/imgui"
group ""

project "rofv"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"examples/flower.cpp",
		"src/**.h",
		"src/**.cpp",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/baseGL/src/*.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"src",
		"vendor/spdlog/include",
		"vendor/GLFW/include",
		"vendor/Glad/include",
		"vendor/imgui",
		"vendor/glm",
		"vendor/stb_image",
		"vendor/baseGL/src"
	}

	links 
	{ 
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ROFV_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE"
		}

	filter "configurations:Debug"
		defines "ROFV_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ROFV_RELEASE"
		runtime "Release"
		optimize "on"
