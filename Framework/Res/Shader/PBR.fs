#version 330 core

in vec4 V_Texcoord;
in vec4 V_Normal;
in vec3 V_WorldPos;

uniform DefaultVec4s
{
    vec4 CameraWorldPosition;
    vec4 Reserved[1023];
};

out vec4 OutColor0;

vec3 Fresnel(in vec3 inF0, in float HdotV)
{
    return inF0 + (vec3(1.0) - inF0) * pow(1-HdotV, 5.0);
}

void main()
{
    vec3 N = normalize(V_Normal.xyz);
    vec3 L = vec3(0.0, 1.0, 0.0);
    vec3 V = normalize(CameraWorldPosition.xyz - V_WorldPos); //surface -> eye
    vec3 H = normalize(L+V);
    float NdotL = max(dot(N, L), 0.0);
    float HdotV = max(dot(H, V), 0.0);

    vec3 FinalColor = vec3(0.0);
    //direct light
    {
        vec3 Ks = Fresnel(vec3(0.04), HdotV);
        vec3 specular = Ks;//vec3(0.0);
        vec3 diffuse = vec3(0.0);
        FinalColor = (diffuse + specular) * NdotL;
    }

    //vec3 FinalColor = vec3(0.0);

    OutColor0 = vec4(FinalColor, 1.0);
}