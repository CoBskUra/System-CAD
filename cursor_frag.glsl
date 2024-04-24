#version 330 core


layout(location = 1) in vec4 color;
// Outputs colors in RGBA
out vec4 FragColor;


// Inputs the color from the Vertex Shader
// in vec4 o_color;

void main()
{
	FragColor = color;
}