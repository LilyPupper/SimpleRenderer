workspace "SimpleRenderer"
	architecture "x64"
	startproject "SimpleRenderer"
	configurations { "Debug", "Release" }

project "SimpleRenderer"
	location "SimpleRenderer"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	systemversion "latest"

	targetdir "bin/%{cfg.buildcfg}/%{cfg.platform}"
	objdir "bin/intermediate/%{cfg.buildcfg}/%{cfg.platform}"

	files
	{
		"%{prj.name}/include/**.h",
		"%{prj.name}/src/**.cpp"
	}

	removefiles
	{
		"%{prj.name}/vendor/**"
	}

	includedirs
	{
		"%{prj.name}/include",
		"%{prj.name}/vendor/glm/glm"
	}

	postbuildcommands
	{
		"{MKDIR} %{wks.location}bin/%{cfg.buildcfg}/%{cfg.platform}/models",
		"{COPYFILE} %{wks.location}models/*.obj %{wks.location}bin/%{cfg.buildcfg}/%{cfg.platform}/models"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		runtime "Debug"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		runtime "Release"