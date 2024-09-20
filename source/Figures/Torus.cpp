#include "Torus.h"

Torus::Torus(const char* name) : Figure3D(FigureType::Torus)
{
	CreateTorusVAO();
	SetName(name);
}

Torus::Torus() : Torus("Torus")
{
}

Torus::Torus(MG1::Torus tori, int offsetId) :Torus()
{
	this->transpose->SetObjectPosition(tori.position.x, tori.position.y, tori.position.z);
	Rotation rotation{};
	rotation.SetRotation_X(tori.rotation.x * M_PI / 180.0);
	rotation.SetRotation_Y(tori.rotation.y * M_PI / 180.0);
	rotation.SetRotation_Z(tori.rotation.z * M_PI / 180.0);
	this->scale.SetScale(tori.scale.x, tori.scale.y, tori.scale.z);
	this->quaternion.SetMainQuaternion((Quaternion)rotation.Get_R());
	if (tori.name != "")
		this->SetName(tori.name.c_str());
	this->SetId(tori.GetId() + offsetId);
}

MG1::Torus Torus::Serialize(int idOffset) const
{
	MG1::Torus tori{};
	tori.SetId(GetId() - idOffset);
	tori.name = name;
	tori.position.x = GetPosition().x;
	tori.position.y = GetPosition().y;
	tori.position.z = GetPosition().z;
	auto angles = quaternion.GetEulerRadXYZ();
	tori.rotation.x = (angles.x / M_PI * 180.0);
	tori.rotation.y = (angles.y / M_PI * 180.0);
	tori.rotation.z = (angles.z / M_PI * 180.0);
	glm::vec3 scale = this->scale.GetScaleVec();
	tori.scale.x = scale.x;
	tori.scale.y = scale.y;
	tori.scale.z = scale.z;
	return tori;
}

void Torus::Draw(GLFWwindow* window, const Camera& camera)
{
	torusShader.Activate();
	vao_torus.Bind();

	auto showColor = GetShowColor();

	glUniformMatrix4fv(glGetUniformLocation(torusShader.ID, "MODEL_MATRIX"),
		1, GL_FALSE, glm::value_ptr(GetModelMatrix()));
	glUniform4f(glGetUniformLocation(torusShader.ID, "COLOR"), showColor.x, showColor.y, showColor.z, showColor.w);
	camera.SaveMatrixToShader(torusShader.ID);

	glDrawElements(GL_LINE_STRIP, 2 * segment1 * segment2 + segment1 + segment2, GL_UNSIGNED_INT, 0);
	vao_torus.Unbind();
}



void Torus::FigureSpecificImGui()
{
	ImGui::BeginGroup();
	{
		ImGui::Text("Torus parameters");
		bool any = false;
		if (ImGui::DragInt("segment 1##Torus", &segment1, 1))
			any = true;
		if (ImGui::DragInt("segment 2##Torus", &segment2, 1))
			any = true;
		if (ImGui::DragFloat("R##Torus", &R, 0.1f, M_ESP))
			any = true;
		if (ImGui::DragFloat("r##Torus", &r, 0.1f, M_ESP))
			any = true;
		if (any)
			CreateTorusVAO();
	}
	ImGui::EndGroup();
}

inline glm::vec3 Torus::TorusLocalParametryzation(float v, float u)
{
	float x = (R + r * cosf(v)) * cosf(u);
	float y = r * sinf(v);
	float z = (R + r * cosf(v)) * sinf(u);
	return { x, y, z };
}

glm::vec3 Torus::Parametrization(float v, float u)
{
	
	auto tmp = GetModelMatrix()* glm::vec4{ TorusLocalParametryzation(v, u), 1 };
	return tmp / tmp.w;
}

glm::vec3 Torus::Derivative_u(float v, float u)
{
	return GetModelMatrix() * glm::vec4{
		-(R + r * cosf(v)) * sinf(u),
		0, 
		(R + r * cosf(v) ) * cosf(u),
		0};
}

glm::vec3 Torus::Derivative_uu(float v, float u)
{
	return GetModelMatrix() * glm::vec4{
		-(R + r * cosf(v)) * cosf(u), 
		0,
		-(R + r * cosf(v)) * sinf(u), 
		0 };
}

glm::vec3 Torus::Derivative_v(float v, float u)
{
	return GetModelMatrix() * glm::vec4{  
		- r * sinf(v) * cosf(u),
		r * cosf(v),
		-r * sinf(v) * sinf(u), 
		0};
}

glm::vec3 Torus::Derivative_vv(float v, float u)
{
	return GetModelMatrix() * glm::vec4{
		-r * cosf(v) * cosf(u),
		- r * sinf(v),
		-r * cosf(v) * sinf(u), 
		0 };
}

glm::vec3 Torus::Derivative_vu(float v, float u)
{
	return GetModelMatrix() * glm::vec4{ 
		r * sinf(v) * sinf(u), 
		0, 
		-r * sinf(v) * cosf(u) , 
		0};
}

glm::vec3 Torus::Derivative_uv(float v, float u)
{
	return GetModelMatrix() * glm::vec4{
		r * sinf(v) * sinf(u), 
		0, 
		- r * sinf(v) * cosf(u), 
		0 };
}

glm::vec2 Torus::Field_u()
{
	return { 0, 2.0 * M_PI };
}

glm::vec2 Torus::Field_v()
{
	return { 0, 2.0 * M_PI };
}

glm::bvec2 Torus::CanWrap()
{
	return { true, true };
}

void Torus::CreateTorusVAO() {
	vao_torus.Reactive();
	vao_torus.Bind();
	auto vs = createTorusVertexBuffer();
	auto ies = createTorusIndeces();
	VBO vbo(vs, GL_DYNAMIC_DRAW);
	EBO ebo(ies);

	vao_torus.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

	vao_torus.Unbind(); vbo.Unbind(); ebo.Unbind();
}

std::vector<GLuint>  Torus::createTorusIndeces(){
	std::vector<GLuint> indeces;
	for (int i = 0; i < segment1; i++) {
		for (int j = 0; j < segment2; j++) {
			indeces.push_back(segment2 * i + j);
		}
		indeces.push_back(segment2 * i);
	}

	for (int j = 0; j < segment2; j++) {
		for (int i = 0; i < segment1; i++) {
			indeces.push_back(segment2 * i + j);
		}
		indeces.push_back(j);
	}

	return indeces;
}

std::vector<float> Torus::createTorusVertexBuffer(){
	std::vector<float> vertices;
	for (int i = 0; i < segment1; i++) {
		float theta1 = 2.0 * M_PI * static_cast<float>(i) / segment1;

		for (int j = 0; j < segment2; j++) {
			float phi1 = 2.0 * M_PI * static_cast<float>( j) / segment2;

			auto pos = TorusLocalParametryzation(theta1, phi1);
			OpenGLHelper::AddVecToVector(vertices, pos);

		}
	}
	return vertices;
}