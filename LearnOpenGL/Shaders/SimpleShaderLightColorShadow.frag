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

	bool useSpecular;
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
in vec4 FragPosLightSpace;

uniform vec3 viewPos;

uniform Light light;
uniform DirLight dirLight;
uniform PointLight pointLights[NUMBER_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform bool useDirLight;
uniform int pointLightCount;
uniform bool useSpotLight;

uniform bool useBlinn;

uniform Material material;

// Shadow Map
uniform sampler2D shadowMap;

const float shadowBias = 0.005;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = vec3(0.0f);

	// phase 1: Directional lighting
	if( useDirLight )
	{
		result += CalcDirLight(dirLight, norm, viewDir);
	}
	// phase 2: Point Lights
	for(int i=0; i < pointLightCount; i++)
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	// phase 3: Spot Light
	if( useSpotLight )
	{
		result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
	}

	color = vec4(result, 1.0);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// [0,1]
	projCoords = projCoords * 0.5 + 0.5;
	// Comparison current depth and expected depth in shadow map
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	// Calculate bias to prevent shadow Acne
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), shadowBias);
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	// Set shadow to 0 if pos outside of the light view frustrum
	if(projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

float ShadowCalculationPCF(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// [0,1]
	projCoords = projCoords * 0.5 + 0.5;
	vec2 texSize = 1.0 / textureSize(shadowMap, 0);
	float currentDepth = projCoords.z;
	// Calculate bias to prevent shadow acne
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), shadowBias);
	float shadow = 0.0;
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	if(projCoords.z > 1.0)
		shadow = 0.0;
	return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot( normal, lightDir), 0.0);
	// specular shading
	float spec = 0.0;
	if( useBlinn )
	{
		vec3 halfwayDir = normalize( lightDir + viewDir );
		spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	}
	else
	{
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
	}
	// combine results
	float shadow = ShadowCalculationPCF(FragPosLightSpace, normal, lightDir);
	vec3 ambient = (light.ambient) * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specularColor = vec3(1.0f);
	if( material.useSpecular )
	{
		specularColor = vec3(texture(material.specular, TexCoords));
	}
	vec3 specular = light.specular * spec * specularColor;

	return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	//specular shading
	float spec = 0.0;
	if( useBlinn )
	{
		vec3 halfwayDir = normalize( lightDir + viewDir );
		spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	}
	else
	{
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
	}
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	// combine result
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specularColor = vec3(1.0f);
	if( material.useSpecular )
	{
		specularColor = vec3(texture(material.specular, TexCoords));
	}
	vec3 specular = light.specular * spec * specularColor;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	//specular shading
	float spec = 0.0;
	if( useBlinn )
	{
		vec3 halfwayDir = normalize( lightDir + viewDir );
		spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	}
	else
	{
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
	}
	// intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	//combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specularColor = vec3(1.0f);
	if( material.useSpecular )
	{
		specularColor = vec3(texture(material.specular, TexCoords));
	}
	vec3 specular = light.specular * spec * specularColor;
	ambient *= attenuation;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}