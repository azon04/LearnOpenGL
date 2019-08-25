-- premake5.lua

-- Generator for project I learnt from LearnOpenGL.com

-- some const
project_dir = "./Projects"

-- Actions
newaction {
    trigger = "clean",
    description = "clean up project files",
    execute = function()
        os.rmdir(project_dir)
        os.rmdir("./LearnOpenGL.sln")
    end
}

workspace "LearnOpenGL"
    location ("./")
    configurations { "Debug", "Release" }

-- BEGIN: Default value for all project under this group
function setup_project()
    defines { "WIN32" }

    targetdir "Binaries/%{cfg.buildcfg}"
    debugdir "./LearnOpenGL"

    includedirs {
        "Dependencies/assimp-3.3.1/include",
        "Dependencies/glew-2.0.0/include",
        "Dependencies/glfw/include",
        "Dependencies/soil/src",
        "Dependencies/includes",
        "LearnOpenGL/Common",
        "LearnOpenGL/"
    }

    filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    configuration "*" 
        links { "glew32", "glfw3", "zlibstatic", "assimp-vc140-mt", "opengl32" }
        libdirs {
            "./Dependencies/assimp-3.3.1/lib/Release/Win32",
            "./Dependencies/glew-2.0.0/lib/Release/Win32",
            "./Dependencies/glfw/lib-vc2015"
        }
        postbuildcommands {
            "{COPY} \"" .. path.getabsolute("./Dependencies/glew-2.0.0/bin/Release/Win32/*.DLL") .. "\" %{cfg.targetdir}"
        }
end
-- END

project "LearnOpenGL-Blank"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-Blank/**"
    }

    setup_project()

group "Basic"

-- Contains tutorial from getting started, lighting and model loading
project "LearnOpenGL-Basic"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-Basic/**"
    }

    setup_project()

group "Advanced Loading"

project "LearnOpenGL-DepthTest"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-DepthTest/**"
    }

    setup_project()

project "LearnOpenGL-Stencil"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-Stencil/**"
    }

    setup_project()

project "LearnOpenGL-Blending"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-Blending/**"
    }

    setup_project()

project "LearnOpenGL-FaceCulling"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-FaceCulling/**"
    }

    setup_project()

project "LearnOpenGL-FrameBuffer"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-FrameBuffer/**"
    }

    setup_project()

project "LearnOpenGL-Cubemaps"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-Cubemaps/**"
    }

    setup_project()

project "LearnOpenGL-GeometryNormalViz"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-GeometryNormalViz/**"
    }

    setup_project()

project "LearnOpenGL-GeometryHouses"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-GeometryHouses/**"
    }

    setup_project()

project "LearnOpenGL-GeometryExploding"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-GeometryExploding/**"
    }

    setup_project()

project "LearnOpenGL-SimpleInstancing"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-SimpleInstancing/**"
    }

    setup_project()

project "LearnOpenGL-AsteroidInstancing"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-AsteroidInstancing/**"
    }

    setup_project()

project "LearnOpenGL-MSAAOffscreen"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-MSAAOffscreen/**"
    }

    setup_project()

project "LearnOpenGL-MSAA"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-MSAA/**"
    }

    setup_project()

group "Advanced Lighting"

project "LearnOpenGL-BlinnPhong"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-BlinnPhong/**"
    }

    setup_project()

project "LearnOpenGL-ShadowMap"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-ShadowMap/**"
    }

    setup_project()

project "LearnOpenGL-PointShadows"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-PointShadows/**"
    }

    setup_project()

project "LearnOpenGL-NormalMap"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-NormalMap/**"
    }

    setup_project()

project "LearnOpenGL-ParallaxMapping"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-ParallaxMapping/**"
    }

    setup_project()

project "LearnOpenGL-HDR"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-HDR/**"
    }

    setup_project()

project "LearnOpenGL-Bloom"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-Bloom/**"
    }

    setup_project()

project "LearnOpenGL-DeferredShading"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-DeferredShading/**"
    }

    setup_project()

project "LearnOpenGL-SSAO"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-SSAO/**"
    }

    setup_project()

group "PBR"

project "LearnOpenGL-PBR"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-PBR/**"
    }

    setup_project()

project "LearnOpenGL-PBRTexture"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-PBRTexture/**"
    }

    setup_project()

project "LearnOpenGL-PBR_IBL"
    location (project_dir .. "/".. _ACTION)
    kind "ConsoleApp"
    language "C++"

    files
    {
        "LearnOpenGL/Common/**",
        "LearnOpenGL/LearnOpenGL-PBR_IBL/**"
    }

    setup_project()
