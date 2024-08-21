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

vec3 bicubicPatch(float u, float v){
    vec3 bezierPoints[4];

    for(int y = 0; y < 4; y++){
    bezierPoints[y] =  Bernstain(u,
                            (gl_in[0 + y * 4].gl_Position.xyz +
                             4.0*gl_in[1 + y * 4].gl_Position.xyz +
                             gl_in[2 + y * 4].gl_Position.xyz) / 6.0,

                            (2.0*gl_in[1 + y * 4].gl_Position.xyz +
                             1.0*gl_in[2 + y * 4].gl_Position.xyz) / 3.0,

                            (1.0*gl_in[1 + y * 4].gl_Position.xyz +
                             2.0*gl_in[2 + y * 4].gl_Position.xyz) / 3.0,

                            (gl_in[1 + y * 4].gl_Position.xyz +
                             4.0*gl_in[2 + y * 4].gl_Position.xyz +
                             gl_in[3 + y * 4].gl_Position.xyz) / 6.0);
    }

    vec3 p = Bernstain(v, 
        (bezierPoints[0] + 4*bezierPoints[1] + bezierPoints[2]) / 6.0,
        (2*bezierPoints[1] + bezierPoints[2]) / 3.0,
        (1*bezierPoints[1] + 2*bezierPoints[2]) / 3.0,
        (bezierPoints[1] + 4*bezierPoints[2] + bezierPoints[3]) / 6.0
    );

    return p;
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

    vec3 p = bicubicPatch(u, v);

    gl_Position = CAM_MATRIX * vec4(p , 1.0);
}