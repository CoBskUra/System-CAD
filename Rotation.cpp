#include "Rotation.h"

Rotation::Rotation()
{
    Rx = glm::mat4(1.0f);
    Ry = glm::mat4(1.0f);
    Rz = glm::mat4(1.0f);
}

Rotation::Rotation(Quaternion q): Rotation(q.GetEulerRotation()){}

Rotation::Rotation(glm::mat4 rotationMatrix): Rotation(glm::mat3(rotationMatrix)){}

Rotation::Rotation(glm::mat3 rotationMatrix)
{
    if (rotationMatrix[2][0] < +1)
    {
        if (rotationMatrix[2][0] > -1)
        {
            alfa_y = asin(rotationMatrix[2][0]);
            alfa_x = atan2(-rotationMatrix[2][1], rotationMatrix[2][2]);
            alfa_z = atan2(-rotationMatrix[1][0], rotationMatrix[0][0]);
        }
        else//r02=−1
        {
            //Notauniquesolution:thetaZ−thetaX=atan2(r10,r11)
            alfa_y = -M_PI * 0.5f;
            alfa_x = -atan2(rotationMatrix[0][1], rotationMatrix[1][1]);
            alfa_z = -0;
        }
    }
    else//r02=+1
    {
        //Notauniquesolution:thetaZ+thetaX=atan2(r10,r11)
        alfa_y = +M_PI * 0.5f;
        alfa_x = +atan2(rotationMatrix[0][1], rotationMatrix[1][1]);
        alfa_z = +0;
    }
    SetRotation_X(alfa_x);
    SetRotation_Y(alfa_y);
    SetRotation_Z(alfa_z);

}

void Rotation::SetRotation_X(float angle)
{
    alfa_x = SimplifiedAngle(angle);

    float c = cos(alfa_x);
    float s = sin(alfa_x);

    Rx[1][1] = Rx[2][2] = c;
    Rx[2][1] = -s;
    Rx[1][2] = s;
}

void Rotation::SetRotation_Y(float angle)
{
    alfa_y = SimplifiedAngle(angle);

    float c = cos(alfa_y);
    float s = sin(alfa_y);

    Ry[0][0] = Ry[2][2] = c;
    Ry[2][0] = -s;
    Ry[0][2] = s;
}

void Rotation::SetRotation_Z(float angle)
{
    alfa_z = SimplifiedAngle(angle);

    float c = cos(alfa_z);
    float s = sin(alfa_z);

    Rz[0][0] = Rz[1][1] = c;
    Rz[1][0] = -s;
    Rz[0][1] = s;
}


void Rotation::Rotate_X(float angle)
{
    SetRotation_X(alfa_x + angle);
}

void Rotation::Rotate_Y(float angle)
{
    SetRotation_Y(alfa_y + angle);
}

void Rotation::Rotate_Z(float angle)
{
    SetRotation_Z(alfa_z + angle);
}

glm::mat4x4 Rotation::Get_Rx()
{
    return Rx;
}

glm::mat4x4 Rotation::Get_Ry()
{
    return Ry;
}

glm::mat4x4 Rotation::Get_Rz()
{
    return Rz;
}

glm::mat4x4 Rotation::Get_R()
{
    return Rx*Ry*Rz;
}

glm::mat4x4 Rotation::Get_RxInvers()
{
    return glm::transpose(Rx);
}

glm::mat4x4 Rotation::Get_RyInvers()
{
    return glm::transpose(Ry);
}

glm::mat4x4 Rotation::Get_RzInvers()
{
    return glm::transpose(Rz);
}

glm::mat4x4 Rotation::Get_RInvers()
{
    return Get_RzInvers() * Get_RyInvers() * Get_RxInvers();
}


float Rotation::SimplifiedAngle(float angle)
{
    if (angle > 2 * M_PI || angle < 0)
    {
        angle = std::fmod(angle, (2 * M_PI));
    }
    return angle;
}