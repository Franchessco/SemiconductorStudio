project "JunctionFitMaster"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++latest"
    staticruntime(srunt)

    location("%{wks.location}/JunctionFitMaster")

    files {
        "%{prj.location}/**.cpp",
        "%{prj.location}/**.hpp"
    }


    includedirs {
        "%{prj.location}/**",

        "%{wks.location}/JFMServices",

        "%{wks.location}/Vendor/imgui",
        "%{wks.location}/Vendor/imgui/backends",
        "%{wks.location}/Vendor/imgui/misc/cpp",
        "%{wks.location}/Vendor/implot",
        "%{wks.location}/Vendor/imgui/examples/libs/glfw/include",
        "%{wks.location}/Vendor/yaml-cpp/include"
    }

    targetdir("%{wks.location}/bin/")
    objdir("%{wks.location}/obj/%{cfg.buildcfg}")

    libdirs("%{wks.location}/lib")

    filter { "configurations:Debug" }
        defines { "DEBUG" }
        runtime "Debug"

    filter { "configurations:Release" }
        runtime "Release"

    filter { "system:windows" }
        ignoredefaultlibraries { "msvcrt" }
        -- links { "JFMApp", "JFMServices" }
        links {  "JFMServices" }


    