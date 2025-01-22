#version 410

layout (equal_spacing ,isolines) in;

uniform int VERTICAL_DRAW;
uniform mat4 CAM_MATRIX;
uniform int PATCH_DIV;
uniform ivec2 SIZE;

out vec2 TexCoord;  
in float tePatchId[]; 

struct Info{
    vec3 normal;
    vec3 pos;
};

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
    p0 = 3.0 * (p1 - p0);
    p1 = 3.0 * (p1 - p2);
    p2 = 3.0 * (p2 - p3);

    return  Bernstain2D(t, p0, p1, p2);
}

void ToBeziseBaise(inout vec3 p0,inout vec3 p1,inout vec3 p2,inout vec3 p3){
    vec3 tmpP1 = p1;

    p0 = (p0 + 4*p1 + p2) / 6.0;
    p3 = (p1 + 4*p2 + p3) / 6.0;

    p1 = (2*tmpP1 + p2) / 3.0;
    p2 = (1*tmpP1 + 2*p2) / 3.0;
}

Info bicubicPatch(float u, float v){
    vec3 bezierPoints[4];
    vec3 bezierPoints_Du[4];
    Info info;
    vec3 p0, p1, p2, p3;

    for(int y = 0; y < 4; y++){
        p0 = gl_in[0 + y * 4].gl_Position.xyz;
        p1 = gl_in[1 + y * 4].gl_Position.xyz;
        p2 = gl_in[2 + y * 4].gl_Position.xyz;
        p3 = gl_in[3 + y * 4].gl_Position.xyz;
        ToBeziseBaise(p0, p1, p2, p3);
        bezierPoints[y] =  Bernstain(u, p0, p1, p2, p3);
        bezierPoints_Du[y] = BernstainDerivted(u, p0, p1, p2, p3);
    }

    p0 = bezierPoints[0];
    p1 = bezierPoints[1];
    p2 = bezierPoints[2];
    p3 = bezierPoints[3];
    ToBeziseBaise(p0, p1, p2, p3);
    vec3 p = Bernstain(v, p0, p1, p2, p3);
    vec3 p_v = BernstainDerivted(v, p0, p1, p2, p3);

    p0 = bezierPoints_Du[0];
    p1 = bezierPoints_Du[1];
    p2 = bezierPoints_Du[2];
    p3 = bezierPoints_Du[3];
    ToBeziseBaise(p0, p1, p2, p3);
    vec3 p_u = Bernstain(v, p0, p1, p2, p3);
    info.pos = p;
    info.normal = normalize(cross(p_u, p_v));

    return info;
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

    int verticalId = int(tePatchId[0] / float(SIZE.y));
    int horizontalId = int(tePatchId[0]) - verticalId * SIZE.y;
    TexCoord = vec2(
    float(horizontalId + u) / float( SIZE.y),
        float(verticalId + v ) / float( SIZE.x));

    //vec3 p = bicubicPatch(u, v);
    // stiupid fix
    Info info = bicubicPatch(u, v);
    if(u < 0.296875)
        info.normal = -info.normal;
    if(v < 0.296875)
        info.normal = -info.normal;

    gl_Position = CAM_MATRIX * vec4(info.pos + info.normal * 10 , 1.0);
}