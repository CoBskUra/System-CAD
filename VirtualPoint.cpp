#include "VirtualPoint.h"

void VirtualPoint::SetObjectPosition(float x, float y, float z)
{
	glm::vec3 lastPos = transpose->GetPosition();
	Point::SetObjectPosition(x, y, z);
	glm::vec3 currentPos = transpose->GetPosition();
	glm::vec3 delta = currentPos - lastPos;
	for (auto iter = connectedPoints.begin(); iter != connectedPoints.end(); iter++) {
		(*iter)->transpose->MoveObjectPosition(delta);
	}
}

bool VirtualPoint::Inputs(GLFWwindow* window, const Camera& camera)
{
	return figureController.Move(window, camera, this);
}

VirtualPoint::VirtualPoint() : Point("##VirtualPoint", FigureType::VirtualPoint)
{
	SetName("VirtualPoint");
}
