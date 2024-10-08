#version 410

layout (equal_spacing ,isolines) in;
in float additionalData[];

uniform int VERTICAL_DRAW;
uniform mat4 CAM_MATRIX;
uniform int PATCH_DIV;

vec3 Bernstain(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3){
    p0 = p0*(1.0 - t) + p1 * t;
    p1 = p1*(1.0 - t) + p2 * t;
    p2 = p2*(1.0 - t) + p3 * t;
    
    p0 = p0*(1.0 - t) + p1 * t;
    p1 = p1*(1.0 - t) + p2 * t;

    p0 = p0*(1.0 - t) + p1 * t;

    return  p0;
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y * PATCH_DIV / (PATCH_DIV - 1); 

    if(VERTICAL_DRAW > 0)
    {
        float tmp = u;
        u = v;
        v = tmp;
    }

    vec3 ps[4];

    int start = 0*4;
    ps[0] = Bernstain(u, 
    gl_in[0 + start].gl_Position.xyz,
    gl_in[1 + start].gl_Position.xyz,
    gl_in[2 + start].gl_Position.xyz,
    gl_in[3 + start].gl_Position.xyz
    );
    

    start += 4;
    ps[1] = Bernstain(u, 
    gl_in[0 + start].gl_Position.xyz,
    (u * gl_in[1 + start].gl_Position.xyz + v * gl_in[2 + start].gl_Position.xyz) /(u + v), 
    ((1 - u) * gl_in[3 + start].gl_Position.xyz + v * gl_in[4 + start].gl_Position.xyz) /(1 - u + v),
    gl_in[5 + start].gl_Position.xyz
    );
    
    start += 6;
    ps[2] = Bernstain(u, 
    gl_in[0 + start].gl_Position.xyz,
    ((1 - u) * gl_in[1 + start].gl_Position.xyz + (1 - v) * gl_in[2 + start].gl_Position.xyz) /(2 - u - v),
    (u * gl_in[3 + start].gl_Position.xyz + (1 - v) * gl_in[4 + start].gl_Position.xyz) /(1 + u - v),
    gl_in[5 + start].gl_Position.xyz
    );
    
    start += 6;
    ps[3] = Bernstain(u, 
    gl_in[0 + start].gl_Position.xyz,
    gl_in[1 + start].gl_Position.xyz,
    gl_in[2 + start].gl_Position.xyz,
    gl_in[3 + start].gl_Position.xyz
    );
    

    vec3 p = Bernstain(v, ps[0], ps[1], ps[2], ps[3]);

    gl_Position = CAM_MATRIX * vec4(p, 1.0);
}