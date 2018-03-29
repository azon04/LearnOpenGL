// shadertype=glsl
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 offset;

out vec4 vertexColor;

void main()
{
	vec2 pos = aPos * (gl_InstanceID / 100.0);
	gl_Position = vec4(pos + offset, 0.0, 1.0);
	vertexColor = vec4(color, 1.0);
}