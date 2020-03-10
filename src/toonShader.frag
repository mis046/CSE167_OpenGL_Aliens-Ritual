#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.;l
in vec3 normalVec;
in vec3 fragPos;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

uniform vec3 objectColor;

// Dir light
uniform vec3 dirLightColor;
uniform vec3 dirLightDir;

uniform vec3 viewPos;

uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_shininess;

vec3 CalcDirLight(vec3 color, vec3 direction, vec3 normal, vec3 viewDir, vec3 objectColor)
{
    float levels = 4;

    vec3 lightDir = normalize(direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    // combine results
    vec3 ambient  = material_ambient;
    vec3 diffuse  = diff * material_diffuse;
    vec3 specular = spec * material_specular;
    float level = floor(diff * levels)/levels;

    return  (specular + level + ambient) * color * objectColor;
}

void main()
{
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 norm = normalize(normalVec);
    vec3 directionalLight = CalcDirLight(dirLightColor, dirLightDir, norm, viewDir, objectColor);

    vec3 result = directionalLight;
    
    fragColor = vec4(result, 1.0);

}
