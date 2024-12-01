#include "BezierSurface.h"

void BezierSurface::AddControlPointsToScene(Scene* scene)
{
	if (nullptr != scene) {
		for (int i = 0; i < controlPoints.size(); i++) {
			if (controlPoints.at(i).get() && Contain(controlPoints.at(i).get()))
				scene->AddFigure(controlPoints.at(i));
		}
	}
}

int BezierSurface::MaxSize()
{
	return horizontalNum * verticalNum * 16;
}

BezierSurface::BezierSurface(const char* name, FigureType type,
	const Shader& shader_surface, const Shader& shaderCurve) :
	shader{ shader_surface }, shader_curve{ shaderCurve }, BezierBase(name, type)
{
}

void BezierSurface::ResizeControlPoints()
{
	while ((int)controlPoints.size() < MaxSize())
	{
		std::shared_ptr<Point> p = std::make_shared<Point>();
		controlPoints.push_back(p);
		p->SetObjectOwner(this);
	}
}

bool BezierSurface::Inputs(GLFWwindow* window, const Camera& camera)
{
	return false;
}

bool BezierSurface::CreationWindowInterfers(glm::ivec2 appWindowSize)

{
	if (!openWindow)
		return false;

	canAdd = true;

	bool receivedInput = false;
	ImGui::SetNextWindowFocus();
	ImGui::SetNextWindowSize(ImVec2(300, 400));
	ImGui::PushID(GetId());
	if (ImGui::Begin("Generate C0 surface", &openWindow) && !openWindow) {
		openWindow = false;
		receivedInput = true;
	}
	{
		appWindowSize /= 2;
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 windowPos = ImVec2{ (float)appWindowSize.x - windowSize.x / 2 , (float)appWindowSize.y - windowSize.y / 2 };
		ImGui::SetWindowPos(windowPos);

		ImGui::BeginGroup();
		{

			for (int i = 0; i < OpenGLHelper::enum_count<CreationType>(); i++) {
				// Render radio button
				ImGui::SameLine();
				if (ImGui::RadioButton(ToString((CreationType)i), (int)creationType == i)) {
					creationType = (CreationType)i;
					receivedInput = true;
				}
			}

			switch (creationType)
			{
			case CreationType::surface:
				if (ImGui::DragInt("Vertical num:", &verticalNum, 1.0f, 1, 1000, "%d", ImGuiSliderFlags_AlwaysClamp) ||
					ImGui::DragInt("Horizontal num:", &horizontalNum, 1.0f, 1, 1000, "%d", ImGuiSliderFlags_AlwaysClamp)
					)
					receivedInput = true;
				break;
			case CreationType::cylinder:
				
				if (ImGui::DragInt("Vertical num:", &verticalNum, 1.0f, 1, 1000, "%d", ImGuiSliderFlags_AlwaysClamp) ||
					ImGui::DragInt("Horizontal num:", &horizontalNum, 3.0f, 1, 1000, "%d", ImGuiSliderFlags_AlwaysClamp)
					)
					receivedInput = true;
				if (ImGui::DragFloat("Radius:", &radius, 1.0f, M_ESP, M_FLOAT_MAX, "%f", ImGuiSliderFlags_AlwaysClamp) ||
					ImGui::DragFloat("Height:", &height, 1.0f, M_ESP, M_FLOAT_MAX, "%f", ImGuiSliderFlags_AlwaysClamp)
					)
					receivedInput = true; 
				if (horizontalNum < 3)
				{
					horizontalNum = 3;
				}
				break;
			default:
				break;
			}
			ImGui::DragInt("Patch div. :", &patch_div);
			if (ImGui::Button("Accept"))
			{
				accepted = true;
				openWindow = false;
				receivedInput = true;
				AddControlPointsToScene(refrenceScene);
				
			}
		}
		ImGui::EndGroup();
	}
	ImGui::End();
	ImGui::PopID();

	canAdd = false;
	return receivedInput;
}


