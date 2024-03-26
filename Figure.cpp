#include "Figure.h"



int Figure::count = 0;

Figure::Figure(Shader* shader, const char* uniqueName, const char* type) : Figure(shader) {
	UniqueName = uniqueName + std::to_string(id);
	Type = type;
}

Figure::Figure(Shader* shader) : id(count) {
	this->shader = shader;
	UnMark();

	count = 0;
	count++;
	UniqueName += std::to_string(id);
}

std::string Figure::GetUniqueName() {
	return UniqueName;
}

glm::mat4x4 Figure::GetModelMatrix()
{
	return transposeImGui.Get();
}

glm::mat4x4 Figure::GetModelMatrixInvers()
{
	return transposeImGui.GetInvers();
}

const char* Figure::GetType() const
{
	return Type;
}

Figure::~Figure()
{
	Delete();
}

void  Figure::Draw() {
}

void  Figure::ActiveImGui() {
	ImGui::Text(name);
	ImGui::InputText("Name", name, sizeof(name));
	transposeImGui.ActiveInterferes();
}

void Figure::RotationAlong(const Figure& center, const Camera& camera, float angle)
{

}

void Figure::ScaleAlong(const Figure& center, const Camera& camera, glm::vec3 scaleVec)
{
}

void Figure::MoveAlong(const Figure& center, const Camera& camera, glm::vec3 direction)
{
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
}