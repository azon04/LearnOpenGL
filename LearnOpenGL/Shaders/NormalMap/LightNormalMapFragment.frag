#version 330 core

struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// attenuation
	float constant;
	float linear;
	float quadratic;
};

out vec4 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;

uniform vec3 viewPos;
uniform Light light;

uniform sampler2D normalMap;
uniform sampler2D diffuseMap;
uniform float shininess;

void main()
{
	// obtain normal from normal map in range [0,1]
	vec3 normal = texture(normalMap, TexCoords).rgb;
	// transform normal vector to range [-1,1]
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(TBN * normal);

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 lightDir = normalize(light.position - FragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	float spec = 0.0;
	// Use Blinn-Phong
	vec3 halfwayDir = normalize( lightDir + viewDir );
	spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	
	// Combine results
	vec3 ambient = light.ambient * vec3(texture(diffuseMap, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap, TexCoords));
	vec3 specular = light.specular * spec;

	// attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	
	vec3 resultColor = (ambient + diffuse + specular) * attenuation;

	color = vec4(resultColor, 1.0);
}