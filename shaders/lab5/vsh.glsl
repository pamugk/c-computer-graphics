#version 450

in vec3 a_position;
in vec3 a_normal;
in int a_material;
in vec2 a_texCoord;

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
