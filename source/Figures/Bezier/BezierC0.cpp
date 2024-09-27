#include "BezierC0.h"
#include "Helper/OpenGLHelper.h"

BezierC0::BezierC0( const char* name) : BezierC0(name, FigureType::BezierC0)
{}

BezierC0::BezierC0() : BezierC0("BezierC0")
{}

BezierC0::BezierC0(MG1::BezierC0 b0, const Scene * scene, int idOffset) :BezierC0() {
	for (auto refPoint : b0.controlPoints) {
		uint32_t id = refPoint.GetId() + idOffset;
		this->Add(scene->byID(id).get());
	}
	if (b0.name != "")
		this->SetName(b0.name.c_str());
	this->SetId(b0.GetId() + idOffset);
}

MG1::BezierC0 BezierC0::Serialize(int idOffset) const
{
	MG1::BezierC0 bezier{};
	bezier.SetId(GetId() - idOffset);
	bezier.name = name;
	for (int i = 0; i < ContainerSize(); i++)
	{
		MG1::PointRef p{ At(i)->GetId() - idOffset };

		bezier.controlPoints.push_back(p);
	}
	return bezier;
}


BezierC0::BezierC0(const char* name, FigureType type) : BezierCurve( name, type)
{
	CreateBezierVAO();
	SetUnmarkColor(glm::vec4(1, 1, 0, 1));
}

glm::vec3 BezierC0::Derivative(float t)
{
	int segmentId = (int)(NumberOfSegments() * t);
	if (segmentId == NumberOfSegments())
		segmentId = segmentId - 1;
	float newT = ((float)NumberOfSegments()) * t - segmentId;

	std::vector<Figure*> points = ControlPointsInSegment(segmentId);
	std::vector<glm::vec3> pos;
	pos.reserve(points.size());

	for (auto figure : points) {
		pos.push_back(figure->transpose->GetPosition());
	}

	return MathOperations::BezierNDerivative(newT, pos);
}

glm::vec3 BezierC0::PositionOnCurve(float t)
{
	int segmentId = (int)(NumberOfSegments() * t);
	if (segmentId == NumberOfSegments())
		segmentId = segmentId - 1;
	float newT = ((float)NumberOfSegments()) * t - segmentId;

	std::vector<Figure*> points = ControlPointsInSegment(segmentId);
	std::vector<glm::vec3> pos;
	pos.reserve(points.size());

	for (auto figure : points) {
		pos.push_back(figure->transpose->GetPosition());
	}

	return MathOperations::BezierND(newT, pos);
}

