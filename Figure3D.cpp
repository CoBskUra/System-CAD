#include "Figure3D.h"

Figure3D::Figure3D(Shader* shader) : Figure(shader) {}

Figure3D::Figure3D(Shader* shader, const char* uniqueName, FigureType type) : Figure(shader, uniqueName, type) {}

void Figure3D::RotationAlong(glm::vec3 axis, glm::vec3 rotationCenter, float angle)
{
    Figure::RotationAlong(axis, rotationCenter, angle);
    Quaternion q(angle, axis);
    quaternion.SaveRotation();
    quaternion.SetMainQuaternion(q * quaternion);
}

void Figure3D::RotationAlong(Quaternion q, glm::vec3 rotationCenter)
{
    Figure::RotationAlong(q, rotationCenter);
    quaternion.SaveRotation();
    quaternion.SetMainQuaternion(q * quaternion );
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
    quaternion.ActiveInterferes();
    scale.ActiveInterferes();
}

glm::mat4x4 Figure3D::GetModelMatrix()
{
    return  transpose->Get() *  quaternion.GetEulerRotation() * rotation.Get_R() *  scale.Get(); // do zmiany
}

glm::mat4x4 Figure3D::GetModelMatrixInvers()
{
    return  scale.GetInvers() * rotation.Get_RInvers() * quaternion.Invers().GetEulerRotation() * transpose->GetInvers();
}
