#version 410 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
layout (location = 1) in float inPatchId;

out float tcPatchId;

// Outputs the color for the Fragment Shader
// out vec4 o_color;

// Imports the camera matrix from the main function

void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = vec4(aPos, 1.0);
	tcPatchId = inPatchId;
}