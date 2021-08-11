#version 330
 
uniform mat4 u_mvp;
uniform mat4 u_mv;
uniform mat3 u_n;
uniform vec3 u_light;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;
in vec3 a_tangent;

out vec3 v_ray;
out vec3 v_eye;
out vec2 v_texCoord;

void main() {
    vec4 pos = vec4(a position, 1.0);
    vec3 binormal = normalize(cross(a normal, a tangent));
    mat3 intbn = inverse(u n ∗ mat3(a tangent, binormal, a_normal));
    vec3 vPos = intbn ∗ (u mv ∗ pos).xyz;
    v_ray = normalize(intbn ∗ vec4(u light, 1.0).xyz − vPos);
    v_eye = normalize(−vPos);
    v_texCoord = a texCoord;
    gl_Position = u mvp ∗ pos;
}
