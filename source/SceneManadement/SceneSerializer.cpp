#include "SceneSerializer.h"


std::unique_ptr<Scene> SceneSerializer::LoadScene(const char* source, std::unique_ptr<Scene> uniqe_ptr_scene)
{
	try {
		MG1::SceneSerializer outsideSceneSerializer{};
		MG1::Scene outsideScene = outsideSceneSerializer.LoadScene(source);
		//uniqe_ptr_scene = std::make_unique<Scene>();
		int offsetId = Figure::LargestTakenId() + 1;

		for (MG1::Point mg1_point : outsideScene.points)
		{
			std::shared_ptr<Point> point = std::make_shared<Point>(mg1_point, offsetId);
			uniqe_ptr_scene->AddFigure(point);
		}

		for (MG1::Torus mg1_torus : outsideScene.tori)
		{
			std::shared_ptr < Torus> object = std::make_shared<Torus>(mg1_torus, offsetId);
			uniqe_ptr_scene->AddFigure(object);
		}
		for (auto toLoad_object : outsideScene.surfacesC0) {
			std::shared_ptr<BezierSurfaceC0> object = std::make_shared < BezierSurfaceC0>(toLoad_object, uniqe_ptr_scene.get(), offsetId);
			uniqe_ptr_scene->AddFigure(object);
		}
		for (auto toLoad_object : outsideScene.surfacesC2) {
			std::shared_ptr<BezierSurfaceC2> object = std::make_shared < BezierSurfaceC2>(toLoad_object, uniqe_ptr_scene.get(), offsetId);
			uniqe_ptr_scene->AddFigure(object);
		}
		for (auto toLoad_object : outsideScene.bezierC0)
		{
			std::shared_ptr<BezierC0> object = std::make_shared < BezierC0>(toLoad_object, uniqe_ptr_scene.get(), offsetId);
			uniqe_ptr_scene->AddFigure(object);
		}
		for (auto toLoad_object : outsideScene.bezierC2)
		{
			std::shared_ptr<BezierC2> object = std::make_shared < BezierC2>(toLoad_object, uniqe_ptr_scene.get(), offsetId);
			uniqe_ptr_scene->AddFigure(object);
		}
		for (auto toLoad_object : outsideScene.interpolatedC2)
		{
			std::shared_ptr<BezierInterpolated> object = std::make_shared<BezierInterpolated>(toLoad_object, uniqe_ptr_scene.get(), offsetId);
			uniqe_ptr_scene->AddFigure(object);
		}

	}
	catch (...) {
		std::exception_ptr p = std::current_exception();
		//std::clog << (p ? p.__cxa_exception_type()->name() : "null") << std::endl;
		return uniqe_ptr_scene;
	}


	return uniqe_ptr_scene;
}

void SceneSerializer::SaveScene(const Scene& scene, const char* destination)
{
	int offset = scene.SmalestIdOnScene();
	MG1::Scene& exitScene = MG1::Scene::Get();
	MG1::SceneSerializer sceneSerializer{};
	exitScene.Clear();

	for (int i = 0; i < scene.Size(); i++) {
		auto figure = scene.at(i);
		if (figure->GetType() == FigureType::Point) {
			MG1::Point point = dynamic_cast<Point*>(figure.get())->Serialize(offset);
			exitScene.points.push_back(point);
		}
	}

	for (int i = 0; i < scene.Size(); i++) {
		auto figure = scene.at(i);
		switch (figure->GetType())
		{
		/*case FigureType::Any:
			break;
		case FigureType::UnchangeObject:
			break;
		case FigureType::Figure:
			break;*/
		case FigureType::Point:
		{
			
			break;
		}
		case FigureType::Torus:
		{
			MG1::Torus torus = dynamic_cast<Torus*>(figure.get())->Serialize(offset);
			exitScene.tori.push_back(torus);
			break;
		}

		/*case FigureType::Cursor:
			break;
		case FigureType::CenterPoint:
			break;*/
		case FigureType::BezierC0:
		{
			MG1::BezierC0 bezier = dynamic_cast<BezierC0*>(figure.get())->Serialize(offset);
			exitScene.bezierC0.push_back(bezier);
			break;
		}
		case FigureType::BezierC2:
		{
			MG1::BezierC2 bezier = dynamic_cast<BezierC2*>(figure.get())->Serialize(offset);
			exitScene.bezierC2.push_back(bezier);
			break;
		}
		case FigureType::BezierInterpolated:
		{
			MG1::InterpolatedC2 bezier = dynamic_cast<BezierInterpolated*>(figure.get())->Serialize(offset);
			exitScene.interpolatedC2.push_back(bezier);
			break;
		}
		case FigureType::BezierSurfaceC0:
		{
			MG1::BezierSurfaceC0 bezier = dynamic_cast<BezierSurfaceC0*>(figure.get())->Serialize(offset);
			exitScene.surfacesC0.push_back(bezier);
			break;
		}
		case FigureType::BezierSurfaceC2:
		{
			MG1::BezierSurfaceC2 bezier = dynamic_cast<BezierSurfaceC2*>(figure.get())->Serialize(offset);
			exitScene.surfacesC2.push_back(bezier);
			break;
		}
		/*case FigureType::VirtualPoint:
			break;
		case FigureType::COUNT:
			break;*/
		default:
			break;
		}
	}

	sceneSerializer.SaveScene(destination);
}


