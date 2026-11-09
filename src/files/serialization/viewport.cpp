#include <files/serialization/workspace.h>

#include <files/serialization/automaton.h>

#include <nlohmann/json.hpp>

#include <cmath>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <files/serialization/json_file_reader.h>

namespace cas::files::serialization {

namespace {

using json = nlohmann::json;

json save_vec2(const math::vec2f& value) {
    return {
        { "x", value.x },
        { "y", value.y }
    };
}

math::vec2f load_vec2(const json& document) {
    const float x = document.at("x").get<float>();
    const float y = document.at("y").get<float>();

    if (!std::isfinite(x) || !std::isfinite(y))
        throw std::invalid_argument("Vector coordinates must be finite.");

    return { x, y };
}

json save_color(const graphics::color& value) {
    return {
        { "r", value.r },
        { "g", value.g },
        { "b", value.b },
        { "a", value.a }
    };
}

graphics::color load_color(const json& document) {
    const float r = document.at("r").get<float>();
    const float g = document.at("g").get<float>();
    const float b = document.at("b").get<float>();
    const float a = document.at("a").get<float>();

    const auto valid_channel = [](float channel) {
        return std::isfinite(channel)
            && channel >= 0.0f
            && channel <= 1.0f;
    };

    if (!valid_channel(r) ||
        !valid_channel(g) ||
        !valid_channel(b) ||
        !valid_channel(a)) {
        throw std::invalid_argument(
            "Color channels must be finite values between 0 and 1."
        );
    }

    return { r, g, b, a };
}

json save_render_config(const cas::rendering::automaton::config& config) {
    return {
        { "background_color", save_color(config.background_color) },
        { "grid_color", save_color(config.grid_color) },
        { "alive_color", save_color(config.alive_color) },
        { "persistent_color", save_color(config.persistent_color) },
        { "cell_size", config.cell_size },
        { "offset", save_vec2(config.offset) }
    };
}

cas::rendering::automaton::config load_render_config(const json& document) {
    cas::rendering::automaton::config config;

    config.background_color =
        load_color(document.at("background_color"));

    config.grid_color =
        load_color(document.at("grid_color"));

    config.alive_color =
        load_color(document.at("alive_color"));

    config.persistent_color =
        load_color(document.at("persistent_color"));

    config.cell_size = document.at("cell_size").get<float>();
    config.offset = load_vec2(document.at("offset"));

    if (!std::isfinite(config.cell_size) || config.cell_size <= 0.0f)
        throw std::invalid_argument("Cell size must be finite and positive.");

    return config;
}

json save_history(const std::vector<float>& history) {
    return history;
}


std::vector<float> load_history(const json& document) {
    std::vector<float> history;

    if (!document.is_array())
        return history;

    history.reserve(cas::viewports::history_buffer::max_size);

    for (const auto& value : document) {
        if (!value.is_number())
            continue;

        try {
            const float sample = value.get<float>();

            if (!std::isfinite(sample))
                continue;

            if (history.size() == cas::viewports::history_buffer::max_size)
                history.erase(history.begin());

            history.push_back(sample);
        }
        catch (const json::exception&) {
            // Un valor inválido no debe invalidar el viewport.
        }
    }

    return history;
}

std::vector<float> load_history_member(
    const json& document,
    const char* key
) {
    if (!document.is_object() || !document.contains(key))
        return {};

    return load_history(document.at(key));
}

std::vector<std::uint8_t> load_linear_history(
    const json& document,
    std::size_t max_size
) {
    if (!document.is_array() || document.size() > max_size)
        return {};

    std::vector<std::uint8_t> history;
    history.reserve(document.size());

    for (const auto& value : document) {
        if (!value.is_number_integer())
            return {};

        const auto state = value.get<int>();

        if (state != 0 && state != 1)
            return {};

        history.push_back(
            static_cast<std::uint8_t>(state)
        );
    }

    return history;
}

} // namespace


viewport_snapshot capture_viewport(
    const cas::viewports::viewport& value
) {
    viewport_snapshot snapshot;

    snapshot.name = value.name();
    snapshot.simulation = capture_automaton(
        value.simulation().automaton()
    );

    snapshot.camera_position = value.camera().position();
    snapshot.camera_zoom = value.camera().zoom();

    snapshot.render_config = value.config();
    snapshot.preview_render_config = value.preview_config();

    const auto population = value.population_history().samples();
    const auto density = value.density_history().samples();
    const auto activity = value.activity_history().samples();

    snapshot.population_history.assign(
        population.begin(),
        population.end()
    );

    snapshot.density_history.assign(
        density.begin(),
        density.end()
    );

    snapshot.activity_history.assign(
        activity.begin(),
        activity.end()
    );

    snapshot.linear_history =
        value.linear_history_data().chronological_data();

    return snapshot;
}

json save_viewport(const viewport_snapshot& snapshot) {
    return {
        { "schema_version", 1 },
        { "name", snapshot.name },
        { "simulation", save_automaton(snapshot.simulation) },
        {
            "camera",
            {
                { "position", save_vec2(snapshot.camera_position) },
                { "zoom", snapshot.camera_zoom }
            }
        },
        { "render_config", save_render_config(snapshot.render_config) },
        {
            "preview_render_config",
            save_render_config(snapshot.preview_render_config)
        },
        {
            "history",
            {
                { "population", save_history(snapshot.population_history) },
                { "density", save_history(snapshot.density_history) },
                { "activity", save_history(snapshot.activity_history) },
                { "linear", snapshot.linear_history }
            }
        }
    };
}

json save_viewport(const cas::viewports::viewport& value) {
    return save_viewport(capture_viewport(value));
}

cas::viewports::viewport load_viewport(
    const json& document,
    progress_reporter progress
) {
    if (!document.is_object())
        throw std::invalid_argument(
            "Viewport document must be an object."
        );

    if (document.at("schema_version").get<int>() != 1)
        throw std::invalid_argument(
            "Unsupported viewport schema version."
        );

    constexpr std::uint64_t viewport_weight = 100;
    constexpr std::uint64_t automaton_weight = 900;

    const auto viewport_progress = progress.subrange(
        0,
        viewport_weight
    );

    const auto automaton_progress = progress.subrange(
        viewport_weight,
        automaton_weight
    );

    viewport_progress.report(
        0,
        1,
        "Restoring viewport settings"
    );

    const std::string name =
        document.at("name").get<std::string>();

    if (name.empty())
        throw std::invalid_argument(
            "Viewport name cannot be empty."
        );

    const auto& camera_document = document.at("camera");

    const math::vec2f camera_position =
        load_vec2(camera_document.at("position"));

    const float zoom =
        camera_document.at("zoom").get<float>();

    if (!std::isfinite(zoom) || zoom < 0.1f || zoom > 10.0f)
        throw std::invalid_argument(
            "Camera zoom must be between 0.1 and 10."
        );

    auto render_config =
        load_render_config(document.at("render_config"));

    auto preview_render_config =
        load_render_config(document.at("preview_render_config"));

    const json history_document =
        document.contains("history") && document["history"].is_object()
        ? document["history"]
        : json::object();

    auto population_history =
        load_history_member(history_document, "population");

    auto density_history =
        load_history_member(history_document, "density");

    auto activity_history =
        load_history_member(history_document, "activity");

    cas::viewports::viewport target;

    target.set_name(name);

    target.camera().set_position(camera_position);
    target.camera().set_zoom(zoom);

    target.config() = std::move(render_config);
    target.preview_config() = std::move(preview_render_config);

    target.population_history().assign(population_history);
    target.density_history().assign(density_history);
    target.activity_history().assign(activity_history);

    viewport_progress.complete(
        "Viewport settings restored"
    );

    auto loaded_automaton = load_automaton(
        document.at("simulation"),
        automaton_progress
    );

    const auto linear_width =
        loaded_automaton.storage().current().size();

    const auto max_linear_history_size =
        linear_width *
        cas::viewports::linear_history::max_generations;

    auto linear_history = load_linear_history(
        history_document.contains("linear")
        ? history_document.at("linear")
        : json{},
        max_linear_history_size
    );

    target.simulation().automaton() =
        std::move(loaded_automaton);

    if (!target.linear_history_data().restore(
        target.simulation().automaton(),
        linear_history
    )) {
        target.linear_history_data().clear();
    }

    progress.complete("Viewport restored");

    return target;
}

void save_viewport_file(
    const std::filesystem::path& path,
    const viewport_snapshot& snapshot
) {
    std::ofstream file(path);

    if (!file) {
        throw std::runtime_error(
            "Could not open viewport file for writing: " +
            path.string()
        );
    }

    file << save_viewport(snapshot).dump(2) << '\n';

    if (!file) {
        throw std::runtime_error(
            "Could not write viewport file: " + path.string()
        );
    }
}

void save_viewport_file(
    const std::filesystem::path& path,
    const cas::viewports::viewport& value
) {
    save_viewport_file(path, capture_viewport(value));
}


cas::viewports::viewport load_viewport_file(
    const std::filesystem::path& path,
    progress_callback on_progress
) {
    progress_reporter progress(
        std::move(on_progress),
        1000
    );

    constexpr std::uint64_t read_weight = 200;
    constexpr std::uint64_t deserialize_weight = 800;

    const auto read_progress = progress.subrange(
        0,
        read_weight
    );

    const auto deserialize_progress = progress.subrange(
        read_weight,
        deserialize_weight
    );

    const auto document = read_json_file(
        path,
        [&read_progress](
            std::uint64_t completed,
            std::uint64_t total
            ) {
                read_progress.report(
                    completed,
                    total,
                    "Reading viewport file"
                );
        }
    );

    read_progress.complete("Viewport file read");

    auto result = load_viewport(
        document,
        deserialize_progress
    );

    progress.complete("Viewport loaded");

    return result;
}


} // namespace cas::files::serialization