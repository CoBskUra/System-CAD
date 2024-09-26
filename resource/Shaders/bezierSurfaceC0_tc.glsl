#version 410

layout (vertices = 16) out;

uniform mat4 CAM_MATRIX;
uniform int PATCH_DIV;

in float tcPatchId[]; // Wspó³rzêdne tekstury z vertex shader
out float tePatchId[]; // Przekazanie do TES

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tePatchId[gl_InvocationID] = tcPatchId[gl_InvocationID];

    gl_TessLevelOuter[0] = PATCH_DIV;

    gl_TessLevelOuter[1] = 2000;

}