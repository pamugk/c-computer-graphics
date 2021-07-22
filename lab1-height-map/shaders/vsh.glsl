#version 460

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;

uniform mat4 u_mvp;

out vec3 v_color;

void main()
{
    v_color = a_color;
    gl_Position = vec4(a_position, 1.0);
}
