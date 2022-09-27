precision mediump float;
precision mediump sampler2D;

layout (location = 0) out vec4 colorOut;

in vec2 v_texCoords;

uniform sampler2D spriteSheet;
uniform sampler2D spriteSheetDepth;

void main()
{
    ivec2 texelCoords = ivec2(v_texCoords);

    colorOut = texelFetch(spriteSheet, texelCoords, 0);

    float spriteDepth = texelFetch(spriteSheetDepth, texelCoords, 0).r;

    gl_FragDepth = gl_FragCoord.z + (spriteDepth - 0.5f);

    if (colorOut.a < 0.01f)
    {
        discard;
    }
}
