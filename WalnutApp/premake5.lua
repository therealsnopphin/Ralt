project "WalnutApp"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "bin/%{cfg.buildcfg}"
   staticruntime "off"

   files { "**.h", "**.cpp" }

   pchheader "pch.h"
   pchsource "pch.cpp"

   includedirs
   {
	"",
	"../vendor/freetype/include",
      "../vendor/fmod/",
      "../vendor/imgui",
      "../vendor/imgui/misc/cpp",
      "../vendor/glfw/include",

      "../Walnut/src",

      "%{IncludeDir.VulkanSDK}",
      "%{IncludeDir.glm}",
   }
	libdirs 
{ 
"../vendor/freetype/lib/x64/v141/static/Release", "../vendor/fmod"
 }


    links
    {
        "freetype28", "Walnut", "fmodL_vc", "ws2_32"
    }

   targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
   objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")

   filter "system:windows"
      systemversion "latest"
      defines { "WL_PLATFORM_WINDOWS" }

   filter "configurations:Debug"
      defines { "WL_DEBUG" }
      runtime "Debug"
      symbols "On"

      filter "toolset:msc*"
         buildoptions { "/openmp" } -- MSVC OpenMP flag
      filter "toolset:gcc or toolset:clang"
         buildoptions { "-fopenmp" }
         linkoptions { "-fopenmp" }
      filter {}

   filter "configurations:Release"
      defines { "WL_RELEASE" }
      runtime "Release"
      optimize "On"
      symbols "On"

   filter "configurations:Dist"
      kind "WindowedApp"
      defines { "WL_DIST" }
      runtime "Release"
      optimize "On"
      symbols "Off"