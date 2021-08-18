#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in int a_material;
layout(location = 3) in vec2 a_texCoord;

uniform mat4 u_mvp; // MVP-матрица
uniform mat3 u_n; // Матрица нормалей

out vec3 v_pos;
out vec3 v_normal;
out flat int i_material;
out vec2 v_texCoord;

void main()
{
    v_texCoord = a_texCoord;
    v_normal = normalize(u_n * a_normal);
    i_material = a_material;
    v_pos = a_position;
    gl_Position = u_mvp * vec4(a_position, 1.0);
}
