#version 330 core

in vec4 V_Texcoord;
in vec4 V_Normal;
in vec3 V_WorldPos;

uniform samplerCube U_PrefilteredColor;
uniform samplerCube U_DiffuseIrradiance;
uniform sampler2D U_BRDFLUT;

uniform DefaultVec4s
{
    vec4 CameraWorldPosition;
    vec4 Reserved[1023];
};

const float PI = 3.1415926;

out vec4 OutColor0;

vec3 Fresnel(in vec3 inF0, in float HdotV)
{
    return inF0 + (vec3(1.0) - inF0) * pow(1-HdotV, 5.0);
}

// 间接光使用的版本
vec3 fresnelSchlickRoughness(vec3 F0, float cosTheta, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float NDF(float NdotH, float inRoughness)
{
    float roughness = inRoughness * inRoughness;
    float numerator = roughness * roughness;
    float denominator = (NdotH * NdotH) * (numerator - 1.0) + 1.0;
    denominator = PI * denominator * denominator;
    return numerator / denominator;
}

float Geometry(float NdotV, float inRoughness)
{
    float K = (inRoughness + 1.0) * (inRoughness + 1.0) / 8.0;

    return NdotV / (NdotV * (1.0 - K) + K);
}

void main()
{
    vec3 N = normalize(V_Normal.xyz);
    vec3 L = normalize(vec3(1.0, 1.0, 1.0));
    vec3 V = normalize(CameraWorldPosition.xyz - V_WorldPos); //surface -> eye
    vec3 H = normalize(L+V);
    vec3 R = normalize(2.0 * dot(V, N) * N - V);
    float NdotL = max(dot(N, L), 0.0);
    float HdotV = max(dot(H, V), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    float lightIntensity = 4.0;
    float attenuation = 1.0;

    vec3 F0 = vec3(0.04);
    vec3 albedo = vec3(0.0, 1.0, 0.0);
    float inRoughness = 0.1;  // 0.0 0.1 0.2 
    float metallic = 0.1;  //0.0 0.1 0.2

    F0 = mix(F0, albedo, metallic);

    vec3 FinalColor = vec3(0.0);
    //direct light
    {
        vec3 Ks = Fresnel(vec3(0.04), HdotV);
        float D = NDF(NdotH, inRoughness);
        float Gv = Geometry(NdotV, inRoughness);
        float Gl = Geometry(NdotL, inRoughness);
        vec3 specular = (D * Ks * Gv * Gl) / (4.0 * NdotL * NdotV + 0.0001);

        vec3 Kd = vec3(1.0) - Ks;
        Kd *= (1.0 - metallic);
        vec3 diffuse = Kd * albedo * PI;
        FinalColor = (diffuse + specular) * lightColor * lightIntensity * attenuation * NdotL;
    }

    {
        // IBL 部分

        vec3 Ks = fresnelSchlickRoughness(F0, NdotV, inRoughness);
        vec3 Kd = vec3(1.0) - Ks;
        Kd *= (1.0 - metallic);
        vec3 diffuseIrradiance = texture(U_DiffuseIrradiance, N).rgb;
        vec3 ambientDiffuse = Kd * diffuseIrradiance * albedo;

        vec2 brdf = texture(U_BRDFLUT, vec2(NdotV, inRoughness)).rg;
        vec3 prefilteredColor = textureLod(U_PrefilteredColor, R, inRoughness * 4.0).rgb;
        vec3 ambientSpecular = prefilteredColor * (F0 * brdf.x + brdf.y);

        vec3 ambinetColor = ambientDiffuse + ambientSpecular;
        FinalColor += ambinetColor;
    }

    OutColor0 = vec4(FinalColor, 1.0);
}