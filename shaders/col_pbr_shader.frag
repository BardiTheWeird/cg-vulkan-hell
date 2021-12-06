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
    vec4 cameraPosition;
    GlobalUboParameters parameters;
    LightSource lightSources[MAX_LIGHT_SOURCES];
} ubo;

layout (set = 1, binding = 0) uniform MaterialUbo {
    vec4 albedoReflectanceRoughnessMetallic; // albedo at .x; reflectance at .y; roughness at w; metallic at .w
    vec4 emissivityMesh; // .w is brightness
    ivec4 parameters; // .x is ignoreLighting; .yzw are empty
} material;

layout(push_constant) uniform Push {
    mat4 modelMatrix; 
    mat4 normalMatrix; 
} push;


const float PI = 3.141592653589793;

float D(float alpha, vec3 N, vec3 H) {
    float numerator = alpha * alpha;

    float NdotH = max(dot(N,H), 0.0);
    float denominator = PI * pow(pow(NdotH, 2.0) * (alpha * alpha - 1.0) + 1.0, 2.0);
    denominator = max(denominator, 0.000001);

    return numerator / denominator;
}

float G1(float alpha, vec3 N, vec3 X) {
    float numerator = max(dot(N, X), 0.0);
    float k = alpha / 2.0;
    float denominator = max(dot(N, X), 0.0) * (1.0 - k) + k;
    denominator = max(denominator, 0.000001);

    return numerator / denominator;
}

float G(float alpha, vec3 N, vec3 V, vec3 L) {
    return G1(alpha, N, V) * G1(alpha, N, L);
}

vec3 F(vec3 F0, vec3 V, vec3 H) {
    return F0 + (vec3(1.0) - F0) * pow(1 - max(dot(V, H), 0.0), 5.0);
}

vec3 PBR(vec3 N, vec3 V, vec3 L, vec3 H, vec3 F0, vec3 albedo, float alpha, vec3 lightColor, vec3 meshColor) {
    vec3 Ks = F(F0, V, H);
    vec3 Kd = vec3(1.0) - Ks;

    vec3 lambert = albedo / PI;

    vec3 cookTorranceNumerator = D(alpha, N, H) * G(alpha, N, V, L) * F(F0, V, H);
    float cookTorranceDenominator = 4.0 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0);
    cookTorranceDenominator = max(cookTorranceDenominator, 0.000001);
    vec3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

    vec3 BRDF = Kd * lambert * meshColor + cookTorrance;
    vec3 outgoingLight = BRDF * lightColor * max(dot(L, N), 0.0);

    return outgoingLight;
}

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float AddedSaturationCurve(float x) {
    return -0.8 * x * x + 0.8 * x;
}

vec3 PostProcessing(vec3 inColor) {
    vec3 hsv = rgb2hsv(inColor);
    hsv.y = min(hsv.y + AddedSaturationCurve(hsv.y), 1.0);
    vec3 rgb = hsv2rgb(hsv);

    return rgb;
}

void main() {
    if (material.parameters.x == 1) {
        outColor = vec4(fragColor, 1.0);
        return;
    }

    vec3 albedo = vec3(material.albedoReflectanceRoughnessMetallic.x);
    vec3 baseReflectance = vec3(material.albedoReflectanceRoughnessMetallic.y);
    float roughness = material.albedoReflectanceRoughnessMetallic.z;
    float metallicCoefficient = material.albedoReflectanceRoughnessMetallic.w;
    vec3 emissivity = material.emissivityMesh.xyz * material.emissivityMesh.w;

    vec3 meshColor = fragColor;

    // Main vectors
    vec3 N = normalize(fragNormalWorld);
    vec3 V = normalize(ubo.cameraPosition.xyz - fragPosWorld);

    // baseReflectance = vec3(0.3);
    // albedo = vec3(2.0);
    // roughness = 0.1;
    float alpha = pow(roughness, 2.0);

    vec3 PBR_SUM = vec3(0.0);

    for (int i = 0; i < ubo.parameters.lightSourceCount; i++) {
        LightSource lightSource = ubo.lightSources[i];
        vec3 lightColor = lightSource.color.xyz * lightSource.color.w;

        vec3 L;

        if (lightSource.parameters.kind == DirectionalLight) {
            L = lightSource.value1.xyz;            
        }
        else if (lightSource.parameters.kind == PointLight) {
            vec3 lightPosition = lightSource.value1.xyz;
            L = lightPosition - fragPosWorld;

            float attenuation = 1.0 / dot(L, L);
            lightColor = lightColor * attenuation;
        }

        L = normalize(L);
        vec3 H = normalize(V + L);

        vec3 PBR_PER_LIGHT = PBR(N, V, L, H, baseReflectance, albedo, alpha, lightColor, meshColor);
        PBR_SUM = PBR_SUM + PBR_PER_LIGHT;
    }

    vec3 ambientLight = ubo.ambientColor.xyz * ubo.ambientColor.w;
    vec3 raw_ish_color = emissivity + PBR_SUM + ambientLight;

    outColor = vec4(PostProcessing(raw_ish_color), 1.0);
}