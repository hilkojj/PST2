precision mediump float;
precision mediump sampler2D;

layout (location = 0) out vec4 colorOut;

in float v_light;
in vec3 v_position;
flat in vec3 v_normal;
flat in vec2 v_spriteSheetOffset;

uniform sampler2D spriteSheet;

void main()
{
    float light = v_light;

    const float gridLightReduction = 0.8f;

    const float halfBorderWidth = 1.0f / float(PIXELS_PER_METER);

    if (mod(v_position.x + halfBorderWidth, 1.0f) < halfBorderWidth * 2.0f && abs(v_normal.x) < 0.99f)
    {
        light *= gridLightReduction;
    }
    else if (mod(v_position.y + halfBorderWidth, TILE_HEIGHT) < halfBorderWidth * 2.0f && abs(v_normal.y) < 0.99f)
    {
        light *= gridLightReduction;
    }
    else if (mod(v_position.z + halfBorderWidth, 1.0f) < halfBorderWidth * 2.0f && abs(v_normal.z) < 0.99f)
    {
        light *= gridLightReduction;
    }

    ivec2 texelCoords = ivec2(v_spriteSheetOffset);
    texelCoords.x += int(mod(v_position.x, 1.0f) * 64.0f);
    texelCoords.y += int(mod(v_position.z, 1.0f) * 64.0f);

    vec3 tileTextureColor = texelFetch(spriteSheet, texelCoords, 0).rgb;

    colorOut = vec4(tileTextureColor * vec3(light), 1.0);

}
