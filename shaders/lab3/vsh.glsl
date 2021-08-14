#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in float a_texNum;
layout(location = 3) in vec3 a_normal;

uniform mat4 u_mvp; // MVP-матрица
uniform mat3 u_n; // Матрица нормалей

out vec2 v_texCoord;
flat out float v_texNum;
out vec3 v_normal;
out vec3 v_pos;

void main()
{
    v_texCoord = a_texCoord;
    v_texNum = a_texNum;
    v_normal = normalize(u_n * a_normal);
    v_pos = a_position;
    gl_Position = u_mvp * vec4(a_position, 1.0);
}
