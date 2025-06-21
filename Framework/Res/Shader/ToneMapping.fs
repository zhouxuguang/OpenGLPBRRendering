#version 330 core
uniform sampler2D U_Texture; // hdr
in vec4 V_Texcoord;
out vec4 OutColor0;
const float gamma = 2.2;
vec3 GammaEncode(vec3 inColor)
{
    return pow(inColor, vec3(1.0/gamma));
}

void main()
{
    vec3 hdrColor = texture(U_Texture, V_Texcoord.xy).rgb;
    vec3 mappedColor = hdrColor / (hdrColor + vec3(1.0));
    OutColor0 = vec4(GammaEncode(mappedColor), 1.0);
    //OutColor0=vec4(1.0, 0.0, 0.0, 1.0);
}