precision mediump float;

layout (location = 0) out vec4 colorOut;

in float v_light;
in vec3 v_position;
flat in vec3 v_normal;

void main()
{
    float light = v_light;

    const float gridLightReduction = 0.85f;

    if (mod(v_position.x, 1.0f) < 1.0f / (float(PIXELS_PER_METER) * 0.75f) && abs(v_normal.x) < 0.99f)
    {
        light *= gridLightReduction;
    }
    else if (mod(v_position.y, TILE_HEIGHT) < 1.0f / (float(PIXELS_PER_METER) * 0.75f) && abs(v_normal.y) < 0.99f)
    {
        light *= gridLightReduction;
    }
    else if (mod(v_position.z, 1.0f) < 1.0f / (float(PIXELS_PER_METER) * 0.75f) && abs(v_normal.z) < 0.99f)
    {
        light *= gridLightReduction;
    }

    colorOut = vec4(0.4 * light, 0.6 * light, 1.0 * light, 1.0);

}
