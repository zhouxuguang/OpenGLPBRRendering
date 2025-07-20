#version 330 core

in vec4 position;
in vec4 texcoord;
in vec4 normal;
in vec4 tangent;
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
out mat3 V_TBN;

void main()
{
    V_Texcoord = texcoord;
    V_Normal = ITModelMatrix * normal;
    vec3 positionWS = (ModelMatrix * position).xyz;
    V_WorldPos = positionWS;

    // 世界空间的tbn矩阵构造
    vec3 t = normalize(vec3(ModelMatrix * vec4(tangent.xyz, 0.0)));
    vec3 n = normalize(V_Normal.xyz);
    vec3 b = cross(n, t);
    V_TBN = mat3(t, b, n);
    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(positionWS, 1.0);
}