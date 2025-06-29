#version 330 core
in vec3 V_Texcoord;
out vec4 OutColor0;
uniform samplerCube U_SkyBox;

void main()
{
    vec3 texcoord = normalize(V_Texcoord);
    vec3 hdrColor = texture(U_SkyBox, texcoord).rgb;
    OutColor0 = vec4(hdrColor, 1.0);
}
