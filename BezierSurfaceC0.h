#pragma once
#include "BezierBase.h"
#include "VirtualPoint.h"
#include "FiguresVector.h"



class BezierSurfaceC0: public BezierBase
{
	enum class CreationType {
		surface,
		cylinder,
		COUNT // Dodajemy COUNT na koñcu
	};

	inline const char* ToString(CreationType a) {
		switch (a)
		{
		case BezierSurfaceC0::CreationType::surface:
			return "surface";
			break;
		case BezierSurfaceC0::CreationType::cylinder:
			return "cylinder";
			break;
		default:
			break;
		}
	}


	



	VAO vao;
	Shader* shader = StaticShaders::GetPointerToBezierSurfaceC0();
	std::vector<VirtualPoint*> virtualPoints;
	int numberOfVertexes;
	int patch_div = 40;

	CreationType creationType = CreationType::surface;
	int verticalNum = 1, horizontalNum = 1;
	float radius = 1, height = 1;
	bool accepted = false, openWindow = true, firstTime = true;

public:
	FiguresVector* figureVector;

	BezierSurfaceC0(const char* name);
	BezierSurfaceC0();
	~BezierSurfaceC0();

	bool virtual Inputs(GLFWwindow* window, const Camera& camera);
	void virtual Draw(GLFWwindow* window, const Camera& camera);
	void virtual ActiveImGui();
	bool virtual IsValid(Figure* figure);

protected:
	BezierSurfaceC0(const char* name, const char* uniqueName, FigureType type);
	void virtual CreateBezier();
private:
	bool CreationWindowInterfers(GLFWwindow* window);
	glm::vec3 GeneratePosForVertexInPatch(int verticalID, int horizontalID, int k1, int k2);
};

