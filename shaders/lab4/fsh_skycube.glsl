#version 450

uniform samplerCube u_map;
in vec3 v_texCoord;
layout (location = 0) out vec4 o_fragColor;

void main() {
    o_fragColor = texture(u_map, v_texCoord);
}
