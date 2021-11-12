#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout (location = 0) out vec3 fragColor; 

#define MAX_LIGHT_SOURCES 128

#define DirectionalLight 0
#define PointLight 1

struct LightSourceParameters {
    int kind;
    int misc1;
    int misc2;
    int misc3;
};

struct LightSource {
    LightSourceParameters parameters;
    vec4 value1; // direction OR point of origin
};

struct UboParameters {
    int lightSourceCount;
    float ambientLighting;
    int misc1;
    int misc2;
};

layout (set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    UboParameters parameters;
    LightSource lightSources[MAX_LIGHT_SOURCES];
    // LightSource lightSource;
} ubo;

layout(push_constant) uniform Push {
    mat4 modelMatrix; 
    mat4 normalMatrix; 
} push;

// const float AMBIENT = 0.02;

void main() {
    gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);

    vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

    float lightIntensity = ubo.parameters.ambientLighting;

    for (int i = 0; i < ubo.parameters.lightSourceCount; i++) {
        LightSource lightSource = ubo.lightSources[i];
    
        if (lightSource.parameters.kind == DirectionalLight) {
            vec3 direction = vec3(lightSource.value1);
    
            float calculationResult = max(dot(normalWorldSpace, direction), 0);
            lightIntensity = lightIntensity + calculationResult;
        }
    }

    fragColor = lightIntensity * color;
}