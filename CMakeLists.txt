cmake_minimum_required(VERSION 3.21)

project(CAS LANGUAGES CXX)

# C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependencias desde vcpkg
find_package(SDL3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# Archivos fuente
file(GLOB_RECURSE PROJECT_SOURCES
    src/*.cpp
)

file(GLOB IMGUI_BACKENDS
    src/platform/imgui/*.cpp
)

add_executable(CAS
    ${PROJECT_SOURCES}
    ${IMGUI_BACKENDS}
)

target_include_directories(CAS PRIVATE
    include
    src/platform/imgui
)

target_link_libraries(CAS PRIVATE
    SDL3::SDL3
    imgui::imgui
)

# Warnings
if (MSVC)
    target_compile_options(CAS PRIVATE /W4 /permissive-)
endif()