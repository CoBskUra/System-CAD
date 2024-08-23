#pragma once
#include "Figures/Figure.h"
#include "Helper/OpenGLHelper.h"

class Cursor : public Figure {
public:
	Cursor(const char* name) : Figure(FigureType::Cursor)
	{
		SetUnmarkColor( glm::vec4(0.8, 0.8, 0.8, 0.6));
		CreatePoint();
		SetName(name);
		editAbleName = false;
	}

	Cursor() : Cursor("Cursor")
	{}

	void virtual Draw(GLFWwindow* window, const Camera& camera) {
		cursorShader->Activate();
		vaoCursor.Bind();

		glUniformMatrix4fv(glGetUniformLocation(cursorShader->ID, "MODEL_MATRIX"),
			1, GL_FALSE, glm::value_ptr(GetModelMatrix()));
		camera.SaveMatrixToShader(cursorShader->ID);
		glDrawArrays(GL_LINES, 0, 12);
		vaoCursor.Unbind();
	}

	bool virtual Inputs(GLFWwindow* window, const Camera& camera) {
		ImGuiIO& io = ImGui::GetIO();
		
		if (io.WantCaptureMouse)
			return false;

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			auto castCursorToScreen = camera.GetCameraMatrix() * glm::vec4(transpose->GetPosition(), 1);
			castCursorToScreen /= castCursorToScreen.w;

			if (castCursorToScreen.z < 0.2f )
				castCursorToScreen.z = 0.2f;
			if(castCursorToScreen.z > 1)
				castCursorToScreen.z = 0.8f;

			glm::vec4 mousePos{ 
				OpenGLHelper::MousePositionOnScreen(window),
				castCursorToScreen.z,
				1.0f
			};

			mousePos = camera.GetCameraMatrixInvers() * mousePos;
			mousePos /= mousePos.w;
			transpose->SetObjectPosition(mousePos);
			return true;
		}
		return false;
	}


private:
	Shader* cursorShader = StaticShaders::GetPointerToCursor();
	VAO vaoCursor;
	const float length = 0.25f;
	void CreatePoint() {

		auto showColor = GetShowColor();
		std::vector<float> vs =
		{
		0, 0, 0,			1, 0, 0, 1,
		length , 0, 0,		1, 0, 0, 1,
		0, length, 0,		0, 1, 0, 1,
		0, 0, 0,			0, 1, 0, 1,
		0, 0, length,		0, 0, 1, 1,
		0, 0, 0,			0, 0, 1, 1,
		-length / 3, 0, 0,		showColor.r, 0.2, 0.2, showColor.a,
		0, 0, 0,			showColor.r, 0.2, 0.2, showColor.a,
		0, -length / 3, 0,		0.2, showColor.g, 0.2, showColor.a,
		0, 0, 0,			0.2, showColor.g, 0.2, showColor.a,
		0, 0, -length / 3, 		0.2, 0.2, showColor.b, showColor.a,
		0, 0, 0, 			0.2, 0.2, showColor.b, showColor.a,
		};


		vaoCursor.Bind();
		VBO vbo(vs, GL_STATIC_DRAW);

		vaoCursor.LinkAttrib(0, 3, GL_FLOAT, false, 7 * sizeof(float), 0);
		vaoCursor.LinkAttrib(1, 4, GL_FLOAT, false, 7 * sizeof(float),(void*) (3 * sizeof(float)));

		vaoCursor.Unbind(); vbo.Unbind();
	}
};