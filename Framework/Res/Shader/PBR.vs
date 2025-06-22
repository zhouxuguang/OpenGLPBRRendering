#version 330 core

in vec4 position;
in vec4 texcoord;
in vec4 normal;
uniform DefaultVertexMatrixs{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
    mat4 ModelMatrix;
    mat4 ITModelMatrix;
    mat4 Mat4Reserved[1020];
};
out vec4 V_Texcoord;
out vec4 V_Normal;
out vec3 V_WorldPos;

void main()
{
    V_Texcoord = texcoord;
    V_Normal = ITModelMatrix * normal;
    vec3 positionWS = (ModelMatrix * position).xyz;
    V_WorldPos = positionWS;
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(positionWS, 1.0);
}