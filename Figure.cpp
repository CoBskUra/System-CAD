#include "Figure.h"



int Figure::count = 0;

Figure::Figure(Shader* shader, const char* uniqueName, const char* type) :  Figure(shader) {
	UniqueName = uniqueName + std::to_string(id);
	Type = type;
}

Figure::Figure(Shader* shader) : id(count), transpose(this) {
	this->shader = shader;
	UnMark();

	count++;
	UniqueName += std::to_string(id);
}

std::string Figure::GetUniqueName() {
	return UniqueName;
}

glm::mat4x4 Figure::GetModelMatrix()
{
	return transpose->Get();
}

glm::mat4x4 Figure::GetModelMatrixInvers()
{
	return transpose->GetInvers();
}

const char* Figure::GetType() const
{
	return Type;
}

Figure::~Figure()
{
	Delete();
	printf("\nDestroyed %s", name);
}

void  Figure::Draw(const Camera& camera) {
}

void  Figure::ActiveImGui() {
	ImGui::Text(name);
	if(editAbleName)
		ImGui::InputText("Name", name, sizeof(name));
	transpose->ActiveInterferes();
}

void Figure::FigureSpecificImGui()
{
}

void Figure::RotationAlong(glm::vec3 axis, glm::vec3 rotationCenter, float angle)
{
	glm::vec3 rotationCenterToFigureVec = transpose->GetPosition() - rotationCenter;
	glm::vec3 newPos = MathOperations::RotationAlongAxis(rotationCenterToFigureVec, angle, axis);
	if(newPos != glm::vec3{0.0f})
		transpose->SetObjectPosition(newPos + rotationCenter);
}

void Figure::RotationAlong(Quaternion q, glm::vec3 rotationCenter)
{
	transpose->SetObjectPosition(rotationCenter + q.RotatePoint(transpose->GetPosition() - rotationCenter));
}

void Figure::ScaleAlong(glm::vec3 scaleCenter, glm::vec3 scaleVec)
{
	auto scaleCenterPos = transpose->GetPosition() - scaleCenter;
	if (scaleCenterPos != glm::vec3{ 0.0f })
		transpose->SetObjectPosition(scaleCenter + scaleVec * scaleCenterPos);
}

void Figure::MoveAlong(const Camera& camera, glm::vec3 direction)
{
	transpose->MoveObjectPosition(direction);
}

bool  Figure::Inputs(GLFWwindow* window, const Camera& camera)
{
	return false;
}

glm::vec4 Figure::PositionOnScreen(const Camera& camera)
{
	auto tmp = camera.GetCameraMatrix() * GetModelMatrix() * glm::vec4(0, 0, 0, 1);
	return tmp / tmp.w;
}

void Figure::SetName(const char* newName) {
	sprintf_s(name, newName);
}

void Figure::UnMark() {
	this->color = glm::vec4(1, 1, 1, 1);
}

void Figure::Mark() {
	this->color = glm::vec4(1, 0.8f, 0, 1);
}

void  Figure::Delete() {
	vao.Delete();
	while (containIn.begin() != containIn.end()) {
		(*containIn.begin())->Erase(this);
	}
}

bool Figure::AddContainer(FigureContainer* fc)
{
	auto result = containIn.insert(fc);
	return result.second;
}

bool Figure::EraseContainer(FigureContainer* fc)
{
	auto result = containIn.erase(fc);
	return result;
}

void Figure::InformContainers()
{
	for (std::set<FigureContainer*>::iterator iter = containIn.begin(); 
		iter != containIn.end(); iter++) {
		(*iter)->SomethingHasChange();
	}
}

void Figure::SetObjectPosition(float x, float y, float z)
{
	Transpose::SetObjectPosition(x, y, z);
	InformContainers();
}
