#version 450
layout(location = 0) in vec2 Position;
layout(location = 0) out vec2 vUV;

layout(push_constant, std430) uniform Registers
{
    vec2 offset;
    vec2 range;
} registers;

void main()
{
    vec2 uv_range = 0.5 * Position + 0.5;
    vec2 shifted = uv_range * registers.range + registers.offset;
#ifdef SHIFT_QUAD
    vec2 pos = shifted * 2.0 - 1.0;
    gl_Position = vec4(pos, 0.0, 1.0);
#else
    gl_Position = vec4(Position, 0.0, 1.0);
#endif
    vUV = shifted;
}
