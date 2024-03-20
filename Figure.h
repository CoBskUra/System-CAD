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

	Figure(Shader* shader, glm::vec4 color) {
		this->shader = shader;
		this->color = color;
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


	void virtual Delete() {
		vao.Delete();
	}

protected:
	AfirmationTransformation_ImGui afirmationTransformation;
	VAO vao;



};

