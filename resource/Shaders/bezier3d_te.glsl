#version 410

layout (equal_spacing ,isolines) in;

uniform float numberOfSegments;
uniform float segmentId;
uniform mat4 CAM_MATRIX;

vec3 BezierD(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3){
    p0 = 3.0 * ( p0 - p1);
    p1 = 3.0 * ( p1 - p2);
    p2 = 3.0 * ( p2 - p3);
    
    p0 = (1 - t)*p0 + t * p1;
    p1 = (1 - t)*p1 + t * p2;

    p0 = (1 - t)*p0 + t * p1;

    return p0;
}

vec3 Normal(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3){
    vec3 n = BezierD(t, p0, p1, p2, p3);
    float tmp;
    tmp = n.x;
    n.x = -n.z;
    n.z = tmp;
    n.y = 0;
    return normalize(n);
}

void main()
{

    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;
    vec3 p3 = gl_in[3].gl_Position.xyz;
    
    float t = (1 / numberOfSegments) *segmentId +  gl_TessCoord.x / numberOfSegments;


    float t1 = (1.0 - t);
    float t2 = t * t;

    // Bernstein polynomials
    float b3 = t2 * t;
    float b2 = 3.0 * t2 * t1;
    float b1 = 3.0 * t * t1 * t1;
    float b0 = t1 * t1 * t1;

    // Cubic Bezier interpolation
    vec3 p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;

    //gl_Position = CAM_MATRIX * vec4(p + 0.1 * Normal(t, p0, p1, p2, p3), 1.0);
    gl_Position = CAM_MATRIX * vec4(p, 1.0);
}