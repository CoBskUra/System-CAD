#include "Figure3D.h"

Figure3D::Figure3D(Shader* shader) : Figure(shader) {}

Figure3D::Figure3D(Shader* shader, const char* uniqueName, const char* type) : Figure(shader, uniqueName, type) {}

void Figure3D::RotationAlong(glm::vec3 axis, glm::vec3 rotationCenter, float angle)
{
    Figure::RotationAlong(axis, rotationCenter, angle);
    globalRotation = MathOperations::RotationMatrixAlongAxis(angle, axis) * globalRotation;
    //// pierwsza próba
    //Figure::RotationAlong(axis, rotationCenter, angle);
    //glm::vec4 axis_x(1, 0, 0, 1), axis_y(0, 1, 0, 1), axis_z(0, 0, 1, 1);
    //axis_x = rotation.Get_Ry() * rotation.Get_Rz() * axis_x;
    //axis_y = rotation.Get_Rx() *  rotation.Get_Rz()* axis_y;
    //axis_z = rotation.Get_Rx() * rotation.Get_Ry() * axis_z;
    //axis = glm::normalize(axis);
    //auto c_x = glm::dot(axis, glm::vec3(axis_x));
    //auto c_y = glm::dot(axis, glm::vec3(axis_y));
    //auto c_z = glm::dot(axis, glm::vec3( axis_z));

    //glm::vec3 axisVec(c_x, c_y, c_z);
    //axisVec = glm::normalize(axisVec);

    //rotation.Rotate_X(angle * axisVec.x);
    //rotation.Rotate_Y(angle * axisVec.y);
    //rotation.Rotate_Z(angle * axisVec.z);

    //// druga próba 
   /* auto centerToFigure = glm::normalize(transpose.GetPosition() - rotationCenter);
    glm::vec4 axis_x(1, 0, 0, 1), axis_y(0, 1, 0, 1), axis_z(0, 0, 1, 1);
    axis_x = rotation.Get_Rx() * axis_x;
    axis_y = rotation.Get_Rx() * axis_y;
    axis_z = rotation.Get_Rx() * axis_z;

    float alfa_x = glm::dot(centerToFigure, glm::vec3(axis_x));
    float alfa_y = glm::dot(centerToFigure, glm::vec3(axis_y));
    float alfa_z = glm::dot(centerToFigure, glm::vec3(axis_z));

    Figure::RotationAlong(axis, rotationCenter, angle);

    centerToFigure = glm::normalize(transpose.GetPosition() - rotationCenter);
    alfa_x = glm::dot(centerToFigure, glm::vec3(axis_x));
    alfa_y = glm::dot(centerToFigure, glm::vec3(axis_y));
    alfa_z = glm::dot(centerToFigure, glm::vec3(axis_z));*/
}

void Figure3D::ScaleAlong(glm::vec3 scaleCenter, glm::vec3 scaleVec)
{
    Figure::ScaleAlong(scaleCenter, scaleVec);
    scale.SetScale(scaleVec * scale.GetScaleVec());
}

void Figure3D::MoveAlong(const Camera& camera, glm::vec3 direction)
{
}

void Figure3D::ActiveImGui()
{
    Figure::ActiveImGui();
    rotation.ActiveInterferes();
    scale.ActiveInterferes();
}

glm::mat4x4 Figure3D::GetModelMatrix()
{
    return  transpose.Get() * globalRotation * rotation.Get_R() * scale.Get();
}

glm::mat4x4 Figure3D::GetModelMatrixInvers()
{
    return  scale.GetInvers() * rotation.Get_RInvers() * glm::transpose(globalRotation) * transpose.GetInvers();
}
