#version 450

// FIXME 4 is material
layout(set = 4, binding = 1) uniform samplerCube u_Texture;

layout(location = 0) in vec3 v_Texcoord;

layout(location = 0) out vec4 out_Color;


void main() {
	out_Color = texture(u_Texture, v_Texcoord);
}
