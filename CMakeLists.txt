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

add_executable(SensoryOverload src/main.cpp
        src/world/world_state.h
        src/data/content.h
        src/data/content.cc)

target_link_libraries(SensoryOverload PRIVATE nlohmann_json::nlohmann_json)
target_link_options(SensoryOverload PRIVATE
        -static-libgcc
        -static-libstdc++
        -static
)