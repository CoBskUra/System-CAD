#version 410

layout (equal_spacing ,isolines) in;
in float additionalData[];

uniform int VERTICAL_DRAW;
uniform mat4 CAM_MATRIX;
uniform int PATCH_DIV;
uniform float OFFSET;

uniform ivec2 SIZE;

out vec3 debugColor;
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

vec3 Bernstain2D(float t, vec3 p0, vec3 p1, vec3 p2){
    p0 = p0*(1.0 - t) + p1 * t;
    p1 = p1*(1.0 - t) + p2 * t;
    
    p0 = p0*(1.0 - t) + p1 * t;

    return  p0;
}

vec3 BernstainDerivted(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3){
    p0 = 3.0 * (p0 - p1);
    p1 = 3.0 * (p1 - p2);
    p2 = 3.0 * (p2 - p3);

    return  Bernstain2D(t, p0, p1, p2);
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
    vec3 ps_u[4];

    for(int i = 0; i < 4; i++){
        ps[i] = Bernstain(u, 
        gl_in[0 + i*4].gl_Position.xyz,
        gl_in[1 + i*4].gl_Position.xyz,
        gl_in[2 + i*4].gl_Position.xyz,
        gl_in[3 + i*4].gl_Position.xyz
        );
        ps_u[i] = BernstainDerivted(u, 
        gl_in[0 + i*4].gl_Position.xyz,
        gl_in[1 + i*4].gl_Position.xyz,
        gl_in[2 + i*4].gl_Position.xyz,
        gl_in[3 + i*4].gl_Position.xyz
        );
    }

    vec3 p = Bernstain(v, ps[0], ps[1], ps[2], ps[3]);
    vec3 p_u = Bernstain(v, ps_u[0], ps_u[1], ps_u[2], ps_u[3]);
    vec3 p_v = BernstainDerivted(v, ps[0], ps[1], ps[2], ps[3]);
    vec3 normal = normalize(cross(p_v, p_u));
    debugColor = 0.5 + normal * 0.5;

    gl_Position = CAM_MATRIX * vec4(p + normal * OFFSET, 1.0);
    //gl_Position = CAM_MATRIX * vec4(p, 1.0);
    
    int verticalId = int(tePatchId[0] / float(SIZE.y));
    int horizontalId = int(tePatchId[0]) - verticalId * SIZE.y;
    TexCoord = vec2(
    float(horizontalId + u) / float( SIZE.y),
        float(verticalId + v ) / float( SIZE.x));
}