void BezierSurface::Draw(GLFWwindow* window, const Camera& camera)
{
	if (openWindow) {
		glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
	}

	if (!showBezierPol && !showBezierCurve) {
		return;
	}

	if (IsSomethingChange())
		Update();

	if (showBezierPol) {
		auto showColor = GetShowColor();
		shader.Activate();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GetTextureId());
		vao.Bind();
		{
			glPatchParameteri(GL_PATCH_VERTICES, 16);

			camera.SaveMatrixToShader(shader.ID);
			glUniform4f(glGetUniformLocation(shader.ID, "COLOR"),
				showColor.x, showColor.y, showColor.z, showColor.w);
			glUniform1i(glGetUniformLocation(shader.ID, "PATCH_DIV"), patch_div);
			glUniform2i(glGetUniformLocation(shader.ID, "SIZE"), verticalNum, horizontalNum);


			glUniform1i(glGetUniformLocation(shader.ID, "VERTICAL_DRAW"), false);
			glDrawArrays(GL_PATCHES, 0, numberOfVertexes);

			glUniform1i(glGetUniformLocation(shader.ID, "VERTICAL_DRAW"), true);
			glDrawArrays(GL_PATCHES, 0, numberOfVertexes);
		}
		vao.Unbind();
	}

	if (showBezierCurve) {
		shader_curve.Activate();
		vao_curve.Bind();
		{
			glUniform4f(glGetUniformLocation(shader_curve.ID, "COLOR"),
				curveColor.x, curveColor.y, curveColor.z, curveColor.w);
			camera.SaveMatrixToShader(shader_curve.ID);

			glDrawElements(GL_LINES, numberOfIndes, GL_UNSIGNED_INT, 0);
		}
		vao_curve.Unbind();
	}

	/*for (int i = 0; i < ContainerSize(); i++)
		At(i)->Draw(window, camera);*/
}
void BezierSurface::CreateBezierVAO()
{
	numberOfVertexes = 0;
	std::vector<float> vs{};
	std::vector<GLuint> ies{};


	canAdd = true;

	if (openWindow) {
		ResizeControlPoints();

		for (int i = 0; i < verticalNum; i++) {
			for (int j = 0; j < horizontalNum; j++) {
				for (int k1 = 0; k1 < 4; k1++) {
					for (int k2 = 0; k2 < 4; k2++) {


						auto p = TakePoint(i, j, k1, k2);
						glm::vec3 pos = GeneratePosForVertexInPatch(i, j, k1, k2);
						p->transpose->SetObjectPosition(pos);

						Add(p);
						OpenGLHelper::AddVecToVector(vs, pos);
						vs.push_back(i * horizontalNum + j);

						if (k2 != 0)
						{
							ies.push_back(numberOfVertexes - 1);
							ies.push_back(numberOfVertexes);

						}
						if (k1 != 0)
						{
							ies.push_back(numberOfVertexes - 4);
							ies.push_back(numberOfVertexes);
						}

						numberOfVertexes++;

					}
				}
			}
		}
	}
	else {
		DeleteRangeControlPoints(MaxSize(), controlPoints.size());

		for (int i = 0; i < verticalNum; i++) {
			for (int j = 0; j < horizontalNum; j++) {
				for (int k1 = 0; k1 < 4; k1++) {
					for (int k2 = 0; k2 < 4; k2++) {

						auto p = TakePoint(i, j, k1, k2);
						OpenGLHelper::AddVecToVector(vs, p->transpose->GetPosition());
						vs.push_back(i * horizontalNum + j);

						if (k2 != 0)
						{
							ies.push_back(numberOfVertexes - 1);
							ies.push_back(numberOfVertexes);

						}
						if (k1 != 0)
						{
							ies.push_back(numberOfVertexes - 4);
							ies.push_back(numberOfVertexes);
						}

						numberOfVertexes++;
					}
				}
			}
		}
	}

	// save vertex and ies
	vao.Reactive();
	vao.Bind();
	{
		VBO vbo(vs, GL_DYNAMIC_DRAW);

		vao.LinkAttrib(0, 3, GL_FLOAT, false, 4 * sizeof(float), 0);
		vao.LinkAttrib(1, 1, GL_FLOAT, false, 4 * sizeof(float), (void*)( 3 * sizeof(float)));

		vao.Unbind(); vbo.Unbind();

		glPatchParameteri(GL_PATCH_VERTICES, 16);
	}

	vao_curve.Reactive();
	vao_curve.Bind();
	{
		VBO vbo(vs, GL_DYNAMIC_DRAW);
		EBO ebo(ies);

		vao_curve.LinkAttrib(0, 3, GL_FLOAT, false, 4 * sizeof(float), 0);

		vao_curve.Unbind(); vbo.Unbind(); ebo.Unbind();
	}
	numberOfIndes = ies.size();


	canAdd = false;
}

