#pragma once
#include "Figure.h"
#include "FigureContainer.h"
//#include <GLFW/glcorearb.h>

class BezierCurve : public Figure, public  FigureContainer {
	Shader lineDrawing{"bezierCurve_vert.glsl", "bezierCurve_frag.glsl"};
	glm::vec4 curveColor{ 1, 0, 1, 1 };
public:
	BezierCurve(Shader* shader, const char* name) : BezierCurve(shader, "##BezierCurve", "BezierCurve")
	{
		CreateBezierCurve();
		SetName(name);
	}

	BezierCurve(Shader* shader) : BezierCurve(shader, "##BezierCurve", "BezierCurve")
	{
		CreateBezierCurve();
		SetName("BezierCurve");
	}

	void virtual Draw() {
		/*shader->Activate();
		vao.Bind();

		glUniform4f(glGetUniformLocation(shader->ID, "COLOR"),
			curveColor.x, curveColor.y, curveColor.z, curveColor.w);

		glDrawArrays(GL_PATCHES, 0, 4);
		vao.Unbind();*/
		lineDrawing.Activate();
		vao.Bind();

		glUniform4f(glGetUniformLocation(lineDrawing.ID, "COLOR"),
			curveColor.x, curveColor.y, curveColor.z, curveColor.w);

		glDrawArrays(GL_LINE_STRIP, 0, 4);

		vao.Unbind();
		//shader->Activate();
	}

	bool virtual Inputs(GLFWwindow* window, const Camera& camera) {
		return false;
	}

	bool IsValid(Figure* figure) override {
		return figure->GetType() == "Point";
	}

protected:
	BezierCurve(Shader* shader, const char* uniqueName, const char* type) : Figure(shader, uniqueName, type) {
		
	}

private:
	void CreateBezierCurve() {
		vao.Bind();
		std::vector<float> vs = {
			+0, +0, +0,
			+0, +1, +0,
			+1, -1, +0,
			+2, +1, +0
		};
		VBO vbo(vs, GL_DYNAMIC_DRAW);

		vao.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

		vao.Unbind(); vbo.Unbind();
		//glPatchParameteri(GL_PATCH_VERTICES, 4);
	}
};