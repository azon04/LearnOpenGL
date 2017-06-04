#version 330 core
struct Material {
	
	// This is old version, only using ambient and diffuse color
	// vec3 ambient;
	// vec3 diffuse;

	// This is new version, using diffuse and ambient as a texture
	sampler2D diffuse;
	
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform Light light;
uniform Material material;

void main() 
{
	// Ambient
	// vec3 ambient = light.ambient * material.ambient;
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	// Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	// vec3 diffuse = light.diffuse * (diff * material.diffuse);
	
	// Specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (material.specular * spec);

	vec3 result = (ambient + diffuse + specular);
	color = vec4(result, 1.0f);
}