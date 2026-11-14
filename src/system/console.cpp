
#include <system/console.h>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#else

#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

#endif

namespace cas::system {

    bool set_console_enabled(bool enabled) {
#ifdef _WIN32

        HWND console = GetConsoleWindow();

        if (enabled) {
            if (!console) {
                if (
                    !AttachConsole(ATTACH_PARENT_PROCESS) &&
                    !AllocConsole()
                    )
                    return false;

                console = GetConsoleWindow();
            }

            if (!console)
                return false;

            ShowWindow(console, SW_SHOW);
            return true;
        }

        if (console)
            ShowWindow(console, SW_HIDE);

        return true;

#else

        if (enabled)
            return true;

        const bool stdout_redirected =
            std::freopen("/dev/null", "w", stdout) != nullptr;

        const bool stderr_redirected =
            std::freopen("/dev/null", "w", stderr) != nullptr;

        return stdout_redirected && stderr_redirected;

#endif
    }

} // namespace cas::system
