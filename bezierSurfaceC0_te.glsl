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

    vec3 p0 = Bernstain(u, 
        gl_in[0].gl_Position.xyz,
        gl_in[1].gl_Position.xyz,
        gl_in[2].gl_Position.xyz,
        gl_in[3].gl_Position.xyz
    );
    vec3 p1 = Bernstain(u, 
        gl_in[4].gl_Position.xyz,
        gl_in[5].gl_Position.xyz,
        gl_in[6].gl_Position.xyz,
        gl_in[7].gl_Position.xyz
    );
    
    vec3 p2 = Bernstain(u, 
        gl_in[8].gl_Position.xyz,
        gl_in[9].gl_Position.xyz,
        gl_in[10].gl_Position.xyz,
        gl_in[11].gl_Position.xyz
    );

    vec3 p3 = Bernstain(u, 
        gl_in[12].gl_Position.xyz,
        gl_in[13].gl_Position.xyz,
        gl_in[14].gl_Position.xyz,
        gl_in[15].gl_Position.xyz
    );

    vec3 p = Bernstain(v, p0, p1, p2, p3);

    gl_Position = CAM_MATRIX * vec4(p, 1.0);
}