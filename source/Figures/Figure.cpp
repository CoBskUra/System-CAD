#include "Figures/Figure.h"



uint32_t Figure::s_firstFreeId = 0;
std::set<uint32_t> Figure::s_takenIds = {};
std::stack<uint32_t> Figure::s_freedIds = {};

Figure::Figure(FigureType type) :  Figure() {
	Type = type;
	showColor = unmarkColor;
}

void Figure::SetFirstFreeId()
{
	while (s_takenIds.find(s_firstFreeId) != s_takenIds.end())
	{
		s_firstFreeId++;
	}

	if (s_freedIds.empty())
	{
		id = s_firstFreeId++;
	}
	else
	{
		id = s_freedIds.top();
		s_freedIds.pop();
	}

	s_takenIds.insert(id);
}

void Figure::FreeId() {
	s_takenIds.erase(id);
	//s_freedIds.push(id);
	if (id < s_firstFreeId)
		s_firstFreeId = id;
	id = -1;
}

Figure::Figure() : transpose(this) {
	UnMark();

	Type = FigureType::Figure;
	showColor = unmarkColor;
	SetFirstFreeId();
}


glm::mat4x4 Figure::GetModelMatrix()
{
	return transpose->Get();
}

glm::mat4x4 Figure::GetModelMatrixInvers()
{
	return transpose->GetInvers();
}

FigureType Figure::GetType() const
{
	return Type;
}

glm::vec4 Figure::GetShowColor() const
{
	return showColor;
}

glm::vec4 Figure::SetUnmarkColor(glm::vec4 newColor)
{
	glm::vec4 oldColor = unmarkColor;
	unmarkColor = newColor;
	if (!mark)
	{
		showColor = unmarkColor;
		//InformContainers();
	}
	return oldColor;
}

void Figure::SetMarkColor(glm::vec4 newColor)
{
	markColor = newColor;
	if (mark)
	{
		showColor = markColor;
		//InformContainers();
	}
}

void Figure::SetShowColor(glm::vec4 newColor)
{
	showColor = newColor;
}

Figure::~Figure()
{
	Delete();
	FreeId();
#ifdef _DEBUG
	printf("\nDestroyed %s", name);
#endif
}

void  Figure::Draw(GLFWwindow* window, const Camera& camera) {
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
	this->showColor = unmarkColor;
}

void Figure::Mark() {
	this->showColor = markColor;
}

void  Figure::Delete() {
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
	if(result)
		fc->Erase(this);
	return result;
}

void Figure::InformContainers()
{
	for (std::set<FigureContainer*>::iterator iter = containIn.begin(); 
		iter != containIn.end(); iter++) {
		(*iter)->SomethingHasChange();
	}
}

int Figure::NumberOfContainers()
{
	return containIn.size();;
}

bool Figure::Swap(std::shared_ptr<Figure> to)
{
	if (GetType() != to->GetType())
		return false;
	
	while (containIn.size() > 0)
	{
		to->AddContainer((*containIn.begin()));
		if (!(*containIn.begin())->Swap(this, to)) {
			throw "Something go wrong during swap";
		}
	}
	
	return true;
}

bool Figure::SetObjectOwner(Figure* parent)
{
	if (haveOwner)
		return false;
	haveOwner = true;
	this->owner = parent;
	parent->haveSubjects = true;
	return true;
}

bool Figure::HaveOwner()
{
	return haveOwner;
}

void Figure::RemoveOwner()
{
	haveOwner = false;
	owner = nullptr;
}

bool Figure::IsMyOwner(Figure* possibleOwner)
{
	return possibleOwner == owner;
}

bool Figure::IsOwner()
{
	return haveSubjects;
}

uint32_t Figure::GetId() const
{
	return id;
}

uint32_t Figure::GetFirstFreeId()
{
	return s_firstFreeId;
}

bool Figure::SetId(uint32_t new_id)
{
	if (new_id == this->id)
	{
		return true;
	}

	auto searchResult = s_takenIds.find(new_id);

	if (searchResult != s_takenIds.end())
	{
		return false;
	}

	s_takenIds.insert(new_id);
	s_freedIds.push(this->id);

	this->id = new_id;

	return true;
}


void Figure::SetObjectPosition(float x, float y, float z)
{
	Transpose::SetObjectPosition(x, y, z);
	InformContainers();
}