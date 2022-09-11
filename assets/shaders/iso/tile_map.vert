precision mediump float;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 mvp;

out float v_light;

void main()
{
    gl_Position = mvp * vec4(a_position, 1.0);
    v_light = max(0.1f, (dot(a_normal, vec3(0.5, 1, 0)) + 1.0f) * 0.5f);
}
