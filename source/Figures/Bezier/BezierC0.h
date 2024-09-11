#pragma once
#include "BezierBase.h"
#include "Container/FigureContainer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include "ShaderRefrence/StaticShaders.h"
#include "Models/Curves/BezierC0.h"
#include "SceneManadement/Scene.h"

class BezierC0 : public BezierBase {
	const Shader& lineDrawingShader = StaticShaders::GetBezierCurve();
	int numberOfVertexes = 0;
protected:
	glm::vec4 curveColor{ 1, 0, 1, 1 };

	const Shader& shader_bezier2D = StaticShaders::GetBezier2D();
	VAO vao_bezier3D;

	const Shader& shader_bezier3D = StaticShaders::GetBezier3D();
	VAO vao_bezier2D;
	bool bezier2D_on = false;
public:
	BezierC0(const char* name);
	BezierC0();
	BezierC0(MG1::BezierC0 b0, const Scene* scene, int idOffset = 0);
	MG1::BezierC0 Serialize(int idOffset = 0) const;

	void virtual Draw(GLFWwindow* window, const Camera& camera);
protected:
	BezierC0(const char* name, FigureType type);
	void CreateBezierVAO() override;
};