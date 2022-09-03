precision mediump float;
precision mediump sampler2DShadow;

#define PI 3.14159265359

struct PointLight
{
    vec3 position;

//    float constant;
//    float linear;
//    float quadratic;
    vec3 attenuation; // x: constant, y: linear, z: quadratic

    vec3 color;
};

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
};

in vec3 v_position;
in vec2 v_textureCoord;
in mat3 v_TBN;

layout (location = 0) out vec4 colorOut;

uniform float time;

uniform vec3 diffuse;
uniform vec2 metallicRoughnessFactors;

uniform int useDiffuseTexture;
uniform sampler2D diffuseTexture;

uniform int useMetallicRoughnessTexture;
uniform sampler2D metallicRoughnessTexture;

uniform int useNormalMap;
uniform sampler2D normalMap;

uniform vec3 camPosition;

#ifndef NR_OF_DIR_LIGHTS
#define NR_OF_DIR_LIGHTS 0
#endif

#ifndef NR_OF_POINT_LIGHTS
#define NR_OF_POINT_LIGHTS 0
#endif

#if NR_OF_DIR_LIGHTS
uniform DirectionalLight dirLights[NR_OF_DIR_LIGHTS];    // TODO: uniform buffer object?
#endif

#if NR_OF_POINT_LIGHTS
uniform PointLight pointLights[NR_OF_POINT_LIGHTS];    // TODO: uniform buffer object?
#endif


void main()
{
    colorOut = vec4(1.0);
}

