#version 410

layout (vertices = 20) out;

uniform mat4 CAM_MATRIX;
uniform int PATCH_DIV;
uniform  int pachId = 0;





vec4 Bernstain2D(float t, vec4 p0, vec4 p1, vec4 p2){
    p0 = p0 * (1 - t) + p1 * t;
    p1 = p1 * (1 - t) + p2 * t;

    p0 = p0 * (1 - t) + p1 * t;

    return p0;

}

float arbitrarySmoothFunction(float v, float scalar_1, float scalar_2)
{
    return scalar_1 * (1 - v) + scalar_1 * v;
}

float det(mat2 matrix){
    return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
}

vec4 function_d(float v, vec4 g[3], vec4 c[3], float k0, float k1, float h0, float h1){
    return arbitrarySmoothFunction(v, k0, k1) * Bernstain2D(v, g[0], g[1], g[2]) + 
    arbitrarySmoothFunction(v, h0, h1) * Bernstain2D(v, c[0], c[1], c[2]);
}

// a,b,c wektory, k,h skalary
// a *k + b*h = c
void solveEquation(vec4 a, vec4 b, vec4 c, out float k, out float h){
    mat2x2 tmp;
    tmp[0] = a.xy;
    tmp[1] = b.xy;
    vec2 answer = c.xy;

    if(det(tmp) == 0)
    {
        tmp[0] = a.yz;
        tmp[1] = b.yz;
        answer = c.yz;
    }
    if(det(tmp) == 0)
    {
        tmp[0] = a.xz;
        tmp[1] = b.xz;
        answer = c.xz;
    }
    vec2 result =  inverse(tmp) * answer;
    k = result.x;
    h = result.y;
}

void SubDivision(float t, vec4 p0, vec4 p1, vec4 p2, vec4 p3, out vec4 result[21], int idOffset ){
    //vec3 results[7];
    result[0 + idOffset] = p0;
    result[6 + idOffset] = p3;

    p0 = p0*( 1 - t) + t * p1;
    p1 = p1*( 1 - t) + t * p2;
    p2 = p2*( 1 - t) + t * p3;
    
    result[1 + idOffset] = p0;
    result[5 + idOffset] = p2;

    p0 = p0*( 1 - t) + t * p1;
    p1 = p1*( 1 - t) + t * p2;

    result[2 + idOffset] = p0;
    result[4 + idOffset] = p1;

    p0 = p0*( 1 - t) + t * p1;
    result[3 + idOffset] = p0;

    //return results;
}

int roundArund(int i, int maxValue){
    return i % maxValue;
}

