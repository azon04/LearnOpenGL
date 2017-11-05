#version 330 core
#define NUMBER_POINT_LIGHTS 4

out vec4 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

void main()
{
	color = vec4(vec3(gl_FragCoord.z), 1.0f);
}