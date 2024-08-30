#pragma once
#include "SceneManadement/Scene.h"
#include "Scene/SceneSerializer.h"
#include "Models/SceneObject.h"

#include <Figures/Point.h>
#include <Figures/Torus.h>
#include <Figures/Bezier/BezierC0.h>
#include <Figures/Bezier/BezierC2.h>
#include <Figures/Bezier/BezierInterpolated.h>
#include "Scene/Scene.h"
#include <iostream>
#include "Figures/Bezier/Surface/BezierSurfaceC0.h"
#include "Figures/Bezier/Surface/BezierSurfaceC2.h"

#include <exception>
#include <typeinfo>
#include <stdexcept>

class SceneSerializer {
public:
	std::unique_ptr<Scene> LoadScene(const char* source, std::unique_ptr<Scene> uniqe_ptr_scene);
	void SaveScene(const Scene &scene, const char* source);
};