cmake_minimum_required(VERSION 4.0)
project(SensoryOverload)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(BUILD_SHARED_LIBS OFF)

include(FetchContent)
FetchContent_Declare(json
        URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(json)

add_executable(SensoryOverload src/main.cc
        src/world/world.h
        src/data/content.h
        src/data/content.cc
        src/consts.h
        src/data/loader.h
        src/world/world.cc
        src/data/loader.cc
        src/component/component.h
        src/world/mob.h
        src/world/player.h
        src/component/component.cc
        src/system/input.h
        src/system/input.cc
        src/system/mob_system.h
        src/system/mob_system.cc
        src/system/render_system.h
        src/system/render_system.cc
        src/system/audio_system.h
        src/system/audio_system.cc
        src/system/interaction_system.h
        src/system/interaction_system.cc)

target_link_libraries(SensoryOverload PRIVATE nlohmann_json::nlohmann_json)
target_link_options(SensoryOverload PRIVATE
        -static-libgcc
        -static-libstdc++
        -static
)