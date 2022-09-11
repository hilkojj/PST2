precision mediump float;

layout (location = 0) out vec4 colorOut;

in float v_light;

void main()
{
    colorOut = vec4(0.4 * v_light, 0.6 * v_light, 1.0 * v_light, 1.0);
}
