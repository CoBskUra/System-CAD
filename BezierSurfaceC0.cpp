#include "BezierSurfaceC0.h"
#include <ImGui/imgui_impl_opengl3.h>

BezierSurfaceC0::BezierSurfaceC0(const char* name) : BezierSurfaceC0(name, "##BezierSurfaceC0", 
	FigureType::BezierSurfaceC0)
{
}

BezierSurfaceC0::BezierSurfaceC0() : BezierSurfaceC0("BezierSurfaceC0")
{
}


BezierSurfaceC0::BezierSurfaceC0(const char* name, const char* uniqueName, FigureType type): BezierBase(name, uniqueName, type)
{
	CreateBezier();
	SetColor(glm::vec4(1, 0, 0, 1));
}

BezierSurfaceC0::~BezierSurfaceC0()
{
	for (int i = 0; i < virtualPoints.size(); i++)
	{
		delete virtualPoints[i];
	}
}

bool BezierSurfaceC0::Inputs(GLFWwindow* window, const Camera& camera)
{

	bool any = false;
	for (int i = 0; i < virtualPoints.size(); i++)
	{
		if (virtualPoints.at(i)->Inputs(window, camera)) {
			any = true;
			break;
		}
	}

	return any;
}

void BezierSurfaceC0::Draw(GLFWwindow* window, const Camera& camera)
{
	if (CreationWindowInterfers(window)) {
		if(!accepted)
			Clear();
		CreateBezier();
	}

	if (IsSomethingChange())
		Update();

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

	for (int i = 0; i < ContainerSize(); i++) {
		At(i)->Draw(window, camera);
	}
}

void BezierSurfaceC0::ActiveImGui()
{
	ImGui::BeginGroup();
	{
		Figure::ActiveImGui();
		RotationInterfers();
		ScaleInterfers();
		ImGui::DragInt("Patch div. :", &patch_div);

	}
	ImGui::EndGroup();
}

bool BezierSurfaceC0::IsValid(Figure* figure)
{
	return figure->GetType() == FigureType::VirtualPoint;
}

void BezierSurfaceC0::CreateBezier()
{
	numberOfVertexes = 0;
	std::vector<float> vs;
	int width = (4 + (horizontalNum - 1 )* 3);
	int height = (4 + (verticalNum - 1) * 3);
	if (openWindow) {
		int tmp = width * height;
		while ((int)virtualPoints.size() < tmp)
		{
			VirtualPoint* p = new VirtualPoint();
			virtualPoints.push_back(p);
		}

		for (int i = 0; i < verticalNum; i++) {
			for (int j = 0; j < horizontalNum; j++) {
				for (int k1 = 0; k1 < 4; k1++) {
					for (int k2 = 0; k2 < 4; k2++) {
						glm::vec3 pos = GeneratePosForVertexInPatch(i, j, k1, k2);

						int id = (i * 3 + k1) * width +  k2 + j * 3;
						auto p = virtualPoints[id];
						p->transpose->SetObjectPosition(pos);
						Add(p);
						OpenGLHelper::AddVecToVector(vs, pos);
						numberOfVertexes++;
					}
				}

			}
		}
	}
	else {
		for (int i = 0; i < verticalNum; i++) {
			for (int j = 0; j < horizontalNum; j++) {
				for (int k1 = 0; k1 < 4; k1++) {
					for (int k2 = 0; k2 < 4; k2++) {
						int id = (i * 3 + k1) * width + k2 + j * 3;
						auto p = virtualPoints[id];
						OpenGLHelper::AddVecToVector(vs, p->transpose->GetPosition());
						numberOfVertexes++;
					}
				}
			}
		}
	}
	
	vao.Reactive();
	vao.Bind();
	VBO vbo(vs, GL_DYNAMIC_DRAW);

	vao.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

	vao.Unbind(); vbo.Unbind();

	glPatchParameteri(GL_PATCH_VERTICES, 16);
}

bool BezierSurfaceC0::CreationWindowInterfers(GLFWwindow* window)
{
	if (!openWindow)
		return false;

	glm::ivec2 appWindowSize;
	glfwGetWindowSize(window, &appWindowSize.x, &appWindowSize.y);
	ImGui::GetWindowSize();
	bool receivedInput = false;
	ImGui::SetNextWindowFocus();
	ImGui::PushID("BezierSurfaceC0");
	if (!ImGui::Begin("Generate C0 surface", &openWindow)) {
		openWindow = false;
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
					ImGui::DragInt("Horizontal num:", &horizontalNum, 1.0f, 1, 1000, "%d", ImGuiSliderFlags_AlwaysClamp)
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

glm::vec3 BezierSurfaceC0::GeneratePosForVertexInPatch(int verticalID, int horizontalID, int k1, int k2)
{
	float width = (  horizontalNum * 3);
	float patchNumVertical = ( verticalNum * 3);
	float scalar = 1;

	glm::vec3 surPos = glm::vec3{ horizontalID * 3, 0,  verticalID * 3 } + glm::vec3{ k2, 0, k1 } - glm::vec3{ width / 2.0f, 0, patchNumVertical / 2.0f };
	switch (creationType)
	{
	case BezierSurfaceC0::CreationType::surface:
		return surPos;
		break;
	case BezierSurfaceC0::CreationType::cylinder:
		surPos.z *= 1.0f / patchNumVertical * this->height;
		if (surPos.x < 0)
			scalar = -1;
		surPos.x *= scalar;
		surPos.x *= 1.0f / width * 2 * radius;
		surPos.x -= radius * 0.5f;
		surPos.x *= 2;
		surPos.y = scalar * sqrtf(powf(radius, 2.0f) - powf(surPos.x, 2));
		return surPos;
		break;
	default:
		break;
	}

	return glm::vec3{ horizontalID * 3, 0,  verticalID * 3 } + glm::vec3{ k2, 0, k1 };;
}
