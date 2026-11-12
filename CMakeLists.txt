cmake_minimum_required(VERSION 3.21)

project(CAS LANGUAGES CXX)

# ============================================================
# Default build configuration
# ============================================================

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
        "Build configuration" FORCE
    )
endif()

# ============================================================
# Project configuration
# ============================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ============================================================
# Dependencies
# ============================================================

find_package(SDL3 CONFIG REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)

# ============================================================
# Dear ImGui
# ============================================================

add_library(imgui STATIC
    # Core
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp

    # SDL3 backends
    external/imgui/backends/imgui_impl_sdl3.cpp
    external/imgui/backends/imgui_impl_sdlrenderer3.cpp
    external/imgui/backends/imgui_impl_sdlgpu3.cpp
)

target_include_directories(imgui PUBLIC
    external/imgui
    external/imgui/backends
)

target_link_libraries(imgui PUBLIC
    SDL3::SDL3
)

# ============================================================
# ImPlot
# ============================================================

add_library(implot STATIC
    external/implot/implot.cpp
    external/implot/implot_items.cpp
)

target_include_directories(implot PUBLIC
    external/implot
)

target_link_libraries(implot PUBLIC
    imgui
)

# ============================================================
# Platform-specific system implementations
# ============================================================

set(CAS_SYSTEM_SOURCES)

if(WIN32)
    list(APPEND CAS_SYSTEM_SOURCES
        src/system/memory_windows.cpp
        src/system/gpu_memory_windows.cpp
    )

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    list(APPEND CAS_SYSTEM_SOURCES
        src/system/memory_linux.cpp
        src/system/gpu_memory_linux.cpp
    )

else()
    message(FATAL_ERROR
        "Unsupported platform: ${CMAKE_SYSTEM_NAME}"
    )
endif()


# ============================================================
# CAS executable
# ============================================================

add_executable(CAS
    # --------------------------------------------------------
    # Entry point
    # --------------------------------------------------------

    src/main.cpp

    # --------------------------------------------------------
    # Application
    # --------------------------------------------------------

    src/application/application.cpp

    # --------------------------------------------------------
    # Automata
    # --------------------------------------------------------

    src/automata/automaton.cpp
    src/automata/grid_storage.cpp
    src/automata/pattern.cpp
    src/automata/topology.cpp

    # Rules
    "src/automata/rules/beehive.cpp"
    "src/automata/rules/binary.cpp"
    src/automata/rules/factory.cpp
    src/automata/rules/rule_22_majority_memory.cpp
    "src/automata/rules/spiral.cpp"

    # --------------------------------------------------------
    # Files
    # --------------------------------------------------------

    src/files/dialog_service.cpp
    src/files/operation_handler.cpp

    # Serialization
    "src/files/serialization/settings.cpp"
    "src/files/serialization/automaton.cpp"
    "src/files/serialization/viewport.cpp"
    "src/files/serialization/workspace.cpp"

    # --------------------------------------------------------
    # GUI
    # --------------------------------------------------------

    src/gui/help_content.cpp
    src/gui/initializer.cpp
    src/gui/layout.cpp
    src/gui/main_menu.cpp
    src/gui/manager.cpp
    "src/gui/notifications.cpp"
    "src/gui/panels/container.cpp"
    src/gui/rule_editor.cpp
    src/gui/settings_window.cpp
    src/gui/style.cpp

    # Panels
    src/gui/panels/appearance.cpp
    src/gui/panels/file.cpp
    src/gui/panels/graphs.cpp
    src/gui/panels/help.cpp
    src/gui/panels/simulation.cpp
    src/gui/panels/stats.cpp
    src/gui/panels/viewport.cpp

    # --------------------------------------------------------
    # Inputs
    # --------------------------------------------------------

    src/inputs/input.cpp

    # --------------------------------------------------------
    # Platform
    # --------------------------------------------------------

    src/platform/gpu_device.cpp
    src/platform/gpu_frame.cpp
    src/platform/gpu_renderer.cpp
    src/platform/renderer.cpp
    src/platform/sdl_context.cpp
    src/platform/sdl_gpu_texture.cpp
    src/platform/sdl_texture.cpp
    src/platform/window.cpp

    # --------------------------------------------------------
    # Rendering
    # --------------------------------------------------------

    src/rendering/context.cpp
    src/rendering/resources.cpp

    # Automaton renderers
    src/rendering/automaton/gpu_renderer.cpp
    src/rendering/automaton/hex_renderer.cpp

    # --------------------------------------------------------
    # Simulations
    # --------------------------------------------------------

    src/simulations/simulation.cpp

    # --------------------------------------------------------
    # System
    # --------------------------------------------------------

    src/system/info.cpp
    src/system/logger.cpp
    src/system/profiler.cpp
    src/system/time.cpp

    # Platform-specific memory and GPU memory implementations
    ${CAS_SYSTEM_SOURCES}

    # --------------------------------------------------------
    # Viewports
    # --------------------------------------------------------

    src/viewports/camera.cpp
    src/viewports/manager.cpp
    src/viewports/viewport.cpp
 "src/workspace/workspace.cpp" "src/files/coordinator.cpp" "src/viewports/linear_history.cpp")

# ============================================================
# Include directories
# ============================================================

target_include_directories(CAS PRIVATE
    include
)


# ============================================================
# Link libraries
# ============================================================

target_link_libraries(CAS PRIVATE
    SDL3::SDL3
    imgui
    implot
    nlohmann_json::nlohmann_json
)
if(WIN32)
    target_link_libraries(CAS PRIVATE
        Psapi
        dxgi
    )
endif()


# ============================================================
# Compiler warnings
# ============================================================

if(MSVC)
    target_compile_options(CAS PRIVATE
        /W4
        /permissive-
    )
endif()

# ============================================================
# Runtime resources
# ============================================================

file(GLOB CAS_SHADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/resources/shaders/*"
)

set(CAS_SHADER_OUTPUT_DIR
    "$<TARGET_FILE_DIR:CAS>/resources/shaders"
)

set(CAS_SHADER_OUTPUTS)

foreach(SHADER_FILE IN LISTS CAS_SHADER_FILES)
    get_filename_component(SHADER_NAME "${SHADER_FILE}" NAME)

    set(SHADER_OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/shader_resources/${SHADER_NAME}"
    )

    add_custom_command(
        OUTPUT "${SHADER_OUTPUT}"
        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_CURRENT_BINARY_DIR}/shader_resources"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SHADER_FILE}"
            "${SHADER_OUTPUT}"
        DEPENDS "${SHADER_FILE}"
        VERBATIM
    )

    list(APPEND CAS_SHADER_OUTPUTS "${SHADER_OUTPUT}")
endforeach()

add_custom_target(CASShaders ALL
    DEPENDS ${CAS_SHADER_OUTPUTS}
)

add_custom_command(TARGET CAS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "${CAS_SHADER_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_BINARY_DIR}/shader_resources"
        "${CAS_SHADER_OUTPUT_DIR}"
    COMMENT "Copying GPU shaders to the output directory"
    VERBATIM
)

add_dependencies(CAS CASShaders)