#version 330 core
in vec4 position;

uniform DefaultVertexMatrixs{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    mat4 ModelMatrix;
    mat4 Reseved[1021];
};
out vec3 V_Texcoord;
void main(){
    V_Texcoord=position.xyz;
    gl_Position=ProjectionMatrix*ViewMatrix*ModelMatrix*position;
}