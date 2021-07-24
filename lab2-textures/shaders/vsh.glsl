#version 460

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_texCoord;
//layout(location = 2) in int a_texNum;

uniform mat4 u_mvp;

out vec2 v_texCoord;
flat out int v_texNum;

void main()
{
    v_texCoord = a_texCoord;
    v_texNum = int(a_position.y >= 0.01);
    gl_Position = u_mvp * vec4(a_position, 1.0);
}
