#ifndef Figura_CLASS_H
#define Figura_CLASS_H

#include "ShaderManadement/Shader.h"
#include "ShaderManadement/VAO.h"
#include "ShaderManadement/VBO.h"
#include "ShaderManadement/EBO.h"
#include <glm/gtc/type_ptr.hpp>
#include "Constants.h"
#include "Figures/Figure.h"
#include "Transformations/TransposeImGui.h"
#include "Camera.h"
#include "Transformations/Quaternion.h"
#include "Container/FigureContainer.h"
#include <set>
#include "FigureType.h"
#include <stack>
#include "Models/SceneObject.h"


class FigureContainer;

class Figure: protected TransposeImGui
{
private:
	glm::vec4 showColor;
	glm::vec4 unmarkColor{ 1,1,1,1 };
	glm::vec4 markColor{ 1, 0.8f, 0, 1 };
	std::set<FigureContainer*> containIn;
	bool mark = false;

	bool haveOwner{ false };
	Figure* owner;
	bool haveSubjects{ false };

	static uint32_t s_firstFreeId;
	static std::set<uint32_t> s_takenIds;
	static std::stack<uint32_t> s_freedIds;
	uint32_t id;

	void SetFirstFreeId();
	void FreeId();
public:
	char name[100] = "Figura";
	TransposeImGui* transpose;

	Figure();

	glm::mat4x4 virtual GetModelMatrix();
	glm::mat4x4 virtual GetModelMatrixInvers();
	FigureType GetType() const;
	glm::vec4 GetShowColor() const;
	glm::vec4 virtual  SetUnmarkColor(glm::vec4 newColor);
	void virtual SetMarkColor(glm::vec4 newColor);
	void virtual SetShowColor(glm::vec4 newColor);

	virtual ~Figure();

	void virtual Draw(GLFWwindow* window, const Camera& camera);

	void virtual ActiveImGui();
	void virtual FigureSpecificImGui();

	void virtual RotationAlong(glm::vec3 axis, glm::vec3 rotationCenter, float angle);
	void virtual RotationAlong(Quaternion q, glm::vec3 rotationCenter);
	void virtual ScaleAlong(glm::vec3 scaleCenter, glm::vec3 scaleVec);
	void virtual MoveAlong(const Camera& camera, glm::vec3 direction);

	bool virtual Inputs(GLFWwindow* window, const Camera& camera);
	glm::vec4 PositionOnScreen(const Camera& camera);

	void SetName(const char* newName);
	void Unmark();
	void Mark();
	void virtual Delete();

	bool virtual AddContainer(FigureContainer* fc);
	bool EraseContainer(FigureContainer* fc);
	void InformContainers();
	int NumberOfContainers();
	bool Swap(std::shared_ptr<Figure> to);

	// mo�na wsadzi� w interfers
	bool SetObjectOwner(Figure* parent);
	bool HaveOwner();
	void RemoveOwner();
	bool IsMyOwner(Figure* possibleOwner);

	// mo�na wsadzi� w interfers
	bool IsOwner();
	bool SetId(uint32_t id);
	uint32_t GetId() const;
	static uint32_t GetFirstFreeId();
	static uint32_t LargestTakenId();

protected:
	void virtual SetObjectPosition(float x, float y, float z);
	bool editAbleName = true;
	Figure(FigureType type);
	Figure(const char* name, FigureType type);
	FigureType Type = FigureType::Figure;
};
#endif