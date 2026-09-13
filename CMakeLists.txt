cmake_minimum_required(VERSION 3.21)

project(CAS LANGUAGES CXX)

# ============================================================
# C++23
# ============================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ============================================================
# Dependencies
# ============================================================

find_package(SDL3 CONFIG REQUIRED)


# ============================================================
# Dear ImGui
# ============================================================

add_library(imgui
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp

    external/imgui/backends/imgui_impl_sdl3.cpp
    external/imgui/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui PUBLIC
    external/imgui
    external/imgui/backends
)

target_link_libraries(imgui PUBLIC
    SDL3::SDL3
)

target_compile_definitions(imgui PUBLIC
    SDL_ENABLE_OLD_NAMES
)


# ============================================================
# CAS
# ============================================================

add_executable(CAS

    # --------------------------------------------------------
    # Entry point
    # --------------------------------------------------------

    src/main.cpp

    # --------------------------------------------------------
    # Core
    # --------------------------------------------------------

    src/core/application.cpp
    src/core/time.cpp
    src/core/camera.cpp
    src/core/simulation.cpp
    src/core/viewport.cpp

    # --------------------------------------------------------
    # Platform
    # --------------------------------------------------------

    src/platform/sdl_context.cpp
    src/platform/window.cpp

    # --------------------------------------------------------
    # Input
    # --------------------------------------------------------

    src/core/input/input.cpp

    # --------------------------------------------------------
    # GUI
    # --------------------------------------------------------

    src/core/gui/gui_manager.cpp

    # --------------------------------------------------------
    # Rendering
    # --------------------------------------------------------

    src/core/rendering/renderer.cpp
    src/core/rendering/render_target.cpp

    # Automaton renderers
    src/core/rendering/automaton_renderers/hex_renderer.cpp

    # --------------------------------------------------------
    # Automaton
    # --------------------------------------------------------

    src/automaton/automaton.cpp
    src/automaton/grid_storage.cpp
    src/automaton/bit_array.cpp
    src/automaton/rules.cpp
    src/automaton/hex_automaton.cpp

    # --------------------------------------------------------
    # GUI Panels
    # --------------------------------------------------------

    src/core/gui/panels/sim_panel.cpp
    src/core/gui/panels/stats_panel.cpp
    src/core/gui/panels/appearance_panel.cpp
    src/core/gui/panels/graphs_panel.cpp
    src/core/gui/panels/help_panel.cpp
    src/core/gui/panels/view_panel.cpp
    src/core/gui/panels/viewports_panel.cpp
    src/core/gui/panels/file_panel.cpp

    # --------------------------------------------------------
    # System
    # --------------------------------------------------------

    src/system/profiler.cpp
)


target_include_directories(CAS PRIVATE
    include
)


target_link_libraries(CAS PRIVATE
    SDL3::SDL3
    imgui
)


# ============================================================
# Warnings
# ============================================================

if (MSVC)
    target_compile_options(CAS PRIVATE
        /W4
        /permissive-
    )
endif()