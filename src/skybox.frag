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
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;
//out vec4 fragColor;

void main()
{
    // Use the color passed in. An alpha of 1.0f means it is not transparent.
//    fragColor = vec4(color, sampleExtraOutput);
    fragColor = texture(skybox, TexCoords);
    
    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    brightColor = fragColor * brightness;

//    if(brightness > 0.3)
//        brightColor = vec4(fragColor.rgb, 1.0);
//    else
//        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
