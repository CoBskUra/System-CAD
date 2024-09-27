#version 410

layout (equal_spacing ,isolines) in;
in float additionalData[];

uniform int VERTICAL_DRAW;
uniform mat4 CAM_MATRIX;
uniform int PATCH_DIV;

uniform ivec2 SIZE;

out vec2 TexCoord;  
in float tePatchId[]; 

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

    for(int i = 0; i < 4; i++){
        ps[i] = Bernstain(u, 
        gl_in[0 + i*4].gl_Position.xyz,
        gl_in[1 + i*4].gl_Position.xyz,
        gl_in[2 + i*4].gl_Position.xyz,
        gl_in[3 + i*4].gl_Position.xyz
        );
    }

    vec3 p = Bernstain(v, ps[0], ps[1], ps[2], ps[3]);

    gl_Position = CAM_MATRIX * vec4(p, 1.0);
    
    int verticalId = int(tePatchId[0] / float(SIZE.y));
    int horizontalId = int(tePatchId[0]) - verticalId * SIZE.y;
    TexCoord = vec2(
    float(horizontalId + u) / float( SIZE.y),
        float(verticalId + v ) / float( SIZE.x));
}