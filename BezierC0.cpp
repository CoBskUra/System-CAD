#include "BezierC0.h"
#include "OpenGLHelper.h"

BezierC0::BezierC0(Shader* shader, const char* name) : BezierC0(shader, name,"##BezierC0", FigureType::BezierC0)
{}

BezierC0::BezierC0(Shader* shader) : BezierC0(shader, "BezierC0")
{}

BezierC0::BezierC0(Shader* shader, const char* name, const char* uniqueName, FigureType type) : CenterPoint(shader, name, uniqueName, type)
{
	CreateBezier();
	SetColor(glm::vec4(1, 0, 0, 1));
}

void BezierC0::Draw(GLFWwindow* window, const Camera& camera) {
	Update();
	if (showBezierPol) {
		shader->Activate();
		vao.Bind();
		{

			auto showColor = GetShowColor();
			camera.SaveMatrixToShader(shader->ID);
			glUniform4f(glGetUniformLocation(shader->ID, "COLOR"),
				showColor.x, showColor.y, showColor.z, showColor.w);

			glm::ivec2 windowSize;
			glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
			int max = windowSize.x > windowSize.y ? windowSize.x : windowSize.y;
			glUniform1f(glGetUniformLocation(shader->ID, "resolution"), max*max);
			for (int i = 0; i < 10; i++) {
				glUniform1f(glGetUniformLocation(shader->ID, "numberOfSegments"), 10);
				glUniform1f(glGetUniformLocation(shader->ID, "segmentId"), i);
				glDrawArrays(GL_PATCHES, 0, numberOfVertexes);
			}
		}
		vao.Unbind();
	}

	if (showBezierC0) {
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


void BezierC0::ActiveImGui() {
	ImGui::BeginGroup();
	{
		Figure::ActiveImGui();
		RotationInterfers();
		ScaleInterfers();

		if (ImGui::RadioButton("Show Bezier's Curve", showBezierC0))
			ChangeShowBezierC0();
		ImGui::SameLine();
		if (ImGui::RadioButton("Show Bezier's polynomial", showBezierPol))
			ChangeShowBezierPol(); 

		SelectedPoints();
	}
	ImGui::EndGroup();
}

bool BezierC0::Inputs(GLFWwindow* window, const Camera& camera) {
	return false;
}

bool BezierC0::IsValid(Figure* figure) {
	return figure->GetType() == FigureType::Point;
}

void BezierC0::Update() {
	if (!IsSomethingChange())
		return;
	CenterPoint::Update();
	CreateBezier();
	somethingHasChange = false;
}

bool BezierC0::AddContainer(FigureContainer* fc)
{
	this->UnMark();
	fc->Erase(this);
	return fc->Add(*(FigureContainer*)this);
}

void BezierC0::MarkFigure(Figure* f)
{
}

void BezierC0::CreateBezier() {
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

void BezierC0::SelectedPoints()
{
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

void BezierC0::ChangeShowBezierC0()
{
	showBezierC0 = !showBezierC0;
}

void BezierC0::ChangeShowBezierPol()
{
	showBezierPol = !showBezierPol;
}
