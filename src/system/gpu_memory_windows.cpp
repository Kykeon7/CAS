
#include <system/gpu_memory.h>

#define NOMINMAX
#include <windows.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

#pragma comment(lib, "dxgi.lib")

namespace cas::system {

namespace {

using Microsoft::WRL::ComPtr;

std::wstring to_wide(const char* text) {
    if (!text || !*text) {
        return {};
    }

    const int length = MultiByteToWideChar(
        CP_UTF8,
        0,
        text,
        -1,
        nullptr,
        0
    );

    if (length <= 1) {
        return {};
    }

    std::wstring result(
        static_cast<std::size_t>(length - 1),
        L'\0'
    );

    MultiByteToWideChar(
        CP_UTF8,
        0,
        text,
        -1,
        result.data(),
        length
    );

    return result;
}

std::wstring lowercase(std::wstring value) {
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](wchar_t character) {
            return static_cast<wchar_t>(
                std::tolower(character)
                );
        }
    );

    return value;
}

struct adapter_entry {
    ComPtr<IDXGIAdapter1> adapter;
    DXGI_ADAPTER_DESC1 description{};
};

} // namespace

gpu_memory_info get_gpu_memory_info(
    const char* sdl_device_name
) {
    gpu_memory_info result{};

    ComPtr<IDXGIFactory1> factory;

    if (FAILED(CreateDXGIFactory1(
        IID_PPV_ARGS(&factory)
    ))) {
        return result;
    }

    std::vector<adapter_entry> adapters;

    for (UINT index = 0;; ++index) {
        adapter_entry entry;

        if (factory->EnumAdapters1(
            index,
            &entry.adapter
        ) == DXGI_ERROR_NOT_FOUND) {
            break;
        }

        if (!entry.adapter) {
            continue;
        }

        if (FAILED(entry.adapter->GetDesc1(
            &entry.description
        ))) {
            continue;
        }

        if (entry.description.Flags &
            DXGI_ADAPTER_FLAG_SOFTWARE) {
            continue;
        }

        adapters.push_back(std::move(entry));
    }

    if (adapters.empty()) {
        return result;
    }

    const std::wstring requested =
        lowercase(to_wide(sdl_device_name));

    // Avoid selecting an arbitrary GPU on multi-GPU systems.
    std::vector<const adapter_entry*> matches;

    if (!requested.empty()) {
        for (const auto& entry : adapters) {
            const std::wstring adapter_name =
                lowercase(entry.description.Description);

            if (requested.find(adapter_name) !=
                std::wstring::npos) {
                matches.push_back(&entry);
            }
        }
    }

    if (matches.size() != 1) {
        return result;
    }

    const adapter_entry& selected = *matches.front();

    ComPtr<IDXGIAdapter3> adapter;

    if (FAILED(selected.adapter.As(&adapter))) {
        return result;
    }

    DXGI_QUERY_VIDEO_MEMORY_INFO local{};
    DXGI_QUERY_VIDEO_MEMORY_INFO non_local{};

    if (FAILED(adapter->QueryVideoMemoryInfo(
        0,
        DXGI_MEMORY_SEGMENT_GROUP_LOCAL,
        &local
    ))) {
        return result;
    }

    if (FAILED(adapter->QueryVideoMemoryInfo(
        0,
        DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL,
        &non_local
    ))) {
        return result;
    }

    result.dedicated_capacity_bytes =
        static_cast<std::size_t>(
            selected.description.DedicatedVideoMemory
            );

    result.shared_capacity_bytes =
        static_cast<std::size_t>(
            selected.description.SharedSystemMemory
            );

    result.local_used_bytes =
        static_cast<std::size_t>(local.CurrentUsage);

    result.local_budget_bytes =
        static_cast<std::size_t>(local.Budget);

    result.non_local_used_bytes =
        static_cast<std::size_t>(non_local.CurrentUsage);

    result.non_local_budget_bytes =
        static_cast<std::size_t>(non_local.Budget);

    result.available = true;

    return result;
}

} // namespace cas::system
