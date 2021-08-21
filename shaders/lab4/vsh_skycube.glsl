#version 450

layout(location = 0) in vec3 a_position;

uniform mat4 u_mvp;
out vec3 v_texCoord;

void main() {
    v_texCoord = vec3(-a_position.x, a_position.y, -a_position.z);
    gl_Position = u_mvp * vec4(a_position, 1.0);
}
