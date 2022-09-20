precision mediump float;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_spriteSheetOffset;

uniform mat4 mvp;

out float v_light;
out vec3 v_position;
flat out vec3 v_normal;
flat out vec2 v_spriteSheetOffset;

void main()
{
    v_position = a_position;
    v_normal = a_normal;
    v_spriteSheetOffset = a_spriteSheetOffset;
    gl_Position = mvp * vec4(a_position, 1.0);
    v_light = max(0.1f, (dot(a_normal, vec3(0.5, 1, 0)) + 1.0f) * 0.5f);
}
