precision mediump float;
precision mediump sampler2D;

layout (location = 0) out vec4 colorOut;

in float v_light;
in vec3 v_position;
flat in vec3 v_normal;
flat in vec2 v_spriteSheetOffset;

uniform sampler2D spriteSheet;
uniform vec3 hoveredTilePos;
uniform vec3 hoveredTileNormal;

void main()
{
    bool isHoveredTile = all(greaterThanEqual(v_position, hoveredTilePos))
        && all(lessThanEqual(v_position, hoveredTilePos + vec3(1.0f, TILE_HEIGHT, 1.0f)));

    float light = v_light;

    const float gridLightReduction = 0.8f;
    const float gridLightMultiplier = 1.05f;

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
    else
    {
        if (mod(v_position.x, 1.0f) > 1.0f - halfBorderWidth * 3.0f && abs(v_normal.x) < 0.99f)
        {
            light *= gridLightMultiplier;
        }
        else if (mod(v_position.z, 1.0f) > 1.0f - halfBorderWidth * 3.0f && abs(v_normal.z) < 0.99f)
        {
            light *= gridLightMultiplier;
        }
    }

    ivec2 texelCoords = ivec2(v_spriteSheetOffset);
    texelCoords.x += int(mod(v_position.x, 1.0f) * 64.0f);
    texelCoords.y += int(mod(v_position.z, 1.0f) * 64.0f);

    vec3 tileTextureColor = texelFetch(spriteSheet, texelCoords, 0).rgb;

    colorOut = vec4(tileTextureColor * vec3(light), 1.0);


    if (isHoveredTile && dot(v_normal, hoveredTileNormal) >= 0.99f)
    {
        colorOut.rgb = mix(vec3(1.0f), colorOut.rgb, 0.5f);
    }
}