void BezierC0::Draw(GLFWwindow* window, const Camera& camera) {
	if(IsSomethingChange())
		Update();
	if (showBezierPol) {
		glm::ivec2 windowSize;
		glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
		int max = windowSize.x > windowSize.y ? windowSize.x : windowSize.y;
		auto showColor = GetShowColor();

		shader_bezier3D.Activate();
		vao_bezier3D.Bind();
		{

			glPatchParameteri(GL_PATCH_VERTICES, 4);
			camera.SaveMatrixToShader(shader_bezier3D.ID);
			glUniform4f(glGetUniformLocation(shader_bezier3D.ID, "COLOR"),
				showColor.x, showColor.y, showColor.z, showColor.w);

			glUniform1f(glGetUniformLocation(shader_bezier3D.ID, "resolution"), max * max);
			for (int i = 0; i < 10; i++) {
				glUniform1f(glGetUniformLocation(shader_bezier3D.ID, "numberOfSegments"), 10);
				glUniform1f(glGetUniformLocation(shader_bezier3D.ID, "segmentId"), i);
				glDrawArrays(GL_PATCHES, 0, numberOfVertexes);
			}
		}
		vao_bezier3D.Unbind();

		shader_bezier2D.Activate();
		vao_bezier2D.Bind();
		{
			glPatchParameteri(GL_PATCH_VERTICES, 3);
			auto showColor = GetShowColor();
			camera.SaveMatrixToShader(shader_bezier2D.ID);
			glUniform4f(glGetUniformLocation(shader_bezier2D.ID, "COLOR"),
				showColor.x, showColor.y, showColor.z, showColor.w);

			glUniform1f(glGetUniformLocation(shader_bezier2D.ID, "resolution"), max * max );
			int numberOfsegments = 5;
			for (int i = 0; i < numberOfsegments; i++) {
				glUniform1f(glGetUniformLocation(shader_bezier2D.ID, "numberOfSegments"), numberOfsegments);
				glUniform1f(glGetUniformLocation(shader_bezier2D.ID, "segmentId"), i);
				glDrawArrays(GL_PATCHES, 0, 3);
			}
		}
		vao_bezier2D.Unbind();
	}

	if (showBezierCurve) {
		lineDrawingShader.Activate();
		auto negShowColor = glm::vec4(1, 1, 1, 1) - GetShowColor();
		negShowColor.a = 1 - negShowColor.a;
		vao_bezier3D.Bind();
		{
			glUniform4f(glGetUniformLocation(lineDrawingShader.ID, "COLOR"),
				negShowColor.x, negShowColor.y, negShowColor.z, negShowColor.w);
			camera.SaveMatrixToShader(lineDrawingShader.ID);
			glDrawArrays(GL_LINE_STRIP, 0, numberOfVertexes); //ContainerSize() + (ContainerSize() - 4) / 3
		}
		vao_bezier3D.Unbind();


		vao_bezier2D.Bind();
		{
			glUniform4f(glGetUniformLocation(lineDrawingShader.ID, "COLOR"),
				negShowColor.x, negShowColor.y, negShowColor.z, negShowColor.w);
			camera.SaveMatrixToShader(lineDrawingShader.ID);
			glDrawArrays(GL_LINE_STRIP, 0, 3); //ContainerSize() + (ContainerSize() - 4) / 3
		}
		vao_bezier2D.Unbind();
	}
}

void BezierC0::CreateBezierVAO() {
	vao_bezier3D.Reactive();
	vao_bezier2D.Reactive();
	bezier2D_on = false;


	glm::vec3 pos;
	std::vector<float> vs;
	int i = 1;
	for (auto iter = orderdFigures.begin(); iter != orderdFigures.end(); iter++)
	{
		pos = (*iter)->transpose->GetPosition();
		OpenGLHelper::AddVecToVector(vs, pos);
		if (i == 4 && ContainerSize() > 4) {
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


		std::vector<float> vs_bezier2D;

		OpenGLHelper::AddVecToVector(vs_bezier2D, p0);
		OpenGLHelper::AddVecToVector(vs_bezier2D, p1);
		OpenGLHelper::AddVecToVector(vs_bezier2D, p2);

		vao_bezier2D.Bind();
		VBO vbo(vs_bezier2D, GL_DYNAMIC_DRAW);

		vao_bezier2D.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

		vao_bezier2D.Unbind(); vbo.Unbind();
		bezier2D_on = true;
	}

	numberOfVertexes = vs.size() / 3;

	vao_bezier3D.Bind();
	VBO vbo(vs, GL_DYNAMIC_DRAW);

	vao_bezier3D.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

	vao_bezier3D.Unbind(); vbo.Unbind();
}

int BezierC0::NumberOfSegments()
{
	if (ContainerSize() < 4)
	{
		return 1;
	}


	return 1 + (ContainerSize() - 4) / 3  + ((ContainerSize() - 4) % 3 != 0);
}

std::vector<Figure*> BezierC0::ControlPointsInSegment(int segmentId)
{
	const int numberOfSegments = NumberOfSegments();
	if(numberOfSegments <= segmentId)
		throw std::out_of_range("Bad idx passed to At()");

	std::vector<Figure*> points;
	int start = segmentId * 3;
	int i = 0;
	for (start; start < ContainerSize() && i < 4; start++, i++) {
		points.push_back(At(start));
	}


	return points;
}
