#include "BazierCurve.h"

BezierCurve::BezierCurve(Shader* shader, const char* name) : BezierCurve(shader, "##BezierCurve", "BezierCurve")
{
	CreateBezierCurve();
	SetName(name);
}

BezierCurve::BezierCurve(Shader* shader) : BezierCurve(shader, "##BezierCurve", "BezierCurve")
{
	CreateBezierCurve();
	SetName("BezierCurve");
}

//BezierCurve::~BezierCurve()
//{
//	FigureContainer::~FigureContainer();
//	Figure::~Figure();
//}

BezierCurve::BezierCurve(Shader* shader, const char* uniqueName, const char* type) : Figure(shader, uniqueName, type) 
{}

void BezierCurve::Draw(const Camera& camera) {
	Update();
	if (showBezierPol) {
		shader->Activate();
		vao.Bind();
		{
			camera.SaveMatrixToShader(shader->ID);
			glUniform4f(glGetUniformLocation(shader->ID, "COLOR"),
				color.x, color.y, color.z, color.w);
			glUniform1i(glGetUniformLocation(shader->ID, "gNumSegments"), 1000);
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
	return figure->GetType() == "Point";
}




void BezierCurve::Update() {
	if (!IsSomethingChange())
		return;

	CreateBezierCurve();
	somethingHasChange = false;
}

void BezierCurve::CreateBezierCurve() {
	vao.Reactive();
	vao.Bind();

	std::vector<float> vs;
	int i = 1;
	for (auto iter = orderdFigures.begin(); iter != orderdFigures.end(); iter++)
	{
		auto pos = (*iter)->transpose->GetPosition();
		vs.push_back(pos.x); vs.push_back(pos.y); vs.push_back(pos.z);
		if (i == 4) {
			vs.push_back(pos.x); vs.push_back(pos.y); vs.push_back(pos.z);
			i = 1;
		}
		i++;
	}
	numberOfVertexes = vs.size()/3;
	VBO vbo(vs, GL_DYNAMIC_DRAW);

	vao.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

	vao.Unbind(); vbo.Unbind();

	glPatchParameteri(GL_PATCH_VERTICES, 4);
}