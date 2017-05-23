#version 330 core

layout (location = 0) in vec3 position; // The position variable has attribute position 0
layout (location = 1) in vec3 color; // The color variable has attribute position 1

uniform float leftOffset;
out vec4 vertexColor;

void main() {
	gl_Position = vec4(leftOffset + position.x, position.y, position.z, 1.0);
	vertexColor = vec4(color, 1.0);
}