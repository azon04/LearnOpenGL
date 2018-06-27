#version 330 core
in vec2 TexCoord;

out vec4 color;

uniform sampler2D inTexture;

void main() {
	float depthValue = texture(inTexture, TexCoord).r;
	color = vec4(vec3(depthValue), 1.0f);
}