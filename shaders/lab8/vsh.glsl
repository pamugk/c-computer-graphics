#version 450

in vec3 a_position; // Позиция вершины
in vec3 a_normal; // Нормаль в вершине
in int a_material; // Индекс материала вершины
in vec2 a_texCoord; // Текстурные координаты вершины
in vec3 a_tangent; // Касательная к вершине
 
uniform mat4 u_mvp;
uniform mat4 u_mv;
uniform mat3 u_n;

uniform vec3 u_olpos; // Позиция источника света
uniform vec3 u_oeye; // Позиция наблюдателя

out vec3 v_ray; // Нормированный вектор падения света
out vec3 v_eye; // Ось зрения наблюдателя
out vec2 v_texCoord;
flat out int i_material; // Индекс материала вершины

void main() {
    vec4 pos = vec4(a position, 1.0);
    vec3 binormal = normalize(cross(a_normal, a_tangent));
    mat3 intbn = inverse(u_n * mat3(a_tangent, binormal, a_normal));
    vec3 vPos = intbn * (u_mv * pos).xyz;
    v_ray = normalize(intbn * vec4(u_olpos, 1.0).xyz − vPos);
    v_eye = normalize(intbn * vec4(u_oeye, 1.0).xyz − vPos);
    v_texCoord = a_texCoord;
    i_material = a_material;
    gl_Position = u_mvp * pos;
}
