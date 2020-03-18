#version 330 core

uniform vec3 lightColor;
uniform vec3 viewPos;
uniform samplerCube skybox;
uniform float reflect_intensity;

uniform sampler2D TerrainTexture;

uniform float max_height;
uniform float min_height;
uniform bool toon_shade;

//Define any in variables from the vertex shader.
in vec3 FragPos;
in vec3 FragNormal;
in vec2 FragTexCoords;

//Define out variable for the fragment shader: color.
//out vec4 color;
//layout (location = 0) out vec4 fragColor;
layout (location = 0) out vec4 color;
layout (location = 1) out vec4 brightColor;

void main()
{
	vec2 tiledCoords = FragTexCoords * 40.0;
	vec4 total_color = texture(TerrainTexture, tiledCoords);

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
    brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.1)
        brightColor = vec4(color.rgb, 1.0);
    else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}

