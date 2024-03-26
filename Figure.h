#ifndef Figura_CLASS_H
#define Figura_CLASS_H

#include"Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include <glm/gtc/type_ptr.hpp>
#include "Constants.h"
#include "AfirmationTransformationImGui.h"
#include "Figure.h"
#include "TransposeImGui.h"
#include "Camera.h"

class Figure
{
public:
	char name[100] = "Figura";
	glm::vec4 color;
	Shader* shader;

	Figure(Shader* shader);

	std::string GetUniqueName();
	glm::mat4x4 virtual GetModelMatrix();
	glm::mat4x4 virtual GetModelMatrixInvers();
	const char* GetType() const;

	~Figure();

	void virtual Draw();

	void virtual ActiveImGui();
	void virtual RotationAlong(const Figure& center, const Camera& camera, float angle);
	void virtual ScaleAlong(const Figure& center, const Camera& camera, glm::vec3 scaleVec);
	void virtual MoveAlong(const Figure& center, const Camera& camera, glm::vec3 direction);
	bool virtual Inputs(GLFWwindow* window, const Camera& camera);
	glm::vec4 PositionOnScreen(const Camera& camera);

	void SetName(const char* newName);
	void UnMark();
	void Mark();
	void virtual Delete();

protected:
	Figure(Shader* shader, const char* uniqueName, const char* type);

	TransposeImGui transposeImGui;
	const char* Type = "Figura";
	//AfirmationTransformation_ImGui afirmationTransformation;
	VAO vao;
	const int id;
private:
	static int count;
	std::string UniqueName = "#Figura";
};
#endif