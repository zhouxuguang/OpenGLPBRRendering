#version 330 core
out vec4 OutColor0;
in vec3 V_Texcoord;
uniform sampler2D U_Texture;
vec2 CubeMapTexcoord2Texture2DTexcoord(vec3 inTexcoord){
    return vec2(0.0);
}
void main(){
    vec2 uv=CubeMapTexcoord2Texture2DTexcoord(normalize(V_Texcoord));
    vec3 hdrColor=texture(U_Texture,uv).rgb;
    OutColor0=vec4(hdrColor,1.0);
}