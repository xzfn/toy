#version 450


layout(location = 0) in vec3 v_Color;

layout(location = 0) out vec4 out_Color;

void main() {
	vec4 base_color = vec4(v_Color, 1.0f);
	out_Color = base_color;
}