void BezierSurface::ActiveImGui()
{
	if (CreationWindowInterfers(windowSize)) {
		if (!accepted)
			Clear();
		CreateBezierVAO();
		if (!openWindow && !accepted)
		{
			if (nullptr != refrenceScene)
				refrenceScene->DeleteLastFigure();
			return;
		}
	}

	if (openWindow)
		return;

	ImGui::BeginGroup();
	{
		Figure::ActiveImGui();
		RotationInterfers();
		ScaleInterfers();
		ImGui::DragInt("Patch div. :", &patch_div);
		if (ImGui::RadioButton("Show Bezier's polynomial", showBezierPol))
			ChangeShowBezierPol();
		ImGui::SameLine();
		if (ImGui::RadioButton("Show Bezier's Curve", showBezierCurve))
			ChangeShowBezierCurve();

	}
	ImGui::EndGroup();
}

void BezierSurface::TurnOffStartupInterfers()
{
	openWindow = false;
	accepted = true;
	firstTime = false;
}
BezierSurface::CreationType BezierSurface::GetWrapType()
{
	return creationType;
}
void BezierSurface::DeleteRangeControlPoints(int start, int end)
{
	for (int i = start; i < end; i++)
	{
		if (controlPoints[i] != nullptr)
			controlPoints[i].reset();
	}

	auto iter = controlPoints.begin();
	controlPoints.erase(std::next(iter, start), std::next(iter, end));
}

glm::vec3 BezierSurface::GeneratePosForVertexInPatch(int verticalID, int horizontalID, int k1, int k2)
{
	float width = (horizontalNum * 3);
	float patchNumVertical = (verticalNum * 3);
	float alfa;
	float scalar = 1;

	glm::vec3 surPos = glm::vec3{ horizontalID * 3, 0,  verticalID * 3 } + glm::vec3{ k2, 0, k1 } - glm::vec3{ width / 2.0f, 0, patchNumVertical / 2.0f };
	switch (creationType)
	{
	case CreationType::surface:
		return surPos;
		break;
	case CreationType::cylinder:
		surPos.z *= 1.0f / patchNumVertical * this->height;
		//surPos.x *= 1.0f / width * 2 * radius;

		if (horizontalNum > 2) {
			alfa = M_PI * 2 / (float)horizontalNum;

			surPos.x = radius;
			if (k2 == 1) {
				surPos.y = 2.0f / 3.0f * tanf(alfa * 0.5f) * radius;
			}
			else if (k2 == 2) {
				surPos.y = -2.0f / 3.0f * tanf(alfa * 0.5f) * radius;
				surPos = MathOperations::RotationAlongAxis(surPos, alfa, glm::vec3{ 0, 0, 1 });
			}
			else if (k2 == 3) {
				surPos = MathOperations::RotationAlongAxis(surPos, alfa, glm::vec3{ 0, 0, 1 });
			}

			surPos = MathOperations::RotationAlongAxis(surPos, alfa * horizontalID, glm::vec3{ 0, 0, 1 });
		}
		else {
			surPos.z *= 1.0f / patchNumVertical * this->height;
			if (surPos.x < 0)
				scalar = -1;
			surPos.x *= scalar;
			surPos.x *= 1.0f / width * 2 * radius;
			surPos.x -= radius * 0.5f;
			surPos.x *= 2;
			surPos.y = scalar * sqrtf(powf(radius, 2.0f) - powf(surPos.x, 2));

		}
		return surPos;
		break;
	default:
		break;
	}

	return glm::vec3{ horizontalID * 3, 0,  verticalID * 3 } + glm::vec3{ k2, 0, k1 };;
}

