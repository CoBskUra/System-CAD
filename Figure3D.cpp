#include "Figure3D.h"

Figure3D::Figure3D(Shader* shader) : Figure(shader) {}

Figure3D::Figure3D(Shader* shader, const char* uniqueName, const char* type) : Figure(shader, uniqueName, type) {}

void Figure3D::RotationAlong(const Figure& center, const Camera& camera, float angle)
{
}

void Figure3D::ScaleAlong(const Figure& center, const Camera& camera, glm::vec3 scaleVec)
{
}

void Figure3D::MoveAlong(const Figure& center, const Camera& camera, glm::vec3 direction)
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
    return scale.Get() * transposeImGui.Get() * rotation.Get_Rx() * rotation.Get_Ry() * rotation.Get_Rz();
}

glm::mat4x4 Figure3D::GetModelMatrixInvers()
{
    return rotation.Get_RxInvers() * rotation.Get_RyInvers() * rotation.Get_RzInvers() 
        * transposeImGui.GetInvers() * scale.GetInvers();
}
