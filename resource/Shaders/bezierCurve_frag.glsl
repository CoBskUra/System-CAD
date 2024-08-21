#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;


// Inputs the color from the Vertex Shader
// in vec4 o_color;

uniform vec4 COLOR;

void main()
{
	FragColor = COLOR;
}