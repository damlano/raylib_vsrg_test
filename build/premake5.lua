-- Export Compile Commands module for clangd support
require "ecc/ecc"

-- Get the absolute path of the directory containing this script
local root = _MAIN_SCRIPT_DIR

newoption
{
    trigger = "graphics",
    value = "OPENGL_VERSION",
    description = "version of OpenGL to build raylib against",
    allowed = {
        { "opengl11", "OpenGL 1.1"},
        { "opengl21", "OpenGL 2.1"},
        { "opengl33", "OpenGL 3.3"},
        { "opengl43", "OpenGL 4.3"},
        { "openges2", "OpenGL ES2"},
        { "openges3", "OpenGL ES3"},
        { "software", "OpenGL 1.1 Software Render"}
    },
    default = "opengl33"
}

newoption
{
    trigger = "backend",
    value = "BACKEND_TYPE",
    description = "Backend Platform to use",
    allowed = {
        { "glfw", "GLFW"},
        { "rgfw", "RGFW"},
        { "win32", "WIN32"},
    },
    default = "glfw"
}

newoption
{
    trigger = "wayland",
    value = "WAYLAND",
    description = "build for wayland",
    allowed = {
        { "off", "Off"},
        { "on", "On"}
    },
    default = "off"
}

function download_progress(total, current)
    local ratio = current / total;
    ratio = math.min(math.max(ratio, 0), 1);
    local percent = math.floor(ratio * 100);
    print("Download progress (" .. percent .. "%/100%)")
end

function check_raylib()
    os.chdir("external")
    if(os.isdir("raylib-master") == false) then
        if(not os.isfile("raylib-master.zip")) then
            print("Raylib not found, downloading from github")
            local result_str, response_code = http.download("https://github.com/raysan5/raylib/archive/refs/heads/master.zip", "raylib-master.zip", {
                progress = download_progress,
                headers = { "From: Premake", "Referer: Premake" }
            })
        end
        print("Unzipping to " ..  os.getcwd())
        zip.extract("raylib-master.zip", os.getcwd())
        os.remove("raylib-master.zip")
    end
    os.chdir("../")
end

function check_tracy()
    os.chdir("external")
    if(os.isdir("tracy") == false) then
        print("Tracy not found, cloning from github")
        os.execute("git clone https://github.com/wolfpld/tracy.git tracy")
    end
    os.chdir("../")
end

function build_externals()
     print("calling externals")
     check_raylib()
     check_tracy()
end

function platform_defines()
    filter {"options:backend=glfw"}
        defines{"PLATFORM_DESKTOP"}

    filter {"options:backend=rgfw"}
        defines{"PLATFORM_DESKTOP_RGFW"}

    filter {"options:backend=win32"}
        defines{"PLATFORM_DESKTOP_WIN32"}

    filter {"options:graphics=opengl43"}
        defines{"GRAPHICS_API_OPENGL_43"}

    filter {"options:graphics=opengl33"}
        defines{"GRAPHICS_API_OPENGL_33"}

    filter {"options:graphics=opengl21"}
        defines{"GRAPHICS_API_OPENGL_21"}

    filter {"options:graphics=opengl11"}
        defines{"GRAPHICS_API_OPENGL_11"}

    filter {"options:graphics=openges3"}
        defines{"GRAPHICS_API_OPENGL_ES3"}

    filter {"options:graphics=openges2"}
        defines{"GRAPHICS_API_OPENGL_ES2"}

    filter {"options:graphics=software"}
        defines{"GRAPHICS_API_OPENGL_11_SOFTWARE"}

    filter {"system:macosx"}
        disablewarnings {"deprecated-declarations"}

    filter {"system:linux", "options:wayland=off"}
        defines {"_GLFW_X11"}

    filter {"system:linux", "options:wayland=on"}
        defines {"_GLFW_WAYLAND"}

    filter {}
end

-- Settings
downloadRaylib = true
raylib_dir = "external/raylib-master"

workspaceName = 'MyGame'
baseName = path.getbasename(path.getdirectory(os.getcwd()));
workspaceName = baseName

