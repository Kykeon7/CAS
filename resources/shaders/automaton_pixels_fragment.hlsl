struct vertex_output
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

Texture2D<float> state_texture : register(t0, space2);
SamplerState state_sampler : register(s0, space2);

cbuffer pixel_config : register(b0, space3)
{
    int grid_width;
    int grid_height;
    float2 padding;

    float2 viewport_size;
    float2 camera_position;

    float zoom;
    float cell_size;
    float2 world_offset;

    float4 background_color;
    float4 alive_color;
};

float4 fragment_main(vertex_output input) : SV_Target0
{
    float2 screen = input.uv * viewport_size;

    float2 world =
        (screen - viewport_size * 0.5) / zoom
        + camera_position;

    world -= world_offset;

    float2 grid_position = world / cell_size
        + float2(grid_width, grid_height) * 0.5;

    int2 cell = int2(floor(grid_position));

    if (cell.x < 0 || cell.x >= grid_width ||
        cell.y < 0 || cell.y >= grid_height)
    {
        return background_color;
    }

    float2 state_uv =
        (float2(cell) + 0.5) /
        float2(grid_width, grid_height);

    float state_value = state_texture.SampleLevel(
        state_sampler,
        state_uv,
        0.0
    );

    int state = (int) round(state_value * 255.0);

    return state != 0
        ? alive_color
        : background_color;
}