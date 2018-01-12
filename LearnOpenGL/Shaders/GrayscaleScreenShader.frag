#version 330 core
in vec2 TexCoord;

out vec4 color;

uniform sampler2D inTexture;

void main() {
	color = vec4(vec3(texture(inTexture, TexCoord)), 1.0f);
	float average = 0.2126 * color.r + 0.7152 * color.g +  0.0722 * color.b;
	color = vec4(average, average, average, 1.0f);
}