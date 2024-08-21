#version 410

layout (equal_spacing ,isolines) in;

uniform float numberOfSegments;
uniform float segmentId;
uniform mat4 CAM_MATRIX;


void main()
{
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    
    float t = (1 / numberOfSegments) *segmentId +  gl_TessCoord.x / numberOfSegments;


    float t1 = (1.0 - t);

    // Bernstein polynomials
    float b2 = t * t;
    float b1 = 2.0 * t * t1;
    float b0 = t1 * t1;

    // Cubic Bezier interpolation
    vec3 p = p0 * b0 + p1 * b1 + p2 * b2;

    gl_Position = CAM_MATRIX * vec4(p, 1.0);
}