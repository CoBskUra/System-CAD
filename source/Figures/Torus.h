#ifndef Torus_CLASS_H
#define Torus_CLASS_H

#include "ShaderManadement/Shader.h"
#include "ShaderManadement/VAO.h"
#include "ShaderManadement/VBO.h"
#include "ShaderManadement/EBO.h"
#include <glm/gtc/type_ptr.hpp>
#include "Constants.h"
#include "Figures/Figure3D.h"
#include "ShaderRefrence/StaticShaders.h"
#include "Models/Torus.h"
#include "Intersection/IntersectionAble.h"
#include "Helper/OpenGLHelper.h"

class Torus: public Figure3D, public IntersectionAble
{
public:
	Torus(const char* name);

	Torus();

	Torus(MG1::Torus tori, int offsetId);
	MG1::Torus Serialize(int idOffset) const;


	void  Draw(GLFWwindow* window, const Camera& camera)  override;
	//void  ActiveImGui() override;
	void  FigureSpecificImGui() override;

	inline glm::vec3 TorusLocalParametryzation(float v, float u);
	glm::vec3 Parametrization(float v, float u) override;
	glm::vec3 Derivative_u(float v , float u ) override;
	glm::vec3 Derivative_uu(float v, float u) override;

	glm::vec3 Derivative_v(float v , float u ) override;
	glm::vec3 Derivative_vv(float v, float u) override;

	glm::vec3 Derivative_vu(float v, float u ) override;
	glm::vec3 Derivative_uv(float v, float u) override;

	glm::vec2 Field_u() override;
	glm::vec2 Field_v() override;
	glm::bvec2 CanWrap() override; 

private:
	int numberOfElements;
	const Shader& torusShader = StaticShaders::GetTorus();
	VAO vao_torus;
	int segment1 = 10;
	int segment2 = 10;
	float R = 0.5f;
	float r = 0.2f;

	std::vector<float>  createTorusVertexBuffer();

	std::vector<GLuint>  createTorusIndeces();

	void CreateTorusVAO();
};
#endif