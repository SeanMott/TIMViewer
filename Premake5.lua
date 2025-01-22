---common vars we have to manually define
SDL_LINK_PATH_DEBUG = "Venders/SDL/Build/RelWithDebInfo/SDL2.lib"
SDL_LINK_PATH_RELEASE = "Venders/SDL/Build/Release/SDL2.lib"
SDL_LINK_PATH_DIST = "Venders/SDL/Build/MinSizeRel/SDL2.lib"

VULKAN_SDK_MANUAL_OVERRIDE = "C:/VulkanSDK/1.3.296.0/Include"

--searches for a Vulkan SDK on the system and loads it

--common includes everyone will use
FMT_INCLUDE = "Venders/FMT/include"
GLM_INCLUDE = "Venders/GLM"
SDL_INCLUDE = "Venders/SDL/include"
VK_BOOTSTRAP_INCLUDE = "Venders/VKBootstrap/src"
STB_INCLUDE = "Venders/STB"
VOLK_INCLUDE = "Venders/Volk"
VMA_INCLUDE = "Venders/VMA/include"

workspace "TIMToolkit"
architecture "x64"
startproject "TimViewer"

configurations
{
    "Debug",
    "Release",
    "Dist"
}

--include "BTDSTD3"
--include "TyGUI"
--include "Smok"
--include "SmokACT"

--TIM Ripper || rips a TIM Bundle into it's single TIMs
project "TimRipper"
location "TimRipper"
kind "ConsoleApp"
language "C++"
targetdir ("bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/")
objdir ("bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/")


files 
{
---base code
"TimRipper/includes/**.h",
"TimRipper/src/**.c",
"TimRipper/includes/**.hpp",
"TimRipper/src/**.cpp",

--volk
--"./Venders/Volk/volk.c",
--"./Venders/Volk/volk.h",

--Vk Bootstrapp
--"./Venders/VKBootstrap/src/VkBootstrap.cpp",
--"./Venders/VKBootstrap/src/VkBootstrap.h",
--"./Venders/VKBootstrap/src/VkBootstrapDispatch.h",

}

includedirs 
{
---base code
"TimRipper/includes",

--SDL_INCLUDE,

--GLM_INCLUDE,
FMT_INCLUDE,
--SDL_INCLUDE,

--VK_BOOTSTRAP_INCLUDE,
STB_INCLUDE,
--VOLK_INCLUDE,
--VMA_INCLUDE,
--VULKAN_SDK_MANUAL_OVERRIDE
}

defines
{
"VK_NO_PROTOTYPES",
"GLM_FORCE_DEPTH_ZERO_TO_ONE",
"GLM_FORCE_RADIANS",
"GLM_ENABLE_EXPERIMENTAL",
}


flags
{
"MultiProcessorCompile",
"NoRuntimeChecks",
}


buildoptions
{
    "/utf-8"
}


--platforms
filter "system:windows"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"


defines
{
"Window_Build",
"VK_USE_PLATFORM_WIN32_KHR",
"Desktop_Build",
}

filter "system:linux"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"


defines
{
"Linux_Build",
"VK_USE_PLATFORM_XLIB_KHR",
"Desktop_Build",
}


    filter "system:mac"
    cppdialect "C++17"
    staticruntime "On"
    systemversion "latest"


defines
{
"MacOS_Build",
"VK_USE_PLATFORM_MACOS_MVK",
"Desktop_Build",
}

--configs
filter "configurations:Debug"
    defines "BTD_DEBUG"
    symbols "On"

    links
    {
       -- SDL_LINK_PATH_DEBUG
    }

filter "configurations:Release"
    defines "BTD_RELEASE"
    optimize "On"

links
{
   -- SDL_LINK_PATH_RELEASE
}

filter "configurations:Dist"
    defines "BTD_DIST"
    optimize "On"

    links
    {
        --SDL_LINK_PATH_DIST
    }


defines
{
"NDEBUG",
}

flags
{
"LinkTimeOptimization",
}

--TIM Converter || converts a single TIM into a bmp

--Tim Viewer || Allows a user to open a TIM and save as bmp