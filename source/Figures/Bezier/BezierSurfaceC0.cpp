#include "BezierSurfaceC0.h"
#include <imgui_impl_opengl3.h>

BezierSurfaceC0::BezierSurfaceC0(const char* name) : BezierSurfaceC0(name, 
	FigureType::BezierSurfaceC0)
{
}

BezierSurfaceC0::BezierSurfaceC0(MG1::BezierSurfaceC0 bs2, Scene* scene, int idOffset) : BezierSurfaceC0()
{
	if (bs2.name != "")
		this->SetName(bs2.name.c_str());
	this->SetId(bs2.GetId() + idOffset);

	refrenceScene = scene;
	this->verticalNum = bs2.size.y;
	this->horizontalNum = bs2.size.x;
	/*if (verticalNum < horizontalNum) {
		std::swap(verticalNum, horizontalNum);
	}*/
	this->creationType = bs2.vWrapped ? CreationType::cylinder : CreationType::surface;
	controlPoints.resize(MaxSize());
	TurnOffStartupInterfers();
	numberOfVertexes = 0;

	for (int i = 0; i < verticalNum; i++) {
		for (int j = 0; j < horizontalNum; j++) {
			for (int k1 = 0; k1 < 4; k1++) {
				for (int k2 = 0; k2 < 4; k2++) {
					int cosId = bs2.patches.at(i * horizontalNum + j).controlPoints.at(k1 * 4 + k2).GetId();
					auto p = scene->byID(bs2.patches.at(i * horizontalNum + j).controlPoints.at(k1 * 4 + k2).GetId() + idOffset);
					p->SetObjectOwner(this);
					int id = TakeId(i, j, k1, k2);
					controlPoints[id] = std::static_pointer_cast<Point>(p);
					Add(p.get());
					numberOfVertexes++;
				}
			}
		}
	}

	//if (creationType == CreationType::cylinder) {
	//	for (int i = 0; i < verticalNum; i++)
	//	{
	//		for (int k2 = 1; k2 < 4; k2++)
	//		{
	//			for (int k1 = 0; k1 < 4; k1++)
	//			{
	//				int id = TakeId(i, horizontalNum - 1, k1, k2);
	//				if (controlPoints[id])
	//					std::cout << controlPoints[id]->GetId() << " ";
	//				controlPoints[id] = nullptr;
	//				//Erase(p);
	//			}
	//		}
	//	}
	//}

	if (creationType == CreationType::cylinder) {
		for (int i = 0; i < verticalNum; i++)
		{
			for (int k1 = 0; k1 < 4; k1++)
			{
				int id = TakeId(i, horizontalNum - 1, k1, 3);
				if (controlPoints[id])
					std::cout << controlPoints[id]->GetId() << " ";
				controlPoints[id] = nullptr;
			}
		}
	}

	CreateBezier();
}

BezierSurfaceC0::BezierSurfaceC0() : BezierSurfaceC0("BezierSurfaceC0")
{
}


BezierSurfaceC0::BezierSurfaceC0(const char* name, FigureType type): BezierBase(name, type)
{
	showBezierCurve = false;
	CreateBezier();
	SetUnmarkColor(glm::vec4(1, 1, 0, 1));
}

BezierSurfaceC0::~BezierSurfaceC0()
{
	for (int i = 0; i < controlPoints.size(); i++)
	{
		if (!controlPoints[i].get())
			continue;
		controlPoints[i]->RemoveOwner();
		controlPoints[i]->EraseContainer(this);
		controlPoints[i].reset();
	}
}

MG1::BezierSurfaceC0 BezierSurfaceC0::Serialize(int idOffset) const
{
	MG1::BezierSurfaceC0 bs0{};
	bs0.size.x = horizontalNum; bs0.size.y = verticalNum;

	bs0.vWrapped = creationType == CreationType::cylinder;

	for (int i = 0; i < verticalNum; i++) {
		for (int j = 0; j < horizontalNum; j++) {
			MG1::BezierPatchC0 patch{};
			for (int k1 = 0; k1 < 4; k1++) {
				for (int k2 = 0; k2 < 4; k2++) {
					MG1::PointRef pointRef{ TakePoint(i, j, k1, k2)->GetId() - idOffset };
					patch.controlPoints.push_back(pointRef);
				}
			}
			bs0.patches.push_back(patch);

		}
	}
	return bs0;
}

