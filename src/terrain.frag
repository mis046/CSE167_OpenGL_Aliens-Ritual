#version 330 core

uniform vec3 lightColor;
uniform vec3 viewPos;
uniform samplerCube skybox;
uniform float reflect_intensity;

uniform sampler2D TerrainTexture_0;
uniform sampler2D TerrainTexture_1;
uniform sampler2D TerrainTexture_2;
uniform sampler2D TerrainTexture_3;
uniform sampler2D blendMap;

uniform float max_height;
uniform float min_height;
uniform bool toon_shade;

//Define any in variables from the vertex shader.
in vec3 FragPos;
in vec3 FragNormal;
in vec2 FragTexCoords;

//Define out variable for the fragment shader: color.
out vec4 color;

void main()
{
	vec4 total_color = vec4(0.0, 0.0, 0.0, 0.0);

	vec4 blendMapColor = texture(blendMap, FragTexCoords);

	float TerrainTexture_0_amount = 1 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
	vec2 tiledCoords = FragTexCoords * 40.0;

	vec4 TerrainTexture_0_color = texture(TerrainTexture_0, tiledCoords) * TerrainTexture_0_amount;
	
	vec4 TerrainTexture_1_color = texture(TerrainTexture_1, tiledCoords) * blendMapColor.r;
	vec4 TerrainTexture_2_color = texture(TerrainTexture_2, tiledCoords) * blendMapColor.g;
	vec4 TerrainTexture_3_color = texture(TerrainTexture_3, tiledCoords) * blendMapColor.b;
	 
	total_color += TerrainTexture_0_color + TerrainTexture_1_color + TerrainTexture_2_color + TerrainTexture_3_color;

	//Fake lighting
	vec3 light_color = vec3(1.0, 0.929, 0.929);
	vec3 toLightVector = vec3(1.0, 1.0, -0.2);
	vec3 unitNormal = normalize(FragNormal);
	vec3 unitLightVector = normalize(toLightVector);

	float nDotl = dot(unitNormal, unitLightVector);
	float brightness = max(nDotl, 0.1);
	vec3 diffuse = brightness * light_color;

	//Toon shading
	float toon_shade_effect = 1.0;
	if (toon_shade)
	{
		if(brightness > 0.98)	toon_shade_effect = 1.0;
		else if(brightness > 0.95)	toon_shade_effect = 0.9;
		else if(brightness > 0.5)	toon_shade_effect = 0.7;
		else if(brightness > 0.05)	toon_shade_effect = 0.35;
		else	toon_shade_effect = 1.0;
	}
	diffuse = toon_shade_effect * diffuse;

	vec3 unitVectorToCamera = normalize(FragPos - viewPos);
	vec3 lightDirection = -unitLightVector;
	vec3 reflectedLightDirection = reflect(lightDirection,unitNormal);
	
	float specularFactor = dot(reflectedLightDirection , unitVectorToCamera);
	specularFactor = max(specularFactor, 0.0);
	float dampedFactor = pow(specularFactor,0.5);
	float reflectivity = 0.05;
	vec3 finalSpecular = dampedFactor * reflectivity * light_color;

	total_color = vec4(diffuse, 1.0) * total_color + vec4(finalSpecular, 1.0);

	color = total_color;

	if (toon_shade)
	{
		float edge = dot(normalize(viewPos - FragPos), FragNormal);
		edge = max(0, edge);
		if (edge < 0.01)
		{
			color = 0.0 * color;
		}
	}

}

