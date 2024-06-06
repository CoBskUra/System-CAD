#include "BezierSurfaceC2.h"
#include <ImGui/imgui_impl_opengl3.h>

BezierSurfaceC2::BezierSurfaceC2(const char* name) : BezierSurfaceC2(name, "##BezierSurfaceC2", 
	FigureType::BezierSurfaceC2)
{
}

BezierSurfaceC2::BezierSurfaceC2() : BezierSurfaceC2("BezierSurfaceC2")
{
}


BezierSurfaceC2::BezierSurfaceC2(const char* name, const char* uniqueName, FigureType type): BezierBase(name, uniqueName, type)
{
	showBezierCurve = false;
	CreateBezier();
	SetUnmarkColor(glm::vec4(1, 1, 0, 1));
}

BezierSurfaceC2::~BezierSurfaceC2()
{
	for (int i = 0; i < controlPoints.size(); i++)
	{
		if (controlPoints[i]->NumberOfContainers() > 1)
		{
			controlPoints[i]->RemoveOwner();
			controlPoints[i]->EraseContainer(this);
		}
		else
			delete controlPoints[i];
	}
}

bool BezierSurfaceC2::Inputs(GLFWwindow* window, const Camera& camera)
{
	return false;
}

void BezierSurfaceC2::Draw(GLFWwindow* window, const Camera& camera)
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
		shader->Activate();
		vao.Bind();
		{
			glPatchParameteri(GL_PATCH_VERTICES, 16);

			camera.SaveMatrixToShader(shader->ID);
			glUniform4f(glGetUniformLocation(shader->ID, "COLOR"),
				showColor.x, showColor.y, showColor.z, showColor.w);
			glUniform1i(glGetUniformLocation(shader->ID, "PATCH_DIV"), patch_div);


			glUniform1i(glGetUniformLocation(shader->ID, "VERTICAL_DRAW"), false);
			glDrawArrays(GL_PATCHES, 0, numberOfVertexes);

			glUniform1i(glGetUniformLocation(shader->ID, "VERTICAL_DRAW"), true);
			glDrawArrays(GL_PATCHES, 0, numberOfVertexes);
		}
		vao.Unbind();
	}

	if (showBezierCurve) {
		shader_curve->Activate();
		vao_curve.Bind();
		{
			glUniform4f(glGetUniformLocation(shader_curve->ID, "COLOR"),
				curveColor.x, curveColor.y, curveColor.z, curveColor.w);
			camera.SaveMatrixToShader(shader_curve->ID);

			glDrawElements(GL_LINES, numberOfIndes, GL_UNSIGNED_INT, 0);
		}
		vao_curve.Unbind();
	}

	/*for (int i = 0; i < controlPoints.size(); i++) {
		controlPoints[i]->Draw(window, camera);
	}*/
}


