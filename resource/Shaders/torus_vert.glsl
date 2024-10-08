#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 inTexCord;

out vec2 TexCoord;

// Imports the camera matrix from the main function
uniform mat4 MODEL_MATRIX;
uniform mat4 CAM_MATRIX;

void main()
{
	// Outputs the positions/coordinates of all vertices
	// CAM_MATRIX *
	gl_Position = CAM_MATRIX * MODEL_MATRIX *  vec4(aPos, 1.0);
	TexCoord = inTexCord;
}