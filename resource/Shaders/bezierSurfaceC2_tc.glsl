#version 410

layout (vertices = 16) out;

uniform mat4 CAM_MATRIX;
uniform int PATCH_DIV;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    gl_TessLevelOuter[0] = PATCH_DIV;

    gl_TessLevelOuter[1] = 2000;
}