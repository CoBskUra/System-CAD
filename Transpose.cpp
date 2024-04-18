#include "transpose.h"

Transpose::Transpose()
{
    ObjectPosition = glm::vec3(0.0f);
    Transform = glm::mat4(1.0f);
}

glm::mat4 Transpose::Get() const
{
    return Transform;
}

glm::mat4 Transpose::GetInvers() const
{
    glm::mat4x4 invers = Transform;

    invers[3][0] = -invers[3][0];
    invers[3][1] = -invers[3][1];
    invers[3][2] = -invers[3][2];

    return invers;
}

glm::vec3 Transpose::GetPosition() const
{
    return ObjectPosition;
}

void Transpose::SetObjectPosition(glm::vec3 vec)
{
    SetObjectPosition(vec.x, vec.y, vec.z);
}

void Transpose::SetObjectPosition(float x, float y, float z)
{
    ObjectPosition[0] = x;
    ObjectPosition[1] = y;
    ObjectPosition[2] = z;

    Transform[3][0] = x;
    Transform[3][1] = y;
    Transform[3][2] = z;
}

void Transpose::MoveObjectPosition(glm::vec3 vec)
{
    SetObjectPosition(ObjectPosition + vec);
}

void Transpose::MoveObjectPosition(float x, float y, float z)
{
    SetObjectPosition(
        ObjectPosition[0] + x,
        ObjectPosition[1] + y,
        ObjectPosition[2] + z);
}
