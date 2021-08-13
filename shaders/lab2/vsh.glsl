#version 450

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in float a_texNum;

uniform mat4 u_mvp;

out vec2 v_texCoord;
flat out float v_texNum;

void main()
{
    v_texCoord = a_texCoord;
    v_texNum = a_texNum;
    gl_Position = u_mvp * vec4(a_position, 1.0);
}