bool BezierSurface::Swap(Figure* from, std::shared_ptr<Figure>  to)
{
	canAdd = true;
	if (!FigureContainer::Swap(from, to))
	{
		canAdd = false;
		return false;
	}

	for (int i = 0; i < controlPoints.size(); i++) {
		if (from == controlPoints[i].get())
		{
			controlPoints[i] = to;
		}
	}
	SomethingHasChange();
	canAdd = false;
	return true;
}

glm::vec4 BezierSurface::Cast_VU_To_PatchVPatchHVU(float v, float u)
{
	int patchV = v;
	if (patchV == SurfaceSize().x)
		patchV--;
	v = v - patchV;

	int patchH = u;
	if (patchH == SurfaceSize().y)
		patchH--;
	u = u - patchH;

	return { patchV, patchH, v, u };
}

glm::vec3 BezierSurface::Parametrization(float v, float u)
{
	glm::vec4 params = Cast_VU_To_PatchVPatchHVU(v, u);

	return Parametrization(params.x, params.y, params.z, params.w);
}

glm::vec3 BezierSurface::Derivative_u(float v, float u)
{
	glm::vec4 params = Cast_VU_To_PatchVPatchHVU(v, u);

	return DerivativeU(params.x, params.y, params.z, params.w);
}

glm::vec3 BezierSurface::Derivative_uu(float v, float u)
{
	return glm::vec3();
}

glm::vec3 BezierSurface::Derivative_v(float v, float u)
{
	glm::vec4 params = Cast_VU_To_PatchVPatchHVU(v, u);

	return DerivativeV(params.x, params.y, params.z, params.w);
}

glm::vec3 BezierSurface::Derivative_vv(float v, float u)
{
	glm::vec4 params = Cast_VU_To_PatchVPatchHVU(v, u);

	return DerivativeUU(params.x, params.y, params.z, params.w);
}

glm::vec3 BezierSurface::Derivative_vu(float v, float u)
{
	glm::vec4 params = Cast_VU_To_PatchVPatchHVU(v, u);

	return DerivativeVU(params.x, params.y, params.z, params.w);
}

glm::vec3 BezierSurface::Derivative_uv(float v, float u)
{
	glm::vec4 params = Cast_VU_To_PatchVPatchHVU(v, u);

	return DerivativeUV(params.x, params.y, params.z, params.w);
}

glm::vec2 BezierSurface::Field_u()
{
	return { 0, (float)SurfaceSize().y};
}

glm::vec2 BezierSurface::Field_v()
{
	return { 0, (float)SurfaceSize().x };
}

std::vector<glm::mat<4, 4, float>> BezierSurface::ControlPointsMatrix(int patchV, int patchH)
{
	glm::mat<4, 4, float> points_x{}, points_y{}, points_z{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			glm::vec3 pos = TakePoint(patchV, patchH, i, j)->transpose->GetPosition();
			points_x[i][j] = pos.x;
			points_y[i][j] = pos.y;
			points_z[i][j] = pos.z;
		}
	}
	return { points_x, points_y, points_z };
}
std::vector<glm::vec3> BezierSurface::ControlPointsPosVector(int patchV, int patchH)
{
	std::vector<glm::vec3> poses;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			glm::vec3 pos = TakePoint(patchV, patchH, j, i)->transpose->GetPosition();
			poses.push_back(pos);
		}
	}
	return poses;
}
bool BezierSurface::IsValid(Figure* figure)
{
	return canAdd && BezierBase::IsValid(figure);
}

glm::ivec2 BezierSurface::SurfaceSize()
{
	return glm::ivec2{verticalNum, horizontalNum};
}

