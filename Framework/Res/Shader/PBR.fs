#version 330 core

in vec4 V_Texcoord;
in vec4 V_Normal;
in vec3 V_WorldPos;

out vec4 OutColor0;

void main()
{
    vec3 N = normalize(V_Normal.xyz);
    vec3 L = vec3(0.0, 1.0, 0.0);
    float NDotL = max(dot(N,L), 0.0);
    OutColor0 = vec4(NDotL, NDotL, NDotL, 1.0);
}