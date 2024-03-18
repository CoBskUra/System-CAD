#include "AfirmatricTransformation.h"


AfirmatricTransformation::AfirmatricTransformation()
{
    Rx                  = glm::mat4(1.0f);
    Rx_invers           = glm::mat4(1.0f);

    Ry                  = glm::mat4(1.0f);
    Ry_invers           = glm::mat4(1.0f);

    Rz                  = glm::mat4(1.0f);
    Rz_invers           = glm::mat4(1.0f);


    ObjectPosition      = glm::vec3(0.0f);
    Transform           = glm::mat4(1.0f);
    Transform_invers    = glm::mat4(1.0f);

    ScaleVec            = glm::vec3(1.0f);
    Scale               = glm::mat4(1.0f);
    Scale_invers        = glm::mat4(1.0f);
    
    M                   = glm::mat4(1.0f);
    M_invers            = glm::mat4(1.0f);


    CalculateM();
}

void AfirmatricTransformation::SetRotation_X(float angle)
{
    alfa_x = SimplifiedAngle(angle);

    float c = cos(alfa_x);
    float s = sin(alfa_x);

    Rx[1][1] = Rx[2][2] = c;
    Rx[2][1] = -s;
    Rx[1][2] = s;

    Rx_invers = glm::transpose(Rx);

    CalculateM();
}

void AfirmatricTransformation::SetRotation_Y(float angle)
{
    alfa_y = SimplifiedAngle(angle);

    float c = cos(alfa_y);
    float s = sin(alfa_y);

    Ry[0][0] = Ry[2][2] = c;
    Ry[2][0] = -s;
    Ry[0][2] = s;

    Ry_invers = glm::transpose(Ry);

    CalculateM();
}

void AfirmatricTransformation::SetRotation_Z(float angle)
{
    alfa_z = SimplifiedAngle(angle);

    float c = cos(alfa_z);
    float s = sin(alfa_z);

    Rz[0][0] = Rz[1][1] = c;
    Rz[1][0] = -s;
    Rz[0][1] = s;

    Rz_invers = glm::transpose(Rz);

    CalculateM();
}

void AfirmatricTransformation::Rotate_X(float angle)
{
    SetRotation_X(alfa_x + angle);
}

void AfirmatricTransformation::Rotate_Y(float angle)
{
    SetRotation_Y(alfa_y + angle);
}

void AfirmatricTransformation::Rotate_Z(float angle)
{
    SetRotation_Z(alfa_z + angle);
}

void AfirmatricTransformation::SetObjectPosition(glm::vec3 vec)
{
    SetObjectPosition(vec.x, vec.y, vec.z);
}

void AfirmatricTransformation::SetObjectPosition(float x, float y, float z)
{
    ObjectPosition[0] = x;
    ObjectPosition[1] = y;
    ObjectPosition[2] = z;

    Transform[3][0] = x;
    Transform[3][1] = y;
    Transform[3][2] = z;

    Transform_invers[3][0] = -x;
    Transform_invers[3][1] = -y;
    Transform_invers[3][2] = -z;

    CalculateM();
}

void AfirmatricTransformation::MoveObjectPosition(glm::vec3 vec)
{
    SetObjectPosition(ObjectPosition + vec);
}

void AfirmatricTransformation::MoveObjectPosition(float x, float y, float z)
{
    SetObjectPosition(
        ObjectPosition[0] + x,
        ObjectPosition[1] + y,
        ObjectPosition[2] + z);

}

void AfirmatricTransformation::SetScale(glm::vec3 vec) {
    SetScale(vec.x, vec.y, vec.z);
}

void AfirmatricTransformation::SetScale(float x, float y, float z)
{
    ScaleVec.x = x;
    ScaleVec.y = y;
    ScaleVec.z = z;

    Scale[0][0] = x;
    Scale[1][1] = y;
    Scale[2][2] = z;

    Scale_invers[0][0] = 1 / x;
    Scale_invers[1][1] = 1 / y;
    Scale_invers[2][2] = 1 / z;

    CalculateM();
}

void AfirmatricTransformation::CalculateM()
{
    M           = Scale * Transform * Rz * Ry * Rx;
    M_invers    = Rx_invers * Ry_invers * Rz_invers * Transform_invers * Scale_invers;
}



float AfirmatricTransformation::SimplifiedAngle(float angle)
{
    if (angle > 2 * M_PI || angle < 0)
    {
        angle = std::fmod(angle, (2 * M_PI));
    }
    return angle;
}