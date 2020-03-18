#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.;l
in vec3 normalVec;
in vec3 fragPos;

// You can output many things. The first vec4 type output determines the color of the fragment
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

uniform vec3 objectColor;

// Dir light
uniform vec3 dirLightColor;
uniform vec3 dirLightDir;

uniform vec3 viewPos;

uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_shininess;

vec3 CalcDirLight(vec3 color, vec3 lightDir, vec3 normal, vec3 viewDir, vec3 objectColor)
{
    float levels = 3;
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    // combine results
    vec3 ambient  = material_ambient;
    vec3 diffuse  = diff * material_diffuse;
    vec3 specular = spec * material_specular;
    float diffLevel = floor(diff * levels)/levels;
    float specLevel = floor(spec * levels)/levels;

    return  (specLevel + diffLevel + ambient) * color * objectColor;
}

void main()
{
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 norm = normalize(normalVec);
    vec3 directionalLight = CalcDirLight(dirLightColor, normalize(dirLightDir), norm, viewDir, objectColor);

    
    vec3 result = directionalLight;
    
//     Draw edges
    float edge = max(0.0, dot(norm, viewDir));
    if (edge < 0.1) {
        result = vec3(0.0f);
    }

    fragColor = vec4(result, 1.0);
    
    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
//    brightColor = fragColor * brightness * 2.0;

    if(brightness > 0.5)
        brightColor = vec4(fragColor.rgb, 1.0);
    else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
