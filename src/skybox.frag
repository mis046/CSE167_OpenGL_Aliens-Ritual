#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
//in float sampleExtraOutput;
in vec3 TexCoords;

//uniform vec3 color;
uniform samplerCube skybox;

// You can output many things. The first vec4 type output determines the color of the fragment
//out vec4 fragColor;
out vec4 FragColor;

void main()
{
    // Use the color passed in. An alpha of 1.0f means it is not transparent.
//    fragColor = vec4(color, sampleExtraOutput);
    FragColor = texture(skybox, TexCoords);
}
