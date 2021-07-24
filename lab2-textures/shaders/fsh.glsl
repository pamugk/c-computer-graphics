#version 460

uniform sampler2D u_map1;
uniform sampler2D u_map2;

in vec2 v_texCoord;
flat in int v_texNum;

layout(location = 0) out vec4 o_color;

void main()
{
   o_color = texture(u_map1, v_texCoord) * (1 - v_texNum) + texture(u_map2, v_texCoord) * (0 + v_texNum);
}
