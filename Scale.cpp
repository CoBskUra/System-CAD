#include "Scale.h"

Scale::Scale()
{
    ScaleVec = glm::vec3(1.0f);
    Matrix = glm::mat4(1.0f);
    Matrix_invers = glm::mat4(1.0f);
}

glm::vec3 Scale::GetScaleVec()
{
    return ScaleVec;
}

glm::mat4x4 Scale::Get()
{
    return Matrix;
}

glm::mat4x4 Scale::GetInvers()
{
    return Matrix_invers;
}

void Scale::SetScale(glm::vec3 vec)
{
    SetScale(vec.x, vec.y, vec.z);
}

void Scale::SetScale(float x, float y, float z)
{
    ScaleVec.x = x;
    ScaleVec.y = y;
    ScaleVec.z = z;

    Matrix[0][0] = x;
    Matrix[1][1] = y;
    Matrix[2][2] = z;

    Matrix_invers[0][0] = 1 / x;
    Matrix_invers[1][1] = 1 / y;
    Matrix_invers[2][2] = 1 / z;
}