void main()
{
    vec4 subDivdePatch_closeEdge[7*3];
    vec4 subDivdePatch_farEdge[7*3];

    // obracam tak by zaczyna³o siê zawsze od patcha od którego chcê czyli patchId
    for(int i = 0; i < 3; i++){
        SubDivision(0.5, 
        gl_in[roundArund(0 + (i + pachId) * 8, 24)].gl_Position,
        gl_in[roundArund(1 + (i + pachId) * 8, 24)].gl_Position,
        gl_in[roundArund(2 + (i + pachId) * 8, 24)].gl_Position,
        gl_in[roundArund(3 + (i + pachId) * 8, 24)].gl_Position, subDivdePatch_closeEdge, i*7 );

        SubDivision(0.5, 
        gl_in[roundArund(4 + (i + pachId) * 8, 24)].gl_Position,
        gl_in[roundArund(5 + (i + pachId) * 8, 24)].gl_Position,
        gl_in[roundArund(6 + (i + pachId) * 8, 24)].gl_Position,
        gl_in[roundArund(7 + (i + pachId) * 8, 24)].gl_Position, subDivdePatch_farEdge, i*7  );
    } 

    vec4 p3[3];
    for(int i = 0; i < 3; i++){
        p3[i] = subDivdePatch_closeEdge[ 3 + i * 7];
    }

    vec4 p2[3];
    for(int i = 0; i < 3; i++){
        p2[i] = 2 * p3[i] - subDivdePatch_farEdge[ 3 + i * 7];
    }

    vec4 q[3];
    for(int i = 0; i < 3; i++){
        q[i] = (3.0*p2[i] - p3[i]) / 2.0;
    }

    vec4 p = (q[0] + q[1] + q[2]) / 3.0;

    vec4 p1[3];
    for(int i = 0; i < 3; i++){
        p1[i] = (2.0 * q[i] + p) / 3.0;
    }

    // na razie jako wektory a nie punkty 
    vec4 g[3]; 
    g[0] = subDivdePatch_closeEdge[4] - subDivdePatch_closeEdge[3];
    g[2] =   (- p1[2] + p1[1]) / 2.0;
    g[1] = (g[0] + g[2]) / 2.0;

    vec4 c[3];
    c[0] = p2[0] - p3[0];
    c[1] = p1[0] - p2[0];
    c[2] = p[0] - p1[0];

    vec4 b = p1[1] - p; 

    float k0, k1, h0, h1;
    solveEquation(g[0], c[0], g[0], k0, h0);
    solveEquation(g[2], c[2], b, k1, h1);

    vec4 g2[3]; 
    g2[0] = subDivdePatch_closeEdge[7 * 1 + 2] - subDivdePatch_closeEdge[3];
    g2[2] =  (- p1[2] + p1[0]) / 2.0;
    g2[1] = (g[0] + g[2]) / 2.0;

    vec4 c2[3];
    c2[0] = p2[1] - p3[1];
    c2[1] = p1[1] - p2[1];
    c2[2] = p[1] - p1[1];

    vec4 b2 = p1[0] - p; 

    float k02, k12, h02, h12;
    solveEquation(g2[0], c2[0], g2[0], k02, h02);
    solveEquation(g2[2], c2[2], b2, k12, h12);


    switch(gl_InvocationID){
    //p0 
        case 0: gl_out[gl_InvocationID].gl_Position =  subDivdePatch_closeEdge[3];
        break;
    //e0- 
        case 1: gl_out[gl_InvocationID].gl_Position = subDivdePatch_closeEdge[4];
        break;
    //e3+ 
        case 2: gl_out[gl_InvocationID].gl_Position = subDivdePatch_closeEdge[5 ];
        break;
    //p3 
        case 3: gl_out[gl_InvocationID].gl_Position = subDivdePatch_closeEdge[6];
        break;
    // e0+  
        case 4: gl_out[gl_InvocationID].gl_Position = p2[0];
        break;
    // f0+ // ten punkt od p2 póŸniej jakoœ inacze//////////////////////////////////////
        case 5: gl_out[gl_InvocationID].gl_Position = p2[0] + function_d(1.0/3.0, g, c, k0, k1, h0, h1);
        break;
    // f0- 
        case 6: gl_out[gl_InvocationID].gl_Position = 2 * subDivdePatch_closeEdge[4] - subDivdePatch_farEdge[4];
        break;
    // f3+  
        case 7: gl_out[gl_InvocationID].gl_Position =  2 * subDivdePatch_closeEdge[5] - subDivdePatch_farEdge[5];
        break;
    // f3-         
        case 8: gl_out[gl_InvocationID].gl_Position = 2 * subDivdePatch_closeEdge[7 + 1] - subDivdePatch_farEdge[7 + 1];
        break;
    // e3-   
        case 9: gl_out[gl_InvocationID].gl_Position = subDivdePatch_closeEdge[1 + 1 * 7];
        break;
    // e1- // p1   
        case 10: gl_out[gl_InvocationID].gl_Position = p1[0];
        break;
    // f1- // to od p1 // póŸniej obliczyæ///////////////////////////////////   
        case 11: gl_out[gl_InvocationID].gl_Position = p1[0] + function_d(2.0/3.0, g, c, k0, k1, h0, h1);;
        break;
    // f1+ // to od p1_1 // póŸniej obliczyæ/////////////////////////////////   
        case 12: gl_out[gl_InvocationID].gl_Position = p1[1] + function_d(2.0/3.0, g, c, k0, k1, h0, h1);;
        break;
    // f2- // to od p2_1 // póŸniej obliczyæ/////////////////////////////////   
        case 13: gl_out[gl_InvocationID].gl_Position = p2[1] + function_d(1.0/3.0, g, c, k0, k1, h0, h1);
        break;
    // f2+ //    
        case 14: gl_out[gl_InvocationID].gl_Position = 2 * subDivdePatch_closeEdge[1 * 7 + 2] - subDivdePatch_farEdge[1 * 7 + 2];
        break;
    // e2+ //
        case 15: gl_out[gl_InvocationID].gl_Position = subDivdePatch_closeEdge[1 * 7 + 2];
        break;
    // p1 // 
        case 16: gl_out[gl_InvocationID].gl_Position = p;
        break;
    // e1+ //    
        case 17: gl_out[gl_InvocationID].gl_Position = p1[1];
        break;
    // e2- //
        case 18: gl_out[gl_InvocationID].gl_Position = p2[1];
        break;
    // p2 //
        case 19: gl_out[gl_InvocationID].gl_Position = p3[1];
        break;
        default:
        break;
    }


    gl_TessLevelOuter[0] = PATCH_DIV;

    gl_TessLevelOuter[1] = 2000;
}