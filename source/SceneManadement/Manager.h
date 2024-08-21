#pragma once

#include "SceneManadement/Scene.h"
#include "ShaderManadement/Shader.h"
#include "imgui.h"
#include "Helper/OpenGLHelper.h"
#include "Camera.h"
#include "ShaderRefrence/StaticShaders.h"

#include "Figures/Figure.h"
#include "Figures/Torus.h"
#include "Figures/Point.h"
#include "Figures/Cursor.h"
#include "Figures/CanterPoint.h"

#include "Figures/InfinityGrid.h"
#include "Figures/FigureFunctions/StereoscoicView.h"

#include "Figures/Bezier/BezierC0.h"
#include "Figures/Bezier/BezierC2.h"
#include "Figures/Bezier/BezierInterpolated.h"
#include "Figures/Bezier/BezierSurfaceC0.h"
#include "Figures/Bezier/BezierSurfaceC2.h"

class Manager {
	Scene scene;
	Camera* currentCamera;

	Camera* mainCamera;

	CenterPoint centerPoint;
	Cursor cursor;
	InfinityGrid infinityGrid;

	GLFWwindow* window;
	bool mouseLeftFirstClick = true;
	glm::vec2 mouseLastPosition;

	StereoscopicView stereoscopicView;

	float minLengthFromMouse = 0.001f;
	int TheClosetFigureToMouse(FigureType figureType);
	
	void CreateFiguresInterfers();
	void SelectableList();

	void Select(int i);
public:
	Manager(Camera* camera, GLFWwindow* window);
	void MenuInterferes();
	void Draw();
	void ProcesInput();

	~Manager();
};