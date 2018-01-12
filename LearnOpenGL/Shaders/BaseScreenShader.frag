#version 330 core
in vec2 TexCoord;

out vec4 color;

uniform sampler2D inTexture;

void main() {
	color = vec4(vec3(texture(inTexture, TexCoord)), 1.0f);
}