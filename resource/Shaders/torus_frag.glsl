#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Inputs the color from the Vertex Shader
in vec2 TexCoord;
uniform sampler2D texture1;

uniform vec4 COLOR;

void main()
{
	if(texture(texture1, TexCoord).r < 0.5)
		discard;
	FragColor = COLOR;
}