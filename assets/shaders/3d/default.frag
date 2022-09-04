// PHONGish shading

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

void calcPointLight(PointLight light, vec3 normal, vec3 viewDir, float roughness, inout vec3 totalDiffuse, inout vec3 totalSpecular, inout vec3 totalAmbient)
{
    vec3 lightDir = normalize(light.position - v_position);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), (1.0f - roughness) * 8.) * (1.0f - roughness);

    // attenuation
    float constant = light.attenuation.x;
    float linear = light.attenuation.y;
    float quadratic = light.attenuation.z;

    float distance    = length(light.position - v_position);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    // combine results
    totalAmbient  += light.color  * attenuation;
    totalDiffuse += light.color  * diff * attenuation;
    totalSpecular  += light.color * spec * attenuation;
}

void calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir, float roughness, inout vec3 totalDiffuse, inout vec3 totalSpecular, inout vec3 totalAmbient, float shadow)
{
    // diffuse shading
    float diff = max(dot(normal, -light.direction), 0.0);

    // specular shading (Blinn-Phong)
    vec3 halfwayDir = normalize(-light.direction + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), (1.0f - roughness) * 8.);

    // combine results
    totalAmbient += light.color;
    totalDiffuse += light.color * diff * (1. - shadow);
    totalSpecular += light.color * spec * (1. - shadow);
}

void main()
{
    vec3 normal = vec3(0, 0, 1);    // normal will be in World space.

    // normal map:
    if (useNormalMap == 1)
    {
        normal = texture(normalMap, v_textureCoord).xyz;
        normal = normal * 2. - 1.;
    }
    normal = normalize(v_TBN * normal);

    vec3 diffuseColor = diffuse;
    if (useDiffuseTexture == 1)
    {
        diffuseColor = texture(diffuseTexture, v_textureCoord).rgb;
    }

    colorOut.rgb = diffuseColor;

    float metallic = metallicRoughnessFactors.x;
    float roughness = metallicRoughnessFactors.y;
    if (useMetallicRoughnessTexture == 1)
    {
        vec2 mr = texture(metallicRoughnessTexture, v_textureCoord).xy;
        metallic = mr.x;
        roughness = mr.y;
    }

    vec3 totalDiffuseLight = vec3(0);
    vec3 totalSpecularLight = vec3(0);
    vec3 totalAmbientLight = vec3(0);

    vec3 viewDir = normalize(camPosition - v_position);

    #if NR_OF_POINT_LIGHTS
    {   // Light points

        for (int i = 0; i < NR_OF_POINT_LIGHTS; i++)
            calcPointLight(pointLights[i], normal, viewDir, roughness, totalDiffuseLight, totalSpecularLight, totalAmbientLight);
    }
    #endif

    #if NR_OF_DIR_LIGHTS
    {   // Directional lights

        for (int i = 0; i < NR_OF_DIR_LIGHTS; i++)
            calcDirLight(dirLights[i], normal, viewDir, roughness, totalDiffuseLight, totalSpecularLight, totalAmbientLight, 0.);
    }
    #endif

    // diffuse & ambient:
    colorOut.rgb *= vec3(max(totalAmbientLight.r, totalDiffuseLight.r), max(totalAmbientLight.g, totalDiffuseLight.g), max(totalAmbientLight.b, totalDiffuseLight.b));

    // specularity:
    vec3 specularColor = mix(vec3(1.0f), diffuseColor, metallic);
    colorOut.rgb += specularColor * totalSpecularLight;
    colorOut.a = 1.0f;
}

