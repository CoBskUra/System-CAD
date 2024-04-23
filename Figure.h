#ifndef Figura_CLASS_H
#define Figura_CLASS_H

#include"Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include <glm/gtc/type_ptr.hpp>
#include "Constants.h"
#include "Figure.h"
#include "TransposeImGui.h"
#include "Camera.h"
#include "Quaternion.h"
#include "FigureContainer.h"
#include <set>
#include "FigureType.h"

class FigureContainer;

class Figure: protected TransposeImGui
{
public:
	char name[100] = "Figura";
	glm::vec4 color;
	Shader* shader;
	TransposeImGui* transpose;

	Figure(Shader* shader);

	std::string GetUniqueName();
	glm::mat4x4 virtual GetModelMatrix();
	glm::mat4x4 virtual GetModelMatrixInvers();
	FigureType GetType() const;

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

	bool AddContainer(FigureContainer* fc);
	bool EraseContainer(FigureContainer* fc);
	void InformContainers();


protected:
	bool editAbleName = true;
	Figure(Shader* shader, const char* uniqueName, FigureType type);

	FigureType Type = FigureType::Figure;
	VAO vao;
	const int id;
private:
	void SetObjectPosition(float x, float y, float z) override;
	static int count;
	std::string UniqueName = "#Figura";
	std::set<FigureContainer*> containIn;
};
#endif