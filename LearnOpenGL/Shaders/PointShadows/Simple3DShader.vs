#version 330 core
layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 texCoords; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform bool reverse_normals;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	FragPos = vec3( model * vec4(position, 1.0f));
	if( reverse_normals )
		Normal = mat3(transpose(inverse(model))) * (-1.0f * normal);
	else
		Normal = mat3(transpose(inverse(model))) * normal;
	TexCoords = texCoords;
}