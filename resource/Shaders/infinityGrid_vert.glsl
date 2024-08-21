#version 410

uniform mat4 camMatrixInvers;
uniform mat4 CAM_MATRIX;

layout(location = 0) in vec3 p;
layout(location = 1) out vec3 nearPoint;
layout(location = 2) out vec3 farPoint;

vec3 UnprojectPoint(float x, float y, float z) {
    vec4 unprojectedPoint =  camMatrixInvers * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {

    nearPoint = UnprojectPoint(p.x, p.y, 0.0).xyz; // unprojecting on the near plane

    farPoint = UnprojectPoint(p.x, p.y, 1.0).xyz; // unprojecting on the far plane

    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
}
