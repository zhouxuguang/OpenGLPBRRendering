#version 330 core

out vec4 OutColor0;
in vec3 V_Texcoord;
uniform sampler2D U_Texture;

const PI = 3.1415926;

vec2 CubeMapTexcoord2Texture2DTexcoord(vec3 inTexcoord)
{
    //float fracZX = inTexcoord.z/inTexcoord.x;
    vec2 texcoord = vec2(atan(inTexcoord.z, inTexcoord.x), asin(inTexcoord.y));
    texcoord *= vec2(0.1591549, 0.3183099);
    texcoord += vec2(0.5);
    return texcoord;
}
void main()
{
    vec2 uv = CubeMapTexcoord2Texture2DTexcoord(normalize(V_Texcoord));
    vec3 hdrColor = texture(U_Texture,uv).rgb;
    OutColor0 = vec4(hdrColor, 1.0);
}