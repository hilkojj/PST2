precision mediump float;

layout(location = 0) in vec2 a_quadPosition;

// per instance:
layout(location = 1) in vec3 a_position;
layout(location = 2) in vec2 a_size;
layout(location = 3) in vec2 a_spriteSheetOffset;

uniform mat4 projection;
uniform vec3 cameraUpDirection;
uniform vec3 cameraRightDirection;

out vec2 v_texCoords;

void main()
{
    vec3 vertPos = a_position;
    vertPos += cameraRightDirection * a_quadPosition.x * a_size.x * 0.5f;
    vertPos += cameraUpDirection * a_quadPosition.y * a_size.y * 0.5f;

    gl_Position = projection * vec4(vertPos, 1.0);
    v_texCoords = a_spriteSheetOffset;
    v_texCoords += (a_quadPosition * 0.5f + 0.5f) * a_size * float(PIXELS_PER_METER);
}
