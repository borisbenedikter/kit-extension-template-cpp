-- Setup the extension.
local ext = get_current_extension_info()
project_ext(ext)

-- Link folders that should be packaged with the extension.
repo_build.prebuild_link {
    { "data", ext.target_dir.."/data" },
    { "docs", ext.target_dir.."/docs" },
    { "boris", ext.target_dir.."/boris" },
}

-- Build the C++ plugin that will be loaded by the extension.
-- The plugin must implement the omni::ext::IExt interface to
-- be automatically loaded by the extension system at startup.
project_ext_plugin(ext, "boris.example.cpp.hello_world_physX.plugin")
    local plugin_name = "boris.example.cpp.hello_world_physX"
    add_files("source", "plugins/"..plugin_name)
    includedirs { "plugins/"..plugin_name , "../../../extern/PhysX/physx/include" , "../../../extern/PhysX/physx/snippets" }
    libdirs {
        "../../../extern/PhysX/physx/bin/linux.clang/checked",
        "/usr/local/cuda/lib64",  -- here there is only cudart, but no cuda
        os.findlib("cuda")
    }
    links {
        "PhysX_static_64",
        "PhysXPvdSDK_static_64",
        "PhysXExtensions_static_64",
        "PhysXCooking_static_64",
        "PhysXCommon_static_64",
        "PhysXFoundation_static_64",
        "PhysXVehicle_static_64",
        "PhysXVehicle2_static_64",
        "PhysXCharacterKinematic_static_64",
        "SnippetUtils_static_64",
        "PVDRuntime_64",
        "PhysXGpu_64",
    }
        -- "cuda"

        -- "cuda"
    -- }
    -- defines { "NOMINMAX", "NDEBUG" }
    -- runtime "Release"
    -- rtti "On"


    -- filter { "system:linux" }
    --     exceptionhandling "On"
    --     staticruntime "Off"
    --     cppdialect "C++17"
    --     includedirs { "%{target_deps}/python/include/python3.10" }
    --     buildoptions { "-D_GLIBCXX_USE_CXX11_ABI=0 -Wno-deprecated-declarations -Wno-deprecated -Wno-unused-variable -pthread -lstdc++fs -Wno-undef" }
    --     linkoptions { "-Wl,--disable-new-dtags -Wl,-rpath,%{target_deps}/nv_usd/release/lib:%{target_deps}/python/lib:%{target_deps}/usd_ext_physics/release/lib:" }
    -- filter { "system:windows" }
    --     buildoptions { "/wd4244 /wd4305" }
    -- filter {}

    -- links {
    --     "PhysX",
    --     "PhysXPvdSDK",
    --     "PhysXExtensions",
    --     "PhysXCooking",
    --     "PhysXCommon",
    --     "PhysXFoundation",
    --     "PhysXVehicle",
    --     "PhysXVehicle2",
    --     "PhysXCharacterKinematic",
    --     "SnippetUtils",
    --     "PVDRuntime",
    --     "PhysXGpu",
    --     "cuda"
    -- }
