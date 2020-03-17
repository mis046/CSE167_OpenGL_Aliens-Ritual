#version 330 core

//The vertex shader gets called once per vertex.

//Define position, normal, and texture defined in the Container.
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;


//Define uniform MVP: model, view, projection passed from the object.
uniform mat4 MVP;
uniform mat4 model;
uniform float time; //elasped time

out vec3 FragNormal;
out vec3 FragPos;

//shader constants
const float amplitude = 0.3;
const float frequency = 10;
const float PI = 3.14159;


void main()
{
    /* Ripple Effect */
    //Get the Euclidean distance of the current vertex from the center of the mesh
    float dist = length(vertex);
    //Create a sin/cos function using the distance, multiply frequency and add the elapsed time
    float y = amplitude*sin(-PI*dist*frequency+time) +  amplitude*cos(-PI*dist*frequency+time);
    //Multiply the MVP matrix with the new position to get the clipspace position.
    gl_Position = MVP * vec4(vertex.x, vertex.y - y*.5, vertex.z, 1);
	//Update variables to pass to the fragment shader.
    FragPos = vec3(model * vec4(vertex.x, y, vertex.z, 1.0f));
    FragNormal = vec3( mat4(transpose(inverse(model)))  * vec4(normal.x, normal.y, normal.z, 1.0f));
}
