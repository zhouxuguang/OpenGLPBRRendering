#version 330 core
const float PI = 3.1415926;
uniform samplerCube U_SkyBox;
in vec3 V_Texcoord;
out vec4 OutColor0;

uniform DefaultVec4s
{
    vec4 U_Setting;
    vec4 Reserved[1023];
};

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

void main()
{
    vec3 prefilteredColor = vec3(0.0);
    float roughness = U_Setting.x;
    vec3 texcoord = normalize(V_Texcoord);
    vec3 N = texcoord;
    vec3 V = N;
    const uint sampleCount = 1000u;
    float weight = 0.0;
    for (uint i = 0u; i < sampleCount; i ++)
    {
        vec2 Xi = HammersleyPoint(i, sampleCount);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness); //D
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            prefilteredColor += texture(U_SkyBox, texcoord).rgb * NdotL;
            weight += NdotL;
        }
    }

    prefilteredColor /= weight;

    OutColor0 = vec4(prefilteredColor, 1.0);
}