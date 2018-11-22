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
in vec3 TangentFragPos;
in vec3 TangentViewPos;

uniform Light light;
uniform float height_scale;
uniform sampler2D normalMap;
uniform sampler2D diffuseMap;
uniform sampler2D displacementMap;

uniform float shininess;

float ParallaxSoftShadowMultiplier(vec2 texCoords, vec3 lightDir)
{
	float shadowMultiplier = 0;

	const float minLayers = 8.0;
	const float maxLayers = 32.0;
	// number of depth layer
	float numLayer = mix(maxLayers, minLayers, abs(dot(vec3(0.0,0.0,1.0), lightDir)));
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayer;
	// the amount to shift the texture coordinates per layer
	vec2 P = lightDir.xy * height_scale;
	vec2 deltaTexCoords = P / numLayer;

	// get initial values
	float currentLayerDepth = texture(displacementMap, texCoords).r;
	float currentLayerHeight = currentLayerDepth - layerDepth;
	vec2 currentTexCoords = texCoords + deltaTexCoords;
	currentLayerDepth = texture(displacementMap, currentTexCoords).r;
	int stepIndex = 0;
	float numberOfSampleUnderSurface = 0;

	while(currentLayerHeight > 0)
	{
		if(currentLayerDepth < currentLayerHeight)
		{
			numberOfSampleUnderSurface += 1;
			float newShadowMultiplier = (currentLayerHeight - currentLayerDepth) * (1.0 - stepIndex / numLayer);
			shadowMultiplier = max(shadowMultiplier, newShadowMultiplier);
		}

		// offset the next layer
		stepIndex += 1;
		currentLayerHeight -= layerDepth;
		currentTexCoords += deltaTexCoords;
		currentLayerDepth = texture(displacementMap, currentTexCoords).r;
	}

	if(numberOfSampleUnderSurface < 1)
	{
		shadowMultiplier = 1;
	}
	else
	{
		shadowMultiplier = 1.0 - shadowMultiplier;
	}

	return shadowMultiplier;
}
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
	const float minLayers = 8.0;
	const float maxLayers = 32.0;
	// number of depth layer
	float numLayer = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
	// calculate the size of each layer
	float layerDepth = 1.0f / numLayer;
	//depth of current layer
	float currentLayerDepth = 0.0f;
	// the amout to shift the texture coordinates per layer (from Vector P)
	vec2 P = viewDir.xy * height_scale;
	vec2 deltaTexCoords = P / numLayer;

	// get initial values
	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = texture(displacementMap, currentTexCoords).r;

	while( currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates
		currentTexCoords -= deltaTexCoords;
		//get depthmap value at the current texture coordinates
		currentDepthMapValue = texture(displacementMap, currentTexCoords).r;
		// get depth of the next layer
		currentLayerDepth += layerDepth;
	}
	
	// get texture coordinates before collision (reverse operation)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(displacementMap, prevTexCoords).r - currentLayerDepth + layerDepth;

	// interpolation of the texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}

void main()
{
	vec3 TangentLightPos = TBN * light.position;
	
	vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
	vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
	vec2 texCoords = ParallaxMapping(TexCoords, viewDir);
	float shadowMultiplier = ParallaxSoftShadowMultiplier(texCoords, lightDir);

	if(texCoords.x > 1.0f || texCoords.x < 0.0f || texCoords.y > 1.0f || texCoords.y < 0.0f)
		discard;

	// obtain normal from normal map in range [0,1]
	vec3 normal = texture(normalMap, texCoords).rgb;
	// transform normal vector to range [-1,1]
	normal = normalize(normal * 2.0 - 1.0);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	float spec = 0.0;
	// Use Blinn-Phong
	vec3 halfwayDir = normalize( lightDir + viewDir );
	spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	
	// Combine results
	vec3 ambient = light.ambient * vec3(texture(diffuseMap, texCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseMap, texCoords));
	vec3 specular = light.specular * spec;

	// attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	
	vec3 resultColor = (ambient + shadowMultiplier * (diffuse + specular)) * attenuation;
	color = vec4(resultColor, 1.0);
	//color = vec4(vec3(shadowMultiplier), 1.0);
}