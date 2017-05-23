#version 330 core

in vec4 vertexColor; // The input variable from vertex shader (same name and same type)

out vec4 color;

uniform vec4 ourColor; // We set this variable in OpenGL Code

void main()
{
	color = vertexColor;
}