#include "BazierCurve.h"
#include "OpenGLHelper.h"

BezierCurve::BezierCurve(Shader* shader, const char* name) : BezierCurve(shader, "##BezierCurve", FigureType::BezierCurve)
{
	CreateBezierCurve();
	SetName(name);
}

BezierCurve::BezierCurve(Shader* shader) : BezierCurve(shader, "##BezierCurve", FigureType::BezierCurve)
{
	CreateBezierCurve();
	SetName("BezierCurve");
}

BezierCurve::BezierCurve(Shader* shader, const char* uniqueName, FigureType type) : Figure(shader, uniqueName, type)
{}

void BezierCurve::Draw(GLFWwindow* window, const Camera& camera) {
	Update();
	if (showBezierPol) {
		shader->Activate();
		vao.Bind();
		{
			camera.SaveMatrixToShader(shader->ID);
			glUniform4f(glGetUniformLocation(shader->ID, "COLOR"),
				color.x, color.y, color.z, color.w);

			glm::ivec2 windowSize;
			glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
			int max = windowSize.x > windowSize.y ? windowSize.x : windowSize.y;
			glUniform1f(glGetUniformLocation(shader->ID, "resolution"), max*max);
			glDrawArrays(GL_PATCHES, 0, numberOfVertexes);
		}
		vao.Unbind();
	}

	if (showBezierCurve) {
		lineDrawing.Activate();
		vao.Bind();
		{
			glUniform4f(glGetUniformLocation(lineDrawing.ID, "COLOR"),
				curveColor.x, curveColor.y, curveColor.z, curveColor.w);
			camera.SaveMatrixToShader(lineDrawing.ID);
			glDrawArrays(GL_LINE_STRIP, 0, numberOfVertexes ); //ContainerSize() + (ContainerSize() - 4) / 3
		}
		vao.Unbind();
	}
}


void BezierCurve::ActiveImGui() {
	ImGui::BeginGroup();
	{
		Figure::ActiveImGui();
		if (ImGui::RadioButton("Show Bezier's Curve", showBezierCurve))
			showBezierCurve = !showBezierCurve;
		ImGui::SameLine();
		if (ImGui::RadioButton("Show Bezier's polynomial", showBezierPol))
			showBezierPol = !showBezierPol;

		int i = 0;
		for (std::set<Figure* >::iterator iter = selectedFigures.begin();
			iter != selectedFigures.end(); iter++)
		{
			auto figure = (*iter);

			char buf[100];
			sprintf_s(buf, "%d. %s", i, figure->name);

			if (ImGui::Selectable(buf)) {
				Erase(figure);
				break;
			}
			i++;
		}
	}
	ImGui::EndGroup();
}

bool BezierCurve::Inputs(GLFWwindow* window, const Camera& camera) {
	return false;
}

bool BezierCurve::IsValid(Figure* figure) {
	return figure->GetType() == FigureType::Point;
}

void BezierCurve::Update() {
	if (!IsSomethingChange())
		return;

	CreateBezierCurve();
	somethingHasChange = false;
}

bool BezierCurve::AddContainer(FigureContainer* fc)
{
	this->UnMark();
	fc->Erase(this);
	return fc->Add(*(FigureContainer*)this);
}

void BezierCurve::CreateBezierCurve() {
	vao.Reactive();
	vao.Bind();

	glm::vec3 pos;
	std::vector<float> vs;
	int i = 1;
	for (auto iter = orderdFigures.begin(); iter != orderdFigures.end(); iter++)
	{
		pos = (*iter)->transpose->GetPosition();
		OpenGLHelper::AddVecToVector(vs, pos);
		if (i == 4) {
			OpenGLHelper::AddVecToVector(vs, pos);
			i = 1;
		}
		i++;
	}

	if (vs.size() / 3 % 4 == 1) {
		vs.push_back(pos.x); vs.push_back(pos.y); vs.push_back(pos.z);
	}

	if (vs.size() / 3 % 4 == 2) {
		glm::vec3 p1 = OpenGLHelper::TakeLastVecFromVector(vs);
		glm::vec3 p0 = OpenGLHelper::TakeLastVecFromVector(vs);
		glm::vec3 pMid = (p1 + p0) * 0.5f;
		
		OpenGLHelper::AddVecToVector(vs, p0);
		OpenGLHelper::AddVecToVector(vs, pMid);
		OpenGLHelper::AddVecToVector(vs, p1);
	}

	if (vs.size() / 3 % 4 == 3) {
		glm::vec3 p2 = OpenGLHelper::TakeLastVecFromVector(vs);
		glm::vec3 p1 = OpenGLHelper::TakeLastVecFromVector(vs);
		glm::vec3 p0 = OpenGLHelper::TakeLastVecFromVector(vs);
		glm::vec3 pMid_1 = (p0 + 2.0f * p1) / 3.0f;
		glm::vec3 pMid_2 = (2.0f * p1 + p2) / 3.0f;

		OpenGLHelper::AddVecToVector(vs, p0);
		OpenGLHelper::AddVecToVector(vs, pMid_1);
		OpenGLHelper::AddVecToVector(vs, pMid_2);
		OpenGLHelper::AddVecToVector(vs, p2);
	}

	numberOfVertexes = vs.size() / 3;

	VBO vbo(vs, GL_DYNAMIC_DRAW);

	vao.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

	vao.Unbind(); vbo.Unbind();

	glPatchParameteri(GL_PATCH_VERTICES, 4);
}