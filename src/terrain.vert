#version 330 core

//The vertex shader gets called once per vertex.

//Define position, normal, and texture defined in the Container.
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

//Define uniform MVP: model, view, projection passed from the object.
uniform mat4 MVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//Define any out variables for the fragment shader.
out vec3 FragPos;
out vec3 FragNormal;
out vec2 FragTexCoords;

void main()
{
    gl_Position = MVP * vec4(vertex.x, vertex.y, vertex.z, 1.0f);
	FragPos = vec3(model * vec4(vertex.x, vertex.y, vertex.z, 1.0f));
	FragNormal = vec3( mat4(transpose(inverse(model)))  * vec4(normal.x, normal.y, normal.z, 1.0f));  
	FragTexCoords = texCoords;
}
