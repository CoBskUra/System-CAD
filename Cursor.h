#pragma once
#include "Figure.h"
#include "OpenGLHelper.h"

class Cursor : public Figure {
public:
	Cursor(Shader* shader, const char* name) : Figure(shader, "##Cursor", "Cursor")
	{
		CreatePoint();
		SetName(name);
		color = glm::vec4(0, 1, 0, 1);
		editAbleName = false;
	}

	Cursor(Shader* shader) : Figure(shader, "##Cursor", "Cursor")
	{
		CreatePoint();
		SetName("Cursor");
		editAbleName = false;
		color = glm::vec4(0, 1, 0, 1);
	}

	void virtual Draw() {
		vao.Bind();

		glUniformMatrix4fv(glGetUniformLocation(shader->ID, "MODEL_MATRIX"),
			1, GL_FALSE, glm::value_ptr(GetModelMatrix()));
		glUniform4f(glGetUniformLocation(shader->ID, "COLOR"), color.x, color.y, color.z, color.w);

		glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
		vao.Unbind();
	}

	bool virtual Inputs(GLFWwindow* window, const Camera& camera) {
		ImGuiIO& io = ImGui::GetIO();
		
		if (io.WantCaptureMouse)
			return false;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			auto castCursorToScreen = camera.GetCameraMatrix() * glm::vec4(transpose.GetPosition(), 1);
			castCursorToScreen /= castCursorToScreen.w;

			if (castCursorToScreen.z < 0.1f || castCursorToScreen.z > 1)
				castCursorToScreen.z = 0.8f;

			glm::vec4 mousePos{ 
				OpenGLHelper::MousePositionOnScreen(window),
				castCursorToScreen.z,
				1.0f
			};

			mousePos = camera.GetCameraMatrixInvers() * mousePos;
			mousePos /= mousePos.w;
			transpose.SetObjectPosition(mousePos);
			return true;
		}
		return false;
	}


private:
	const float length = 0.25f;
	void CreatePoint() {
		std::vector<float> vs =
		{
		length , 0, 0,
		0, length, 0,
		0, 0, length,
		-length, 0, 0,
		0, -length, 0,
		0, 0, -length };

		std::vector<GLuint> ies =
		{
		0, 3,
		1, 4,
		2, 5
		};


		vao.Bind();
		VBO vbo(vs, GL_STATIC_DRAW);
		EBO ebo(ies);

		vao.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

		vao.Unbind(); vbo.Unbind(); ebo.Unbind();
	}
};