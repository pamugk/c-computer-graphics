#version 460

in vec3 v_color;

layout(location = 0) out vec4 o_color;

void main()
{
   o_color = vec4(sin(v_color[0] * 10 * 3.1415926) * 0.5 + 0.5, 0.0, 0.0, 1.0);
}