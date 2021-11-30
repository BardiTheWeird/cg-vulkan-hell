#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout (location = 0) out vec3 fragColor; 
layout (location = 1) out vec3 fragPosWorld;
layout (location = 2) out vec3 fragNormalWorld;
layout (location = 3) out vec2 texCoord;

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
    vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);
    gl_Position = ubo.projectionViewMatrix * positionWorld;

    fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
    fragPosWorld = positionWorld.xyz;
    fragColor = color;

    texCoord = uv;
}