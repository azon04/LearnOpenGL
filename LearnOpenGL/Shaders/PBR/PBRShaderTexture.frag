#version 330
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

uniform vec3 camPos;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metalicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

const float PI = 3.14159265359;

vec3 fresnelShlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow((1.0 - cosTheta), 5);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N,H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1 - k) + k;

    return num/denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N,V), 0.0);
    float NdotL = max(dot(N,L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Easy trick to get tangent-normal to world-space
vec3 getNormalFromMap()
{
    // transform normal vector to range [-1,1]
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1 = dFdx(WorldPos);
    vec3 Q2 = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2)); // albedo usually using sRGB when authored
    vec3 normal = getNormalFromMap();
    float metalic = texture(metalicMap, TexCoords).r;
    float roughness = texture(roughnessMap, TexCoords).r;
    float ao = texture(aoMap, TexCoords).r;

    vec3 N = normalize(Normal);
    vec3 V = normalize(camPos - WorldPos);

    vec3 Lo = vec3(0.0);
    for(int i=0; i < 4; i++)
    {
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L); // halfway distance between view vector and Light distance

        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, metalic);
        vec3 F = fresnelShlick(max(dot(H, V), 0.0), F0);

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);

        vec3 numerator = NDF * G * F;
        float denominator = 4 * max(dot(N,V), 0.0) * max(dot(N,L), 0.0);
        vec3 specular = numerator / max(denominator, 0.001);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metalic;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0)); // tone or exposure map the HDR to LDR; using Reinhard
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}