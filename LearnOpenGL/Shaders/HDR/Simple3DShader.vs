#version 330 core
layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 texCoords; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform bool reverse_normals;

out VS_OUT
{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} vs_out;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	vs_out.FragPos = vec3( model * vec4(position, 1.0f));
	if( reverse_normals )
		vs_out.Normal = mat3(transpose(inverse(model))) * (-1.0f * normal);
	else
		vs_out.Normal = mat3(transpose(inverse(model))) * normal;
	vs_out.TexCoords = texCoords;
}