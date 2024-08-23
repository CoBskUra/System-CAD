#pragma once
#include "BezierBase.h"
#include "Figures/VirtualPoint.h"
#include "SceneManadement/Scene.h"
#include <imgui_impl_opengl3.h>
#include <Models/Surfaces/BezierSurfaceC2.h>




class BezierSurfaceC2: public BezierBase
{
	enum class CreationType {
		surface,
		cylinder,
		COUNT // Dodajemy COUNT na koñcu
	};

	inline const char* ToString(CreationType a) {
		switch (a)
		{
		case BezierSurfaceC2::CreationType::surface:
			return "surface";
			break;
		case BezierSurfaceC2::CreationType::cylinder:
			return "cylinder";
			break;
		default:
			break;
		}
	}


	


	glm::ivec2 windowSize;

	VAO vao;
	Shader* shader = StaticShaders::GetPointerToBezierSurfaceC2();

	VAO vao_curve;
	Shader* shader_curve = StaticShaders::GetPointerToBezierCurve();
	glm::vec4 curveColor{ 1, 0, 1, 1 };
	int numberOfIndes{};

	std::vector<std::shared_ptr<Point>> controlPoints;
	int numberOfVertexes{};
	int patch_div = 4;

	CreationType creationType = CreationType::surface;
	int verticalNum = 1, horizontalNum = 1;
	float radius = 1, height = 1;
	bool accepted = false, openWindow = true, firstTime = true;

public:
	Scene *refrenceScene;

	BezierSurfaceC2(const char* name);
	BezierSurfaceC2(MG1::BezierSurfaceC2 bs2, Scene* scene, int idOffset);
	BezierSurfaceC2();
	~BezierSurfaceC2();

	bool virtual Inputs(GLFWwindow* window, const Camera& camera);
	void virtual Draw(GLFWwindow* window, const Camera& camera);
	void virtual ActiveImGui();
	void TurnOffStartupInterfers();
	MG1::BezierSurfaceC2 Serialize(int idOffset) const;


protected:
	BezierSurfaceC2(const char* name, FigureType type);
	void virtual CreateBezier();
private:
	bool CreationWindowInterfers(glm::ivec2 appWindowSize);
	glm::vec3 GeneratePosForVertexInPatch(int verticalID, int horizontalID, int k1, int k2);
	void DeleteRangeControlPoints(int start, int end);
	Figure* TakePoint(int verticalID, int horizontalID, int k1, int k2) const;
	int TakeId(int verticalID, int horizontalID, int k1, int k2) const;
	int MaxSize();
};