if (os.isdir('build_files') == false) then
    os.mkdir('build_files')
end

if (os.isdir('external') == false) then
    os.mkdir('external')
end

workspace (workspaceName)
    location "build_files" -- Kept inside for clean root
    configurations { "Debug", "Release"}
    platforms { "x64", "x86", "ARM64"}
    defaultplatform ("x64")

    filter "configurations:Debug"
        defines { "DEBUG", "TRACY_ENABLE" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter {"configurations:Release", "action:vs*"}
       linktimeoptimization "On"

    filter { "platforms:x64" }
        architecture "x86_64"

    filter { "platforms:ARM64" }
        architecture "ARM64"
    filter {}

    targetdir "bin/%{cfg.buildcfg}/"

if (downloadRaylib) then
    build_externals()
end

startproject(workspaceName)

---
-- Tracy Static Library
---
project "Tracy"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    location "build_files/"
    targetdir "../bin/%{cfg.buildcfg}"
    objdir "../bin/obj/%{prj.name}"
    
    files 
    { 
        root .. "/external/tracy/public/TracyClient.cpp"
    }
    
    includedirs
    {
        root .. "/external/tracy/public"
    }
    
    filter "system:windows"
        defines { "WIN32_LEAN_AND_MEAN", "NOMINMAX" }
        systemversion "latest"
        
    filter "system:linux"
        links { "pthread", "dl", "rt" }
        buildoptions { "-pthread" }
        
    filter "system:macosx"
        links { "pthread", "dl" }
        buildoptions { "-pthread" }
    
    filter "configurations:Debug"
        defines { "TRACY_ENABLE" }
        runtime "Debug"
        symbols "On"
        
    filter "configurations:Release"
        -- Tracy usually disabled in Release unless specifically requested
        runtime "Release"
        optimize "On"
    filter {}

---
-- Main Application
---
project (workspaceName)
    kind "ConsoleApp"
    location "build_files/"
    targetdir "../bin/%{cfg.buildcfg}"

    filter {"system:windows", "configurations:Release", "action:gmake*"}
        kind "WindowedApp"
        buildoptions { "-Wl,--subsystem,windows" }

    filter {"system:windows", "configurations:Release", "action:vs*"}
        kind "WindowedApp"
        entrypoint "mainCRTStartup"

    filter "action:vs*"
        debugdir "$(SolutionDir)"
    filter{}

    vpaths 
    {
        ["Header Files/*"] = { "../include/**.h",  "../include/**.hpp", "../src/**.h", "../src/**.hpp"},
        ["Source Files/*"] = {"../src/**.c", "src/**.cpp"},
        ["Windows Resource Files/*"] = {"../src/**.rc", "../src/**.ico"},
        ["Game Resource Files/*"] = {"../resources/**"},
    }
    
    files {"../src/**.c", "../src/**.cpp", "../src/**.h", "../src/**.hpp", "../include/**.h", "../include/**.hpp"}
    
    filter {"system:windows", "action:vs*"}
        files {"../src/*.rc", "../src/*.ico"}
        files {"../resources/**"}
    filter{}
    
    includedirs { 
        "../src", 
        "../include",
        root .. "/external/tracy/public",
        root .. "/" .. raylib_dir .. "/src"
    }

    links {"raylib", "Tracy"}

    cdialect "C17"
    cppdialect "C++23"

    flags { "ShadowedVariables"}
    platform_defines()

    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
        dependson {"raylib", "Tracy"}
        links {"raylib.lib", "Tracy.lib"}
        characterset ("Unicode")
        buildoptions { "/Zc:__cplusplus" }
        
    filter "system:windows"
        defines{"_WIN32"}
        links {"winmm", "gdi32", "opengl32"}
        libdirs {"../bin/%{cfg.buildcfg}"}

    filter "system:linux"
        links {"pthread", "m", "dl", "rt"}
        buildoptions { "-pthread" }
        libdirs {"../bin/%{cfg.buildcfg}"}

    filter {"system:linux", "options:wayland=off"}
        links {"X11"}

    filter {"system:linux", "options:wayland=on"}
        links {"wayland-client", "wayland-cursor", "wayland-egl", "xkbcommon"}

    filter "system:macosx"
        links {"OpenGL.framework", "Cocoa.framework", "IOKit.framework", "CoreFoundation.framework", "CoreAudio.framework", "CoreVideo.framework", "AudioToolbox.framework"}
        buildoptions { "-pthread" }
        libdirs {"../bin/%{cfg.buildcfg}"}
    filter{}

---
-- Raylib Static Library
---
project "raylib"
    kind "StaticLib"
    platform_defines()
    location "build_files/"
    language "C"
    targetdir "../bin/%{cfg.buildcfg}"

    filter {"options:wayland=on"}
        defines {"GLFW_LINUX_ENABLE_WAYLAND=TRUE" }

    filter {"options:wayland=on", "system:linux"}
        prebuildcommands {
            "@echo Generating Wayland protocols...",
            "@wayland-scanner client-header ../" .. raylib_dir .. "/src/external/glfw/deps/wayland/wayland.xml ../" .. raylib_dir .. "/src/wayland-client-protocol.h",
            "@wayland-scanner client-header ../" .. raylib_dir .. "/src/external/glfw/deps/wayland/xdg-shell.xml ../" .. raylib_dir .. "/src/xdg-shell-client-protocol.h",
            "@wayland-scanner client-header ../" .. raylib_dir .. "/src/external/glfw/deps/wayland/xdg-decoration-unstable-v1.xml ../" .. raylib_dir .. "/src/xdg-decoration-unstable-v1-client-protocol.h",
            "@wayland-scanner client-header ../" .. raylib_dir .. "/src/external/glfw/deps/wayland/viewporter.xml ../" .. raylib_dir .. "/src/viewporter-client-protocol.h",
            "@wayland-scanner client-header ../" .. raylib_dir .. "/src/external/glfw/deps/wayland/relative-pointer-unstable-v1.xml ../" .. raylib_dir .. "/src/relative-pointer-unstable-v1-client-protocol.h",
            "@wayland-scanner client-header ../" .. raylib_dir .. "/src/external/glfw/deps/wayland/pointer-constraints-unstable-v1.xml ../" .. raylib_dir .. "/src/pointer-constraints-unstable-v1-client-protocol.h",
            "@wayland-scanner client-header ../" .. raylib_dir .. "/src/external/glfw/deps/wayland/fractional-scale-v1.xml ../" .. raylib_dir .. "/src/fractional-scale-v1-client-protocol.h",
            "@wayland-scanner client-header ../" .. raylib_dir .. "/src/external/glfw/deps/wayland/xdg-activation-v1.xml ../" .. raylib_dir .. "/src/xdg-activation-v1-client-protocol.h",
            "@wayland-scanner client-header ../" .. raylib_dir .. "/src/external/glfw/deps/wayland/idle-inhibit-unstable-v1.xml ../" .. raylib_dir .. "/src/idle-inhibit-unstable-v1-client-protocol.h",
        }
    filter {}

    filter "action:vs*"
        defines{"_WINSOCK_DEPRECATED_NO_WARNINGS", "_CRT_SECURE_NO_WARNINGS"}
        characterset ("Unicode")
        buildoptions { "/Zc:__cplusplus" }
    filter{}

    includedirs { root .. "/" .. raylib_dir .. "/src", root .. "/" .. raylib_dir .. "/src/external/glfw/include" }
    
    vpaths
    {
        ["Header Files"] = { root .. "/" .. raylib_dir .. "/src/**.h"},
        ["Source Files/*"] = { root .. "/" .. raylib_dir .. "/src/**.c"},
    }
    
    files { root .. "/" .. raylib_dir .. "/src/*.h", root .. "/" .. raylib_dir .. "/src/*.c"}

    removefiles { root .. "/" .. raylib_dir .. "/src/rcore_*.c"}

    filter { "system:macosx", "files:" .. root .. "/" .. raylib_dir .. "/src/rglfw.c" }
        compileas "Objective-C"
    filter{}