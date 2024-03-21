#pragma once
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include <glm/gtc/type_ptr.hpp>
#include "Constants.h"
#include "AfirmationTransformationImGui.h"
#include "Figure.h"

class Figure
{
public:
	const char* name = "Figure";
	glm::vec4 color;
	Shader* shader;

	Figure(Shader* shader) {
		this->shader = shader;
		UnMark();
	}

	~Figure()
	{
		Delete();
	}

	void virtual Draw(bool& showInterferes) {
	}

	void virtual ActiveImGui(bool& show) {
	}

	void SetName(const char* newName) {
		name = newName;
	}

	void UnMark() {
		this->color = glm::vec4(1, 1, 1, 1);
	}

	void Mark() {
		this->color = glm::vec4(1, 0.8f, 0, 1);
	}

	void virtual Delete() {
		vao.Delete();
	}

protected:
	AfirmationTransformation_ImGui afirmationTransformation;
	VAO vao;



};