void BezierSurfaceC2::ActiveImGui()
{
	if (CreationWindowInterfers(windowSize)) {
		if (!accepted)
			Clear();
		CreateBezier();
		if (!openWindow && !accepted)
		{
			figureVector->DeleteLastFigure();
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

void BezierSurfaceC2::CreateBezier()
{
	numberOfVertexes = 0;
	std::vector<float> vs;
	std::vector<GLuint> ies;

	int width = (4 + (horizontalNum - 1 )* 3);
	int height = (4 + (verticalNum - 1) * 3);

	

	if (openWindow) {
		int tmp = width * height;
		while ((int)controlPoints.size() < tmp)
		{
			Point* p = new Point();
			controlPoints.push_back(p);
			p->SetObjectOwner(this);
		}

		for (int i = 0; i < verticalNum; i++) {
			for (int j = 0; j < horizontalNum; j++) {
				for (int k1 = 0; k1 < 4; k1++) {
					for (int k2 = 0; k2 < 4; k2++) {
						glm::vec3 pos = GeneratePosForVertexInPatch(i, j, k1, k2);
						auto p = TakePoint(i, j, k1, k2);
						p->transpose->SetObjectPosition(pos);
						Add(p);
						OpenGLHelper::AddVecToVector(vs, pos);
						numberOfVertexes++;
					}
				}

			}
		}

		if (creationType == CreationType::cylinder) {
			for (int i = 0; i < verticalNum; i++)
			{
				for (int k2 = 1; k2 < 4; k2++)
				{
					for (int k1 = 0; k1 < 4; k1++)
					{
						auto p = TakePoint(i, horizontalNum - 1, k1, k2);
						Erase(p);
					}
				}
			}
		}
	}
	else {
		DeleteRangeControlPoints(width * height, controlPoints.size());
		if (creationType == CreationType::surface) 
		{
			for (int i = 0; i < verticalNum; i++) {
				for (int j = 0; j < horizontalNum; j++) {

					for (int k1 = 0; k1 < 4; k1++) {
						for (int k2 = 0; k2 < 4; k2++) {
							auto p = TakePoint(i, j, k1, k2);
							OpenGLHelper::AddVecToVector(vs, p->transpose->GetPosition());

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

		if (creationType == CreationType::cylinder) {
			for (int i = 0; i < verticalNum; i++) {
				for (int j = 0; j < horizontalNum; j++) {

					for (int k1 = 0; k1 < 4; k1++) {
						for (int k2 = 0; k2 < 4; k2++) {
							Figure* p;
							if (j >= horizontalNum - 3 && k2 >= horizontalNum - j)
							{
								p = TakePoint(i, 0, k1, k2 - (horizontalNum - j));
							}
							else
								p = TakePoint(i, j, k1, k2);
							OpenGLHelper::AddVecToVector(vs, p->transpose->GetPosition());

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
	}
	
	vao.Reactive();
	vao.Bind();
	{
		VBO vbo(vs, GL_DYNAMIC_DRAW);

		vao.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

		vao.Unbind(); vbo.Unbind();

		glPatchParameteri(GL_PATCH_VERTICES, 16);
	}

	vao_curve.Reactive();
	vao_curve.Bind();
	{
		VBO vbo(vs, GL_DYNAMIC_DRAW);
		EBO ebo(ies);

		vao_curve.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

		vao_curve.Unbind(); vbo.Unbind(); ebo.Unbind();
	}
	numberOfIndes = ies.size();
}

bool BezierSurfaceC2::CreationWindowInterfers(glm::ivec2 appWindowSize)
{
	if (!openWindow)
		return false;

	ImGui::GetWindowSize();
	bool receivedInput = false;
	ImGui::SetNextWindowFocus();
	ImGui::PushID(GetUniqueName().c_str());
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
				if (horizontalNum < 3)
					horizontalNum = 3;
				if (ImGui::DragInt("Vertical num:", &verticalNum, 1.0f, 1, 1000, "%d", ImGuiSliderFlags_AlwaysClamp) ||
					ImGui::DragInt("Horizontal num:", &horizontalNum, 3.0f, 1, 1000, "%d", ImGuiSliderFlags_AlwaysClamp)
					)
					receivedInput = true;
				if (ImGui::DragFloat("Radius:", &radius, 1.0f, M_ESP, M_FLOAT_MAX, "%f", ImGuiSliderFlags_AlwaysClamp) ||
					ImGui::DragFloat("Height:", &height, 1.0f, M_ESP, M_FLOAT_MAX, "%f", ImGuiSliderFlags_AlwaysClamp)
					)
					receivedInput = true;
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
				for (int i = 0; i < ContainerSize(); i++) {
					figureVector->AddFigure(At(i));
				}
			}
		}
		ImGui::EndGroup();
	}
	ImGui::End();
	ImGui::PopID();

	return receivedInput;
}

glm::vec3 BezierSurfaceC2::GeneratePosForVertexInPatch(int verticalID, int horizontalID, int k1, int k2)
{
	float width = ( 2 + horizontalNum );
	float patchNumVertical = (2 +  verticalNum );
	float alfa;
	float scalar = 1;

	glm::vec3 surPos = glm::vec3{ horizontalID , 0,  verticalID  } + glm::vec3{ k2, 0, k1 } - glm::vec3{ width / 2.0f, 0, patchNumVertical / 2.0f };
	switch (creationType)
	{
	case BezierSurfaceC2::CreationType::surface:
		return surPos;
		break;
	case BezierSurfaceC2::CreationType::cylinder:
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

void BezierSurfaceC2::DeleteRangeControlPoints(int start, int end)
{
	for (int i = start; i < end; i++)
	{
		if(controlPoints[i] != nullptr)
			delete controlPoints[i];
	}

	auto iter = controlPoints.begin();
	controlPoints.erase(std::next(iter, start), std::next(iter, end));
}

Figure* BezierSurfaceC2::TakePoint(int verticalID, int horizontalID, int k1, int k2)
{
	int width = (3 + horizontalNum );
	int id = (verticalID  + k1) * width + k2 + horizontalID ;
	//std::cout << id << std::endl;
	auto p = controlPoints[id];
	return p;
}
