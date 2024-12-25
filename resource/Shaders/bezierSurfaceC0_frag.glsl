#version 410 core

// Outputs colors in RGBA
out vec4 FragColor;


// Inputs the color from the Vertex Shader
// in vec4 o_color;

uniform vec4 COLOR;
in vec3 debugColor;
in vec2 TexCoord;
uniform sampler2D texture1;

void main()
{
	if(texture(texture1, TexCoord).r < 0.5)
		discard;
	FragColor = COLOR;
	FragColor = vec4(debugColor, 1);
}