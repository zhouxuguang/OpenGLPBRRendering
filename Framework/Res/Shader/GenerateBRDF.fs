#version 330 core
in vec4 V_Texcoord;
out vec4 OutColor0;
const float PI = 3.141592;

float Geometry(float inNDotX, float inRoughness)
{
    float k = pow(inRoughness, 2.0) / 2.0;
    return inNDotX / (inNDotX * (1.0 - k) + k);
}

float RadicalInverse_VDC(uint inIndex) 
{
    inIndex = (inIndex << 16u) | (inIndex >> 16u);
    inIndex = ((inIndex & 0x55555555u) << 1u) | ((inIndex & 0xAAAAAAAAu) >> 1u);
    inIndex = ((inIndex & 0x33333333u) << 2u) | ((inIndex & 0xCCCCCCCCu) >> 2u);
    inIndex = ((inIndex & 0x0F0F0F0Fu) << 4u) | ((inIndex & 0xF0F0F0F0u) >> 4u);
    inIndex = ((inIndex & 0x00FF00FFu) << 8u) | ((inIndex & 0xFF00FF00u) >> 8u);
    return float(inIndex) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 HammersleyPoint(uint index, uint inTotalSampleCount)
{
    return vec2(float(index) / float(inTotalSampleCount), RadicalInverse_VDC(index));
}

vec3 ImportanceSampleGGX(vec2 inXi, vec3 inN, float inRoughness)
{
    float r4=pow(inRoughness,4);
    float phi=2.0*PI*inXi.x;//0->1
    float cosTheta=sqrt((1.0-inXi.y)/(1.0+(r4-1.0)*inXi.y));//D
    float sinTheta=sqrt(1.0-cosTheta*cosTheta);

    vec3 H;
    H.x=sinTheta*cos(phi);
    H.y=sinTheta*sin(phi);
    H.z=cosTheta;

    vec3 temp=abs(inN.z)<0.999?vec3(0.0,0.0,1.0):vec3(1.0,0.0,0.0);
    vec3 X=normalize(cross(temp,inN));
    vec3 Y=cross(inN,X);

    vec3 HFinal=X*H.x+Y*H.y+inN*H.z;
    return HFinal;
}

vec2 GenerateBRDF(float NdotV, float roughness)
{
    vec3 N = vec3(0.0, 0.0, 1.0);
    vec3 V = vec3(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);
    const uint sampleCount = 1024u;

    float A = 0;
    float B = 0;
    for (uint i = 0u; i < sampleCount; i ++)
    {
        vec2 Xi = HammersleyPoint(i, sampleCount);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness); //D
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if (NdotL > 0.0)
        {
            float Gl = Geometry(NdotL, roughness);
            float Gv = Geometry(NdotV, roughness);
            float G = Gl * Gv;
            float G_Vis = G * VdotH / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);
            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }

    return vec2(A, B) / float(sampleCount);
}

void main()
{
    vec2 brdf = GenerateBRDF(V_Texcoord.x, V_Texcoord.y);
    OutColor0 = vec4(brdf, 0.0, 0.0); 
}