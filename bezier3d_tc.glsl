#version 410

layout (vertices = 4) out;

uniform mat4 CAM_MATRIX;
uniform float resolution;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    gl_TessLevelOuter[0] = 1.0;

    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;

    vec3 v1 = vec3( CAM_MATRIX * vec4(p1 - p0, 0));
    vec3 v2 = vec3( CAM_MATRIX * vec4(p2 - p0, 0));
    vec3 v3 = vec3( CAM_MATRIX * vec4(p1 - p3, 0));
    vec3 v4 = vec3( CAM_MATRIX * vec4(p2 - p3, 0));

    v1.z = 0;
    v2.z = 0;
    v3.z = 0;
    v4.z = 0;

    float area1 =  length(cross(v1, v2));
    float area2 =  length(cross(v3, v4));

    gl_TessLevelOuter[1] = (area1 + area2) * resolution + 800;
}