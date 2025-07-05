#version 330 core
const float PI = 3.1415926;
uniform samplerCube U_SkyBox;
in vec3 V_Texcoord;
out vec4 OutColor0;

void main()
{
    vec3 N = normalize(V_Texcoord);
    vec3 Y = vec3(0.0, 1.0, 0.0);
    vec3 X = normalize(cross(Y, N));
    Y = normalize(cross(N, X));
    vec3 precomputedLight = vec3(0.0);
    float sampleCount = 0.0;
    float step1 = 2.0 * PI / 1000.0;
    float step2 = 0.5 * PI / 250.0;

    //begin
    for (float phi = 0.0; phi < 2.0 * PI; phi += step1)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += step2)
        {
            vec3 localL = vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));  //局部坐标系的坐标
            vec3 L = localL.x*X + localL.y*Y + localL.z*N;  //世界坐标

            precomputedLight += texture(U_SkyBox, L).rgb * cos(theta) * sin(theta);
            sampleCount++;
        }
    }

    //end
    precomputedLight = precomputedLight * PI * (1.0 / sampleCount);
    OutColor0 = vec4(precomputedLight, 1.0);
}