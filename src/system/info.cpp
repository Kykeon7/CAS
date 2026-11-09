#include <system/info.h>
#include <system/logger.h>

#include <SDL3/SDL.h>
#include <imgui.h>

#include <string>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winternl.h>
#elif defined(__linux__)
#include <sys/utsname.h>
#endif

namespace cas::system {
namespace {

const char* operating_system()
{
#if defined(_WIN32)
    return "Windows";
#elif defined(__APPLE__)
    return "macOS";
#elif defined(__linux__)
    return "Linux";
#else
    return "Unknown";
#endif
}

const char* architecture()
{
#if defined(_M_X64) || defined(__x86_64__)
    return "x86_64";
#elif defined(_M_ARM64) || defined(__aarch64__)
    return "ARM64";
#elif defined(_M_IX86) || defined(__i386__)
    return "x86";
#else
    return "Unknown";
#endif
}

std::string operating_system_version()
{
#if defined(_WIN32)
    using rtl_get_version_fn =
        LONG(WINAPI*)(PRTL_OSVERSIONINFOW);

    const auto ntdll = GetModuleHandleW(L"ntdll.dll");

    if (!ntdll)
        return "Unknown";

    const auto get_version =
        reinterpret_cast<rtl_get_version_fn>(
            GetProcAddress(ntdll, "RtlGetVersion")
        );

    if (!get_version)
        return "Unknown";

    RTL_OSVERSIONINFOW version{};
    version.dwOSVersionInfoSize = sizeof(version);

    if (get_version(&version) != 0)
        return "Unknown";

    return std::to_string(version.dwMajorVersion) + "." +
        std::to_string(version.dwMinorVersion) + "." +
        std::to_string(version.dwBuildNumber);

#elif defined(__linux__)
    utsname info{};

    if (uname(&info) != 0)
        return "Unknown";

    return info.release;

#else
    return "Not available";
#endif
}

const char* compiler()
{
#if defined(_MSC_VER)
    return "MSVC";
#elif defined(__clang__)
    return "Clang";
#elif defined(__GNUC__)
    return "GCC";
#else
    return "Unknown";
#endif
}

std::string compiler_version()
{
#if defined(_MSC_VER)
    return std::to_string(_MSC_VER);
#elif defined(__clang__)
    return std::to_string(__clang_major__) + "." +
        std::to_string(__clang_minor__) + "." +
        std::to_string(__clang_patchlevel__);
#elif defined(__GNUC__)
    return std::to_string(__GNUC__) + "." +
        std::to_string(__GNUC_MINOR__) + "." +
        std::to_string(__GNUC_PATCHLEVEL__);
#else
    return "Unknown";
#endif
}

long cxx_standard()
{
#ifdef _MSVC_LANG
    return _MSVC_LANG;
#else
    return __cplusplus;
#endif
}

std::string cxx_standard_name()
{
    const long standard = cxx_standard();

    if (standard >= 202302L)
        return "C++23";
    if (standard >= 202002L)
        return "C++20";
    if (standard >= 201703L)
        return "C++17";
    if (standard >= 201402L)
        return "C++14";
    if (standard >= 201103L)
        return "C++11";

    return "Pre-C++11";
}

} // namespace

void print_system_info()
{
    auto& logger = system::logger::instance();

    const int sdl_version = SDL_GetVersion();

#ifdef NDEBUG
    constexpr const char* configuration = "Release";
#else
    constexpr const char* configuration = "Debug";
#endif

    logger.info("System", "========== CAS - SYSTEM INFO ==========");

    logger.info("Application", "Name: CAS");
    logger.info("Application", std::string("Configuration: ") + configuration);

    logger.info("System", std::string("OS: ") + operating_system());
    logger.info("System", "OS Version: " + operating_system_version());
    logger.info("System", std::string("Architecture: ") + architecture());

    logger.info(
        "Development",
        "C++ Standard: " + cxx_standard_name() +
            " (" + std::to_string(cxx_standard()) + ")"
    );

    logger.info(
        "Development",
        std::string("Compiler: ") + compiler() +
            " " + compiler_version()
    );

    logger.info(
        "Libraries",
        "SDL: " +
            std::to_string(SDL_VERSIONNUM_MAJOR(sdl_version)) + "." +
            std::to_string(SDL_VERSIONNUM_MINOR(sdl_version)) + "." +
            std::to_string(SDL_VERSIONNUM_MICRO(sdl_version))
    );

    logger.info(
        "Libraries",
        std::string("Dear ImGui: ") + IMGUI_VERSION
    );

    logger.info("System", "=======================================");
}

} // namespace cas::system