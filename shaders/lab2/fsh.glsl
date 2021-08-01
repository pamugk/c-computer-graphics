#version 450

uniform sampler2DArray u_surfaceMap; // Текстуры поверхности

in vec2 v_texCoord;
flat in int v_texNum;

layout(location = 0) out vec4 o_color;

void main()
{
   o_color = texture(u_surfaceMap, vec3(v_texCoord, v_texNum));
}
