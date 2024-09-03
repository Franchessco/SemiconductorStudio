workspace "JunctionFitMaster"
    srunt = "off"

    configurations { "Debug", "Release" }
    platforms { "x86", "x86_64" }
    language "C++"
    cppdialect "C++latest"
    staticruntime(srunt)
    location "./"

    startproject "JunctionFitMaster"
    
group "JunctionFitMaster"
    include "./JFMServices"
    include "./JFMApp"
    include "./JunctionFitMaster"

group "Dependencies"
    include "./Vendor/LambertW"
    include "./Vendor/imgui"
    include "./Vendor/implot"
    include "./Vendor/yaml-cpp"




