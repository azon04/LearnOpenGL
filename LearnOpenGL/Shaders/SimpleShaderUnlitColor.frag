#version 330 core
#define NUMBER_POINT_LIGHTS 4

struct Material {
	
	// This is old version, only using ambient and diffuse color
	// vec3 ambient;
	// vec3 diffuse;
	// vec2 specular;

	// This is new version, using diffuse and ambient as a texture
	sampler2D diffuse;
	sampler2D diffuse1;
	sampler2D diffuse2;

	sampler2D specular;
	sampler2D specular1;
	
	float shininess;
};

// Directional light structure
struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// Point Light
struct PointLight{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// Spot Light
struct SpotLight{
	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct Light {
	vec3 position;
	vec3 direction; // For Directional Light and Flashlight
	float cutOff; // Flashlight
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// Use for point light
	float constant;
	float linear;
	float quadratic;
};

out vec4 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform Light light;
uniform DirLight dirLight;
uniform PointLight pointLights[NUMBER_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform Material material;

void main()
{
	color = vec4(texture(material.diffuse, TexCoords), 1.0);
}