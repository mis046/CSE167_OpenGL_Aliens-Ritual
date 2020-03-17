#version 330 core

in vec3 FragPos;
in vec3 FragNormal;

uniform vec3 viewPos;
uniform samplerCube skybox;
uniform bool toon_shade;

//Define out variable for the fragment shader: color.
out vec4 color;

void main()
{
	vec4 total_color = vec4(0, 0, 0, 1);

	//Fake lighting
	vec3 light_color = vec3(0.9, 0.929, 0.929);
	vec3 toLightVector = vec3(1.0, 1.0, -0.2);
	vec3 unitNormal = normalize(FragNormal);
	vec3 unitLightVector = normalize(toLightVector);

	float nDotl = dot(unitNormal, unitLightVector);
	float brightness = max(nDotl, 0.1);
	vec3 diffuse = brightness * light_color * 0.5;

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
	specularFactor = max(specularFactor, 0.01);
	float dampedFactor = pow(specularFactor, 0.5);
	float reflectivity = 0.1;
	vec3 finalSpecular = dampedFactor * reflectivity * light_color;

	total_color = vec4(diffuse, 1.0) + vec4(finalSpecular, 1.0);
    
    //Water to air ratio.
    float ratio = 1.0/1.33;
    
    vec3 I = normalize(FragPos - viewPos);
    
    vec3 reflection = reflect(I, normalize(FragNormal));
    vec3 refraction = refract(I, normalize(FragNormal), ratio);

    vec4 colorReflection = texture(skybox, reflection);
    vec4 colorRefraction = texture(skybox, refraction);

	float percent = 0.35;
    
    color = total_color + normalize(mix(colorReflection, colorRefraction, percent));

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

    