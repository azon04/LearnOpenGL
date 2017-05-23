#version 330
layout (location = 0) in vec3 position;
layout (location = 0) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

out vec4 vertexColor;

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0f);

	vec3 FragPos = vec3(model * vec4(position, 1.0f));
	vec3 Normal = mat3(model) * normal;

	float ambientStrength = 0.2f;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float specularStrength = 1.0f;
	int shininess = 256;

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	vertexColor = vec4(result, 1.0f);
}

