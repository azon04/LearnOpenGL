#version 330 core
layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 texCoords;
layout (location=3) in vec3 tangent;
layout (location=4) in vec3 bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;
out vec3 TangentFragPos;
out vec3 TangentViewPos;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	FragPos = vec3( model * vec4(position, 1.0f));
	Normal = mat3(transpose(inverse(model))) * normal;
	TexCoords = texCoords; 

	vec3 T = mat3(transpose(inverse(model))) * tangent;
	vec3 B = mat3(transpose(inverse(model))) * bitangent;

	TBN = mat3(T, B, Normal);
	TangentFragPos = TBN * FragPos;
	TangentViewPos = TBN * viewPos;
}