bool BezierSurfaceC0::Inputs(GLFWwindow* window, const Camera& camera)
{
	return false;
}

void BezierSurfaceC0::Draw(GLFWwindow* window, const Camera& camera)
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


void BezierSurfaceC0::ActiveImGui()
{
	if (CreationWindowInterfers(windowSize)) {
		if (!accepted)
			Clear();
		CreateBezier();
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

void BezierSurfaceC0::TurnOffStartupInterfers()
{
	openWindow = false;
	accepted = true;
	firstTime = false;
}

void BezierSurfaceC0::CreateBezier()
{
	numberOfVertexes = 0;
	std::vector<float> vs;
	std::vector<GLuint> ies;

	

	

	if (openWindow) {
		int tmp = MaxSize();
		while ((int)controlPoints.size() < tmp)
		{
			std::shared_ptr<Point> p = std::make_shared<Point>();
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
				for (int k1 = 0; k1 < 4; k1++)
				{
					auto p = TakePoint(i, horizontalNum - 1, k1, 3);
					Erase(p);
				}
			}
		}
	}
	else {
		DeleteRangeControlPoints(MaxSize(), controlPoints.size());
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
							if (j == horizontalNum - 1 && k2 == 3) {
								p = TakePoint(i, 0, k1, 0);

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

bool BezierSurfaceC0::CreationWindowInterfers(glm::ivec2 appWindowSize)
{
	if (!openWindow)
		return false;

	ImGui::GetWindowSize();
	bool receivedInput = false;
	ImGui::SetNextWindowFocus();
	ImGui::PushID("BezierSurfaceC0");
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
				if (nullptr != refrenceScene) {
					for (int i = 0; i < controlPoints.size(); i++) {
						if(controlPoints.at(i).get() && Contain(controlPoints.at(i).get()))
							refrenceScene->AddFigure(controlPoints.at(i));
					}
				}
			}
		}
		ImGui::EndGroup();
	}
	ImGui::End();
	ImGui::PopID();
	if (!openWindow)
		float aaaa = 1;

	return receivedInput;
}

glm::vec3 BezierSurfaceC0::GeneratePosForVertexInPatch(int verticalID, int horizontalID, int k1, int k2)
{
	float width = (  horizontalNum * 3);
	float patchNumVertical = ( verticalNum * 3);
	float alfa;
	float scalar = 1;

	glm::vec3 surPos = glm::vec3{ horizontalID * 3, 0,  verticalID * 3 } + glm::vec3{ k2, 0, k1 } - glm::vec3{ width / 2.0f, 0, patchNumVertical / 2.0f };
	switch (creationType)
	{
	case BezierSurfaceC0::CreationType::surface:
		return surPos;
		break;
	case BezierSurfaceC0::CreationType::cylinder:
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

void BezierSurfaceC0::DeleteRangeControlPoints(int start, int end)
{
	for (int i = start; i < end; i++)
	{
		if(controlPoints[i] != nullptr)
			controlPoints[i].reset();
	}

	auto iter = controlPoints.begin();
	controlPoints.erase(std::next(iter, start), std::next(iter, end));
}

Figure* BezierSurfaceC0::TakePoint(int verticalID, int horizontalID, int k1, int k2) const
{
	//int width = (4 + (horizontalNum - 1) * 3);
	//int id = (verticalID * 3 + k1) * width + k2 + horizontalID * 3;
	//std::cout << id << std::endl;
	auto p = controlPoints[TakeId(verticalID, horizontalID, k1, k2)];
	return p.get();
}

int BezierSurfaceC0::TakeId(int verticalID, int horizontalID, int k1, int k2) const
{
	int width = (4 + (horizontalNum - 1) * 3);
	int id = (verticalID * 3 + k1) * width + k2 + horizontalID * 3;
	return id;
}

int BezierSurfaceC0::MaxSize()
{
	int width = (4 + (horizontalNum - 1) * 3);
	int height = (4 + (verticalNum - 1) * 3);
	return width * height;
}
