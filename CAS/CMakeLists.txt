cmake_minimum_required(VERSION 3.16)

project(CAS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

add_executable(CAS
    renderer.cpp
    sdl_context.cpp
    window.cpp
    src/application.cpp
    src/gui_manager.cpp
    src/hex_automaton.cpp
    src/input.cpp
    src/main.cpp
    src/time.cpp
)

target_include_directories(CAS
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(CAS
    PRIVATE
        SDL3::SDL3
        imgui::imgui
)

if (UNIX)
    target_compile_options(CAS PRIVATE -Wall -Wextra)
endif()
