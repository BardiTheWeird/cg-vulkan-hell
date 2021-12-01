#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;


#define MAX_LIGHT_SOURCES 128

#define DirectionalLight 0
#define PointLight 1

struct LightSourceParameters {
    int kind;
    float misc1;
    float misc2;
    float misc3;
};

struct LightSource {
    LightSourceParameters parameters;
    vec4 color; // {r, g, b, intensity}
    vec4 value1; // direction OR point of origin
};

struct GlobalUboParameters {
    int lightSourceCount;
    int unused1;
    int unused2;
    int unused3;
};

layout (set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    vec4 ambientColor;
    GlobalUboParameters parameters;
    LightSource lightSources[MAX_LIGHT_SOURCES];
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix; 
    mat4 normalMatrix; 
} push;


void main() {
    vec3 diffuseLight;
    diffuseLight.x = diffuseLight.y = diffuseLight.z = 0;

    for (int i = 0; i < ubo.parameters.lightSourceCount; i++) {
        LightSource lightSource = ubo.lightSources[i];
        vec3 lightColor = lightSource.color.xyz * lightSource.color.w;

        if (lightSource.parameters.kind == DirectionalLight) {
            vec3 direction = lightSource.value1.xyz;            
            diffuseLight = diffuseLight + lightColor * max(dot(normalize(fragNormalWorld), direction), 0);
        }
        else if (lightSource.parameters.kind == PointLight) {
            vec3 lightPosition = lightSource.value1.xyz;
            vec3 directionToLight = lightPosition - fragPosWorld;

            float attenuation = 1.0 / dot(directionToLight, directionToLight);
            float luminanceAtVert = max(dot(normalize(fragNormalWorld), normalize(directionToLight)), 0);

            diffuseLight = diffuseLight + lightColor * attenuation * luminanceAtVert;
        }
    }

    vec3 ambientLight = ubo.ambientColor.xyz * ubo.ambientColor.w;
    outColor = vec4((diffuseLight + ambientLight) * fragColor, 1.0);
    // outColor = vec4(1.0,1.0,1.0,1.0);
}