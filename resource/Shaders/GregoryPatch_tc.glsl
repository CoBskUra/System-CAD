#version 410

layout (vertices = 20) out;

uniform mat4 CAM_MATRIX;
uniform int PATCH_DIV;
uniform  int pachId = 0;

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


    if(gl_InvocationID == 0){ //p0 
        gl_out[gl_InvocationID].gl_Position =  subDivdePatch_closeEdge[3];
    }
	else if(gl_InvocationID == 1){ //e0- 
        gl_out[gl_InvocationID].gl_Position = subDivdePatch_closeEdge[4];
    }
	 else if(gl_InvocationID == 2){ //e3+ 
        gl_out[gl_InvocationID].gl_Position = subDivdePatch_closeEdge[5 ];
    }
    else if(gl_InvocationID == 3){ //p3
        gl_out[gl_InvocationID].gl_Position = subDivdePatch_closeEdge[6];
    }
    else if(gl_InvocationID == 4){ // e0+ //inaczej p20 //inaczej e0+
        gl_out[gl_InvocationID].gl_Position = p2[0];
    }
    else if(gl_InvocationID == 5){ // f0+ // ten punkt od p2 póŸniej jakoœ inacze//////////////////////////////////////
        gl_out[gl_InvocationID].gl_Position = p;
    }else if(gl_InvocationID == 6){     // f0-      
        gl_out[gl_InvocationID].gl_Position = 2 * subDivdePatch_closeEdge[4] - subDivdePatch_farEdge[4];
    }
    else if(gl_InvocationID == 7){     // f3+      
        gl_out[gl_InvocationID].gl_Position =  2 * subDivdePatch_closeEdge[5] - subDivdePatch_farEdge[5];
    }
    else if(gl_InvocationID == 8){       // f3-      
        gl_out[gl_InvocationID].gl_Position = 2 * subDivdePatch_closeEdge[7 + 1] - subDivdePatch_farEdge[7 + 1];
    }
    else if(gl_InvocationID == 9){   // e3-
        gl_out[gl_InvocationID].gl_Position = subDivdePatch_closeEdge[1 + 1 * 7];
    }
    else if(gl_InvocationID == 10){  // e1- // p1
        gl_out[gl_InvocationID].gl_Position = p1[0];
    } 
    else if(gl_InvocationID == 11){  // f1- // to od p1 // póŸniej obliczyæ///////////////////////////////////
        gl_out[gl_InvocationID].gl_Position = p;
    }
    else if(gl_InvocationID == 12){  // f1+ // to od p1_1 // póŸniej obliczyæ/////////////////////////////////
        gl_out[gl_InvocationID].gl_Position = p;
    }
    else if(gl_InvocationID == 13){  // f2- // to od p2_1 // póŸniej obliczyæ/////////////////////////////////
        gl_out[gl_InvocationID].gl_Position = p;
    }
    else if(gl_InvocationID == 14){  // f2+ // 
        gl_out[gl_InvocationID].gl_Position = 2 * subDivdePatch_closeEdge[1 * 7 + 2] - subDivdePatch_farEdge[1 * 7 + 2];
    }
    else if(gl_InvocationID == 15){  // e2+ // 
        gl_out[gl_InvocationID].gl_Position = subDivdePatch_closeEdge[1 * 7 + 2];
    }
    else if(gl_InvocationID == 16){  // p1 // 
        gl_out[gl_InvocationID].gl_Position = p;
    }
    else if(gl_InvocationID == 17){  // e1+ // 
        gl_out[gl_InvocationID].gl_Position = p1[1];
    }
    else if(gl_InvocationID == 18){  // e2- // 
        gl_out[gl_InvocationID].gl_Position = p2[1];
    }
    else if(gl_InvocationID == 19){  // p2 // 
        gl_out[gl_InvocationID].gl_Position = p3[1];
    }


    gl_TessLevelOuter[0] = PATCH_DIV;

    gl_TessLevelOuter[1] = 2000;
}