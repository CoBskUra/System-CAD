#pragma once

#include "SceneManadement/Scene.h"
#include "ShaderManadement/Shader.h"
#include "imgui.h"
#include "Helper/ImGuiExtenstions/imfilebrowser.h"
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
#include "Figures/Bezier/Surface/BezierSurfaceC0.h"
#include "Figures/Bezier/Surface/BezierSurfaceC2.h"
#include "SceneManadement/SceneSerializer.h"
#include <Figures/FigureFunctions/MergePoints.h>
#include "Figures/GregoryPatch.h"
#include "Windows/TransformSurfaceWindow.h"
#include "Windows/IntersectionWindow.h"
#include <PathGenerator/HeightMapGenerator.h>

class Manager {
	ImGui::FileBrowser fileDialog;
	char pathSource[200]{ "" };
	char pathDestination[200]{ "" };
	std::unique_ptr<Scene> sher_ptrScene = std::make_unique<Scene>();
	Camera* currentCamera;

	Camera* mainCamera;

	CenterPoint centerPoint;
	Cursor cursor;
	InfinityGrid infinityGrid;

	TransformSurfaceWindow transformSurface;
	IntersectionWindow intersectionWindow;

	GLFWwindow* window;
	bool mouseLeftFirstClick = true;
	glm::vec2 mouseLastPosition{};
	glm::vec2 mouseFirstClickPosition{};

	StereoscopicView stereoscopicView;
	HeightMapGenerator heightGenerator{ 1500 , 1500 , glm::vec3{0, 0, 0},  glm::vec3{3, 0, 3} };

	float minLengthFromMouse = 0.001f;
	int TheClosetFigureToMouse(FigureType figureType);
	
	void CreateFiguresInterfers();
	FigureType FigureFilter();
	void SelectableList(FigureType figureType = FigureType::Any);

	void SelectUnselect(int i);
	void LoadScene();
	void SaveScene();
	bool UnselectAllShortCut();
	bool MergeFigures();
	void ClearScene();
	void TransformSurfaceUsingCurve();
	void IntersectionWindow();
	void SelectManyWithMouse(FigureType figureType);

public:
	Manager(Camera* camera, GLFWwindow* window);
	void MenuInterferes();
	void Draw();
	void ProcessInput();

	~Manager();
};