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

class FigureContainer;

class Figure: protected TransposeImGui
{
private:
	glm::vec4 showColor;
	glm::vec4 unmarkColor{ 1,1,1,1 };
	glm::vec4 markColor{ 1, 0.8f, 0, 1 };
	static int count;
	std::string UniqueName = "#Figura";
	std::set<FigureContainer*> containIn;
	bool mark = false;

	bool haveOwner{ false };
	Figure* owner;
	bool haveSubjects{ false };
public:
	char name[100] = "Figura";
	TransposeImGui* transpose;

	Figure();

	std::string GetUniqueName();
	glm::mat4x4 virtual GetModelMatrix();
	glm::mat4x4 virtual GetModelMatrixInvers();
	FigureType GetType() const;
	glm::vec4 GetShowColor() const;
	glm::vec4 SetUnmarkColor(glm::vec4 newColor);
	void SetMarkColor(glm::vec4 newColor);
	void SetShowColor(glm::vec4 newColor);

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
	void UnMark();
	void Mark();
	void virtual Delete();

	bool virtual AddContainer(FigureContainer* fc);
	bool EraseContainer(FigureContainer* fc);
	void InformContainers();
	int NumberOfContainers();

	bool SetObjectOwner(Figure* parent);
	bool HaveOwner();
	void RemoveOwner();
	bool IsMyOwner(Figure* possibleOwner);
	bool IsOwner();

protected:
	void virtual SetObjectPosition(float x, float y, float z);
	bool editAbleName = true;
	Figure(const char* uniqueName, FigureType type);

	FigureType Type = FigureType::Figure;
	const int id;
};
#endif