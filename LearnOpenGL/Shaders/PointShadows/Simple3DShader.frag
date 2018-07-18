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

out vec4 color;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform PointLight light;
uniform bool useBlinn;
uniform Material material;

uniform samplerCube shadowMap;

uniform float far_plane;

const float shadowBias = 0.05;

float ShadowCalculation(vec3 fragPos)
{
	vec3 fragToLight = fragPos - light.position;
	float closestDepth = texture(shadowMap, fragToLight).r;
	
	closestDepth *= far_plane;

	float currentDepth = length(fragToLight);

	float shadow = (currentDepth - shadowBias) > closestDepth ? 1.0 : 0.0;

	return shadow;
}

float ShadowCalculationPCF(vec3 fragPos)
{
	vec3 fragToLight = fragPos - light.position;
	float currentDepth = length(fragToLight);

	float shadow = 0.0;
	float samples = 4.0;
	float offset = 0.1;
	float sampleStep = offset / (samples * 0.5);
	
	for(float x= -offset; x < offset; x += sampleStep)
	{
		for(float y=-offset; y < offset; y += sampleStep)
		{
			for(float z=-offset; z < offset; z += sampleStep)
			{
				float closestDepth = texture(shadowMap, fragToLight + vec3(x,y,z)).r;
				closestDepth *= far_plane;
				shadow += (currentDepth - shadowBias) > closestDepth ? 1.0 : 0.0;
			}
		}
	}

	shadow /= (samples * samples * samples);

	return shadow;
}

vec3 sampleOffsetDirections[20] = vec3[]
	(
		vec3( 1, 1, 1), vec3( 1, -1, 1), vec3(-1, -1, 1), vec3( -1, 1, 1),
		vec3( 1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3( -1, 1, -1),
		vec3( 1, 1, 0), vec3( 1, -1, 0), vec3(-1, -1, 0), vec3( -1, 1, 0),
		vec3( 1, 0, 1), vec3( -1, 0, 1), vec3(1, 0, -1), vec3( -1, 0, -1),
		vec3( 0, 1, 1), vec3( 0, -1, 1), vec3(0, -1, -1), vec3( 0, 1, -1)
	);

float ShadowCalculationPCF_Improved(vec3 fragPos)
{
	vec3 fragToLight = fragPos - light.position;
	float currentDepth = length(fragToLight);

	float shadow = 0.0;
	int samples = 20;
	float bias = 0.15;
	float viewDistance = length(viewPos - fragPos);
	float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
	for(int i=0; i < samples; i++)
	{
		float closestDepth = texture(shadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= far_plane;
		shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
	}

	shadow /= float(samples);

	return shadow;
}

void main()
{
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 lightDir = normalize(light.position - FragPos);

	// diffuse shading
	float diff = max(dot(norm, lightDir), 0.0);

	//specular shading
	float spec = 0.0;
	if( useBlinn )
	{
		vec3 halfwayDir = normalize( lightDir + viewDir );
		spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
	}
	else
	{
		vec3 reflectDir = reflect(-lightDir, norm);
		spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
	}
	// combine result
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	
	vec3 specularColor = vec3(1.0f);
	if( material.useSpecular )
	{
		specularColor = vec3(texture(material.specular, TexCoords));
	}
	vec3 specular = light.specular * spec * specularColor;
	
	// Calculate shadow
	float shadow = ShadowCalculationPCF_Improved(FragPos);

	vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular));
	color = vec4(result, 1.0);
}