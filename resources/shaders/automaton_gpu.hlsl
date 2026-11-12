
struct vertex_output
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

vertex_output vertex_main(uint vertex_id : SV_VertexID)
{
    vertex_output output;

    float2 positions[3] = {
        float2(-1.0, -1.0),
        float2(-1.0,  3.0),
        float2( 3.0, -1.0)
    };

    float2 position = positions[vertex_id];

    output.position = float4(position, 0.0, 1.0);
    output.uv = float2(
        position.x * 0.5 + 0.5,
        0.5 - position.y * 0.5
    );

    return output;
}

Texture2D<float> state_texture : register(t0, space2);
SamplerState state_sampler : register(s0, space2);

cbuffer render_config : register(b0, space3)
{
    float2 viewport_size;
    float2 camera_position;

    float zoom;
    float cell_size;
    float2 world_offset;

    int grid_width;
    int grid_height;

    float4 background_color;
    float4 grid_color;
    float4 alive_color;
    float4 persistent_color;
};

float2 hex_to_world(int q, int r)
{
    return float2(
        cell_size * 1.7320508075688772 * (q + r * 0.5)
            + world_offset.x,
        cell_size * 1.5 * r + world_offset.y
    );
}

int2 world_to_hex(float2 world)
{
    float2 p = world - world_offset;

    float q = (
        1.7320508075688772 * p.x / 3.0 - p.y / 3.0
    ) / cell_size;

    float r = (2.0 * p.y / 3.0) / cell_size;

    float x = q;
    float z = r;
    float y = -x - z;

    float rx = round(x);
    float ry = round(y);
    float rz = round(z);

    float dx = abs(rx - x);
    float dy = abs(ry - y);
    float dz = abs(rz - z);

    if (dx > dy && dx > dz)
        rx = -ry - rz;
    else if (dy > dz)
        ry = -rx - rz;
    else
        rz = -rx - ry;

    return int2((int)rx, (int)rz);
}

float distance_to_segment(float2 p, float2 a, float2 b)
{
    float2 ab = b - a;
    float denominator = dot(ab, ab);

    float t = denominator > 0.0
        ? saturate(dot(p - a, ab) / denominator)
        : 0.0;

    return length(p - (a + t * ab));
}

float distance_to_hex_edge(float2 p, float size)
{
    const float2 vertices[6] = {
        float2( 0.8660254038, -0.5),
        float2( 0.0,          -1.0),
        float2(-0.8660254038, -0.5),
        float2(-0.8660254038,  0.5),
        float2( 0.0,           1.0),
        float2( 0.8660254038,  0.5)
    };

    float distance = 1e20;

    [unroll]
    for (int i = 0; i < 6; ++i)
    {
        float2 a = vertices[i] * size;
        float2 b = vertices[(i + 1) % 6] * size;

        distance = min(
            distance,
            distance_to_segment(p, a, b)
        );
    }

    return distance;
}

float4 fragment_main(vertex_output input) : SV_Target0
{
    // Coordenadas del centro del píxel en espacio de pantalla.
    float2 screen = input.uv * viewport_size;

    // Inversa de camera.world_to_screen().
    float2 world =
        (screen - viewport_size * 0.5) / zoom
        + camera_position;

    int2 axial = world_to_hex(world);

    float2 center_world = hex_to_world(axial.x, axial.y);
    float2 center_screen =
        (center_world - camera_position) * zoom
        + viewport_size * 0.5;

    float2 local = screen - center_screen;
    float size = cell_size * zoom;

    // Reproducir la rasterización de los hexágonos rellenos.
    float abs_y = abs(local.y);

    float half_width = abs_y <= size * 0.5
        ? 0.8660254038 * size
        : 1.7320508075688772 * (size - abs_y);

    bool inside_hex =
        abs_y <= size &&
        abs(local.x) <= half_width;

    // La cuadrícula se dibuja primero y las celdas la cubren.
    // La textura guarda cada estado como un byte normalizado.
    int2 cell = axial + int2(grid_width / 2, grid_height / 2);

    bool in_grid =
        cell.x >= 0 && cell.x < grid_width &&
        cell.y >= 0 && cell.y < grid_height;

    float state_value = 0.0;

    float2 state_uv =
        (float2(cell) + 0.5) /
        float2(grid_width, grid_height);

    if (in_grid)
    {
        state_value = state_texture.SampleLevel(
        state_sampler,
        state_uv,
        0.0
    );
    }

    int state = (int)round(state_value * 255.0);

    if (state != 0 && in_grid && inside_hex)
        return state == 1 ? alive_color : persistent_color;

    // El grosor se expresa en píxeles de pantalla, no del mundo.
    float edge_distance = distance_to_hex_edge(local, size);

    if (edge_distance <= 0.5)
        return grid_color;

    return background_color;
}
