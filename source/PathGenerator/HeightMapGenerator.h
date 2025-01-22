#pragma once
#include <SceneManadement/Scene.h>
#include <limits>
#include <omp.h>
#include <stdio.h>
#include "Figures/Intersection/Intersection.h"
#include "Figures/Intersection/IntersectionAblePuffed.h"
#include <Figures/Bezier/IntersectionCurve.h>
#include <Figures/Bezier/Surface/BezierSurfaceC0.h>
#include "glm/gtx/string_cast.hpp"
#include <ctime>    
#include <future>
#include <utility>
#include "Helper/FileManager.h"
#include "GeneratorOfPathInsideConstrain.h"

class HeightMapGenerator {
	Texture heightTexture{ GL_TEXTURE_2D };
	int resolution_x; int resolution_z;
	glm::vec3 start; glm::vec3 end;
	std::vector<float> heights;
	float height = 4.0f / 5.0f;
	float maxHeight = 1;
	float baseHeight = 20;
	float tolerance = 0.95;
	float holeTolerance = 0.5f;
	float error = 0.5f;
	float epsilon = 0.000001;
	const float r = 10.0f * 0.5f / 50.0f;
	const float r_08_frez_k = 8.0f * 0.5f / 50.0f;
	const float scale = 50;
	const glm::vec3 center{ 1.5, 0, 1.5 };
	const glm::vec3 bease{ 0, baseHeight, 0 };
	const glm::vec3 frezStartPos{ 0, 60, 0 };
	const std::string path = "D:/fast_acess/Studia/MINI CAD-CAM/Programowanie urz¹dŸeñ sterowanych numerycznie/Projekty/symulator/MillingCutterSimulator/Paths/Fish_01_";
	std::string outPathConstrains{ "D:/fast_acess/Studia/MINI CAD-CAM/Modelowanie Geometryczne I/projekt/MG1_projekt/resource/Data/Constrains/" };

	void ResizeHeights() {
		heights.resize(resolution_x * resolution_z);
		std::fill(heights.begin(), heights.end(), 0);
	}

public:
	HeightMapGenerator(int resolution_x, int resolution_z, glm::vec3 start, glm::vec3 end) {
		this->resolution_x = resolution_x;
		this->resolution_z = resolution_z;
		this->start = start;
		this->end = end;
	}

	void Interfers(Scene* scene) {
		if (ImGui::Button("Generate new texture"))
		{
			Generate(scene);
		}
		if (ImGui::Button("Load tex"))
		{
			LoadHeights();
		}

		ImGui::DragFloat("tolerance", &tolerance);
		ImGui::SameLine();
		if (ImGui::Button("path_1_rougher")) {
			LoadHeights();
			GeneratePaths(tolerance);
		}
		/*if (ImGui::Button("path_3_border (befor 2)"))
		{
			Path_2(scene);
		}*/
		if (ImGui::Button("path_2_base (after 3)"))
		{
			//Path_1dot5(scene, {});
			std::string outFile = path + std::string("path_2_base_poprawka.f10");
			SavePath(outFile.c_str(), Path_1dot5(scene, {}));
		}
		if (ImGui::Button("path_2_base_border (after 3)")) {
			std::vector<glm::vec3> result;
			result.push_back(frezStartPos);
			auto border = Path_2(scene);
			result.append_range(Path_1dot5(scene, border));
			

			std::for_each(border.begin(), border.end(), [=](glm::vec3& p) { p = TransformOrigin(p); });
			result.append_range(border);

			glm::vec3 last = result[result.size() - 1];
			last.y = frezStartPos.y;
			result.push_back(last);
			result.push_back(frezStartPos);

			std::string outFile = path + std::string("path_2_base_border.f10");
			SavePath(outFile.c_str(), result);
		}

		if (ImGui::Button("path_4_accurate_borders_generation_to_folder\n( befor path_4_accurate_from_folder"))
		{
			GenerateObjectConstrains(scene);
		}
		if (ImGui::Button("path_3_accurate_from_folder_and_hole"))
		{
			GeneratorOfPathInsideConstrain ap;
			ap.m_height = height;
			SceneSerializer sc;
			auto tmpScene = std::make_unique<Scene>();
			tmpScene = sc.LoadScene(
				"D:/fast_acess/Studia/MINI CAD-CAM/Modelowanie Geometryczne I/projekt/MG1_projekt/resource/Models/fish_19_double.json",
				move(tmpScene));
			auto positions = ap.GeneratePathFromConstrains(tmpScene.get(), outPathConstrains + "/testOfBetterSolution/", TakeAllFiguresAndIntersectionAble(tmpScene.get(), r_08_frez_k), r_08_frez_k);

			std::for_each(positions.begin(), positions.end(), [=](glm::vec3& p) {
				p = TransformOrigin(p);
				});

			auto tmpPositions = TheHole(scene, holeTolerance);
			tmpPositions.erase(tmpPositions.begin());
			positions.append_range(tmpPositions);
			SavePath((path + std::string("path_3_accurateAndHole.k08")).c_str(), positions);
			//GeneratePathFromConstrains(scene);
		}

		ImGui::DragFloat("hole tolerance", &holeTolerance);
		if (ImGui::Button("path 5 hole")) {
			TheHole(scene, holeTolerance);
		}

		ShowTexture();
	}

	void Generate(Scene* scene) 
	{
		std::vector<IntersectionAble*> intersectionAbles;
		intersectionAbles.reserve(scene->figureContainers.size() * 2);
		for (int i = 0; i < scene->Size(); i++) {
			IntersectionAble* casted = dynamic_cast<IntersectionAble*>(scene->at(i).get());
			if (casted) {
				intersectionAbles.push_back(casted);
			}
		}

		ResizeHeights();
		glm::vec3 lenght = end - start;
		Intersection intersection;

		for (auto obj : intersectionAbles) {
			float sample_u = obj->Field_u().y * 1000;
			float sample_v = obj->Field_v().y * 1000;
			for (int i = 0; i <= sample_v; i++) {
				if (i % 200 == 0)
					std::cout << i << std::endl;
				float v = (float)i * obj->Field_v().y / sample_v;
				for (int j = 0; j < sample_u; j++) {
					float u = (float)j * obj->Field_u().y / sample_u;
					auto result = obj->Parametrization(v, u);
					int x = result.x * resolution_x / lenght.x;
					int z = result.z * resolution_z / lenght.z;
					if (x >= 0 && z >= 0 && x <resolution_x && z < resolution_z && result.y > heights[x * resolution_z + z])
					{
						heights[x * resolution_z + z] = result.y;
					}
				}
			}
		}
		std::ofstream output_file;

		output_file.open("D:/fast_acess/Studia/MINI CAD-CAM/Modelowanie Geometryczne I/projekt/MG1_projekt/resource/Models/fish_finish_anotherTypeOfFish.tex");
		output_file << resolution_x << " " << resolution_z << "\n";
		for (int i = 0; i < resolution_x; i++) {
			for (int j = 0; j < resolution_z; j++) {
				output_file << heights[i * resolution_z + j] << " ";
			}
			output_file << "\n";
		}

		output_file.close();

		/*
		auto dataObjects = intersectionAbles.data();
		int numberOfObjects = intersectionAbles.size();
		auto dataHeights = heights.data();

		omp_set_num_threads(8);
		#pragma omp parallel for collapse(2)  
		for (int i = 0; i < resolution_x; i++) {
			for (int j = 0; j < resolution_z; j++) {
				glm::vec3 pos{ i * lenght.x / (float)resolution_x, 1, j * lenght.z / (float)resolution_z };
				pos += start;
				for (int id = 0; id < numberOfObjects; id++)
				{
					auto object = dataObjects[id];
					bool success = false;
					float h = abs(intersection.RayIntersection(object, pos, { 0, -1, 0 }, success).y);
					if (success && h > heights[i * resolution_z + j])
						dataHeights[i * resolution_z + j] = h;
				}
			}
			std::cout << omp_get_thread_num() <<" "<< omp_get_max_threads() << std::endl;
		}*/



		for(int i = 0; i < heights.size(); i++)
			heights[i ] /= maxHeight;

		RecreateHeightMap();
	}
	void RecreateHeightMap()
	{
		heightTexture.Recreat();
		heightTexture.Bind();
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, resolution_x, resolution_z, 0, GL_RED, GL_FLOAT, heights.data());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	void ShowTexture() {

		ImGui::Text("size = %d x %d", resolution_x, resolution_z);
		ImGui::Image((void*)(intptr_t)heightTexture.ID, ImVec2(resolution_x, resolution_z));
	}



	void LoadHeights() {
		std::ifstream input_file;
		input_file.open("D:/fast_acess/Studia/MINI CAD-CAM/Modelowanie Geometryczne I/projekt/MG1_projekt/resource/Models/fish_finish_anotherTypeOfFish.tex");

		std::string line;
		std::getline(input_file, line);
		std::istringstream iss(line);
		float resolutionX; iss >> resolutionX;
		float resolutionZ; iss >> resolutionZ;
		heights = std::vector<float>(resolutionX * resolutionZ);
		
		int x = 0;
		while (std::getline(input_file, line)) {
			std::istringstream iss_2(line);
			float number;

			int z = 0;
			while (iss_2 >> number) {
				heights[x * resolutionZ + z] = number;
				z++;
			}

			if (!iss_2.eof() && iss_2.fail()) {
				std::cerr << "Blad podczas odczytu liczb z linii: " << line << std::endl;
				continue;
			}
			x++;
		}
		input_file.close();
		RecreateHeightMap();
	}

	void GeneratePaths(float tolerance) {
		float r = 9.0f;
		float size_x = 150;
		float size_z = 150;
		float size_y = 50;

		glm::vec3 leftBottom{ -size_x * 0.5, 0, -size_z * 0.5 };
		glm::vec3 rightTop{ size_x * 0.5, 0, size_z * 0.5 };

		std::vector<glm::vec3> positions;
		positions.reserve(resolution_x * resolution_z);

		float nextPaths = r * tolerance * resolution_x / size_x;
		r *= resolution_x / size_x;
		positions.push_back(frezStartPos);
		positions.push_back(frezStartPos + leftBottom);
		float baseWithError = baseHeight + 0.001;
		auto position_1 = PathGenerator_1_SetBese(r, nextPaths, size_y, (baseWithError + frezStartPos.y) * 0.5f , leftBottom);
		auto position_2 = PathGenerator_1_SetBese(r, nextPaths, size_y, baseWithError, leftBottom);
		positions.append_range(position_1);
		std::reverse(position_2.begin(), position_2.end());
		positions.append_range(position_2);

		for (int i = 0; i < positions.size() - 1; i++) {
			if (positions[i] == positions[i + 1])
			{
				positions.erase(positions.begin() + i);
				i--;
			}
		}

		float eps = 0.0001;
		for (int i = 0; i < positions.size() - 2; i++) {
			auto p0 = positions[i];
			auto p1 = positions[i + 1];
			auto p2 = positions[i + 2];

			auto d0 = p1 - p0;
			auto d1 = p2 - p1;
			//glm::vec3 d_result = d0 / d1;
			auto result = glm::dot(glm::normalize(d0), glm::normalize(d1));
			if(result <= 1 + eps && result >= 1 - eps)
			{
				positions.erase(positions.begin() + i + 1);
				i--;
			}
			/*if (
				d_result.x <= 1 + eps && d_result.x >= 1 - eps &&
				d_result.y <= 1 + eps && d_result.y >= 1 - eps &&
				d_result.z <= 1 + eps && d_result.z >= 1 - eps)
			{
				positions.erase(positions.begin() + i + 1);
				i--;
			}*/
		}

		auto lastPos = positions[positions.size() - 1];
		lastPos.y = frezStartPos.y;
		positions.push_back(lastPos);
		positions.push_back(frezStartPos);


		SavePath((path + std::string("path_1_rougher.k16")).c_str(), positions);

	}

	std::vector<glm::vec3> PathGenerator_1_SetBese(float r, float nextPaths, float size_y, float minHeight, glm::vec3 offset) {
		std::vector<glm::vec3> positions{};
		positions.reserve(resolution_x * resolution_z);
		bool swap = false;
		bool last = true;
		for (float x = 0; x <= resolution_x || last; x += nextPaths)
		{
			if (x > resolution_x)
			{
				last = false;
				x = resolution_x;
			}
			if (!swap)
			{
				auto pos_last = MaxHeight(x, 0, r, size_y, minHeight);
				positions.push_back(pos_last + offset);
				for (float z = 0; z <= resolution_z; z++) {
					auto pos = MaxHeight(x, z, r, size_y, minHeight);
					positions.push_back(pos + offset);
				}
			}
			else {
				auto pos_last = MaxHeight(x, resolution_z, r, size_y, minHeight);
				positions.push_back(pos_last + offset);
				for (float z = resolution_z; z >= 0; z--) {
					auto pos = MaxHeight(x, z, r, size_y, minHeight);
					positions.push_back(pos + offset);
				}
			}
			swap = !swap;
		}

		return positions;
	}

	void SavePath(const char* path, const std::vector<glm::vec3>& poses) {

		std::ofstream output_file;
		output_file.open(path);
		char buf[100];
		for (auto p : poses)
		{
			sprintf(buf, "N3G01X%.4fY%.4fZ%.4f\n", p.x, p.z, p.y);
			output_file << buf;
		}
		output_file.close();
	}

	glm::vec3 MaxHeight(float x, float z, float r, float scalar_y, float minHeight) {
		glm::vec3 max{ x * 0.1, minHeight, z * 0.1};
		float r_invers = 1 / r;
		r = r + 10;
		for (int i = -r; i <= r; i++) {
			int id_x = (x + i) ;
			for (int j = -r; j <= r; j++) {
				int id_z = (z + j);
				if (id_x >= 0 && id_z >= 0 && id_x < resolution_x &&
					id_z < resolution_z )
				{
					float frez_h = (i * i + j * j) * 0.5 * r_invers;
					//float frez_h = 0;
					float h = baseHeight + heights[id_x * resolution_z + id_z] * scalar_y - frez_h * 0.1;
					if (max.y < h)
					{
						max.y = h;
					}
				}
			}
		}
		return max;
	}

	std::vector<glm::vec3> Path_2(Scene* scene) {
		auto bezierS = BezierSurfaceC0::CreateSurfaceShard_ptr("bezier", 1, 1, { 1.5f, 0, 1.5f });
		bezierS->ScaleAlong(bezierS->transpose->GetPosition(), { 1.25, 0.0, 1.25 });
		Intersection intersection{};
		std::vector<IntersectionAble*> intersectionAbles;
		intersectionAbles.reserve(scene->figureContainers.size() * 2);
		for (int i = 0; i < scene->Size(); i++) {
			IntersectionAble* casted = dynamic_cast<IntersectionAble*>(scene->at(i).get());
			if (casted && scene->at(i)->GetType() != FigureType::BezierSurfaceC0) {
				intersectionAbles.push_back(casted);
			}
		}
		if (intersectionAbles.size() < 8)
			return {};
		// magiczne liczby specjalnie dobrane do modelu rybki
		std::vector<glm::vec4> parametrs{
			{0.463905, 2.55678, 0.616398, 0.379837},	//left_fin_1
			{4.095642, 0.543322, 0.412113, 0.109434},		//tail_1
			{1.59584, 4.6981, 0.761361, 0.53057},		//right_fin_1
			{8.06015, 13.1123, 0.652453, 0.360059},		//body_1

			{0.642608, 3.42447, 0.383945, 0.542146},	//left_fin_1
			{1.336567, 4.631216, 0.800564, 0.699986 },		//tail_1
			{0.457183, 3.81921, 0.267036, 0.571402},		//right_fin_1
			{ 13.8911, 3.34967, 0.251967, 0.23339 },	//body_1	
		};
		std::vector<std::shared_ptr<IntersectionCurve>> interCurves;
		interCurves.reserve(intersectionAbles.size());
		glm::vec3 point{ 3, 0, 1.5 };
		int k = 0;
		for (auto interObj : intersectionAbles) {
			glm::vec4 params{ 0, 0, 0, 0 };
			/*for (int i = 0; i < 2 && !intersection.IsIntersected(interObj, bezierS.get(), params); i++) {
				params = intersection.RandomTheClosetToEachOther(interObj, bezierS.get());
				params = intersection.FirstIntersectionPoint(interObj, bezierS.get());
			}*/
			/*
			if (k == 4)
			{
				point.x = 0;
				point.z = 1;
			}
			glm::vec2 tmpParams = intersection.ParamsCloseToPointSample(point, interObj);
			glm::vec3 pos = interObj->Parametrization(tmpParams.x, tmpParams.y);
			glm::vec2 b_params{ pos.z / 3.0f, pos.x / 3.0f};
			glm::vec3 cosTam = bezierS->Parametrization(0, 0, b_params.x, b_params.y);
			params = { tmpParams, b_params };*/
			
			params = parametrs[k];
			//if(k == 1 || k == 5)
				//params = intersection.RandomTheClosetToEachOther(interObj, bezierS.get());

			params = intersection.FirstIntersectionPoint(interObj, bezierS.get(), params);

			if (!intersection.IsIntersected(interObj, bezierS.get(), params))
				continue;
			
			std::cout << params.x << " " << params.y << " " << params.z << " " << params.w << std::endl;
			std::cout << interObj->Parametrization(params.x, params.y).x << " " << interObj->Parametrization(params.x, params.y).y << " " << interObj->Parametrization(params.x, params.y).z << std::endl;
			float step = 0.01;
			
			auto positions = intersection.IntersectionFrame(params, interObj, bezierS.get(), step, epsilon);
			if (k == 1 || k == 5)
				RemoveSimilarAtBeginAndEnd(positions.second, 0.0001);

			interCurves.push_back(std::make_shared< IntersectionCurve>(positions.second, bezierS, scene));

			if ( !(k == 3 || k == 7)) // magiczna zimenna upewniaj¹ca siê, ¿e pochodne s¹ w dobrym kierunku
				interCurves[k]->Reverse();

			k++;
		}
		/*params = intersection.IntersectionBezier_2D_XZ(params, interCurves[0].get(), {0, 3}, {1, 0}, 6.0f / 5.0f);*/

		//std::cout << params.x << " " << params.y;
		

		/*for (int i = 0; i < interCurves.size(); i++) {
			scene->AddFigure(interCurves[i]);
		}
		scene->AddFigure(bezierS);
		return;*/

		/*auto future_taile_1_taile_1 = std::async(std::launch::async, &Intersection::PosibleIntersections_2, &intersection,
			interCurves[1].get(), interCurves[1].get(), r);
		auto future_taile_2_taile_2 = std::async(std::launch::async, &Intersection::PosibleIntersections_2, &intersection,
			interCurves[5].get(), interCurves[5].get(), r);*/

		// Pobieranie wyników
		auto body_1_rightFin_1 = intersection.PosibleIntersections_2(interCurves[3].get(), interCurves[0].get(), r);
		auto body_1_taile_1 = intersection.PosibleIntersections_2(interCurves[3].get(), interCurves[1].get(), r);
		auto body_1_leftFin_1 = intersection.PosibleIntersections_2(interCurves[3].get(), interCurves[2].get(), r);
		auto body_1_taile_2 = intersection.PosibleIntersections_2(interCurves[3].get(), interCurves[5].get(), r);
		
		auto body_2_rightFin_2 = intersection.PosibleIntersections_2(interCurves[7].get(), interCurves[4].get(), r);
		auto body_2_taile_2 = intersection.PosibleIntersections_2(interCurves[7].get(), interCurves[5].get(), r);
		auto body_2_leftFin_2 = intersection.PosibleIntersections_2(interCurves[7].get(), interCurves[6].get(), r);
				  
		auto body_2_taile_1 = intersection.PosibleIntersections_2(interCurves[7].get(), interCurves[1].get(), r);


		std::vector<std::vector<cureIntersectionInfo>> intersections{
			body_1_taile_1, body_1_leftFin_1, body_1_rightFin_1, body_1_taile_2,
			body_2_taile_2, body_2_leftFin_2, body_2_rightFin_2, body_2_taile_1
		};
		std::vector<cureIntersectionInfo> allPoints;
		for (auto vector : intersections)
			allPoints.insert(allPoints.end(), vector.begin(), vector.end());

		for (auto& pointData : allPoints) {
			std::shared_ptr <Point> point = std::make_shared<Point>();
			point->transpose->SetObjectPosition(intersection.MoveAcrossNormal(pointData.t1_1, pointData.bezier_1, r));
			scene->AddFigure(point);

			point = std::make_shared<Point>();
			point->transpose->SetObjectPosition(intersection.MoveAcrossNormal(pointData.t1_2, pointData.bezier_1, r));
			scene->AddFigure(point);
		}

		std::shared_ptr<BezierInterpolated> bezierInterpolaited = std::make_shared<BezierInterpolated>();
		BezierCurve* startCure = interCurves[3].get();

		float start_t = intersection.TheClosetTo(interCurves[3].get(), { 0, 0, 1.5f });
		auto tmp = intersection.MoveAcrossNormal(start_t, startCure, r);
		
		//glm::vec2 params{ t, interCurves[3]->PositionOnCurve(t).x };
		//float start_t = interCurves[3]->MaxValue() * 0.75;
		BezierCurve* currentCure = startCure;
		float current_t = start_t;
		int switchCount = -1;
		float dis = 0.49;
		float nextParam = 0.5f;
		std::cout << std::endl;
		std::vector<glm::vec3> results;
		results.reserve(1024);
		//results.push_back(frezStartPos);
		/*results.push_back(frezStartPos);
		results.push_back(frezStartPos);*/
		do {
			std::shared_ptr <Point> point = std::make_shared<Point>();
			if (current_t > currentCure->MaxValue())
				current_t = 0;
			else if (current_t < 0)
				current_t = currentCure->MaxValue();
			auto position = intersection.MoveAcrossNormal(current_t, currentCure, r);
			results.push_back(position);
			point->transpose->SetObjectPosition(intersection.MoveAcrossNormal(current_t, currentCure, r));
			scene->AddFigure(point);
			bezierInterpolaited->Add(point.get());

			for (auto info : allPoints) {
				if (info.ShouldMove(currentCure, current_t, nextParam - 0.001))
				{
					switchCount++;
					auto next = info.MoveToNextCurve(currentCure, current_t);
					std::cout << current_t << std::endl;

					currentCure = next.first;
					current_t = next.second;
					std::cout << switchCount << " " << glm::to_string(info.pos) << " " << current_t << std::endl;
					auto der = currentCure->Derivative(current_t);
					auto derLength = glm::dot(der, der);
					nextParam = 0.5f;
					break;
				}
			}
			current_t += nextParam;

		} while (fabsf(current_t - start_t) > nextParam*0.5 || startCure != currentCure);
		
		
		scene->AddFigure(bezierInterpolaited);

		/*results[0].x = -75 - 5.0f - 0.1f;
		results[0].z = results[2].z;

		results[1] = results[0];
		results[1].y = baseHeight;

		results.push_back(results[2]);
		auto last = results[results.size() - 1];
		last.y = frezStartPos.y;
		results.push_back(last);*/

		return results;

		//results.push_back(frezStartPos);

		/*std::string outFile = path + std::string("path_3_border.f10");
		SavePath(outFile.c_str(), results);*/



		/*for (int i = 0; i < interCurves.size(); i++) {
			scene->AddFigure(interCurves[i]);
		}*/
		//scene->AddFigure(bezierS);
	}

	void RemoveSimilarAtBeginAndEnd(std::vector<glm::vec2>& data, float dis) {
		glm::vec2 start = data[0];
		while (data.size() > 0) {
			glm::vec2 diff = start - data[0];
			if (glm::dot(diff, diff) <= dis)
				data.erase(data.begin());
			else
				break;
		}

		glm::vec2 end = data[data.size() - 1];
		while (data.size() > 0) {
			glm::vec2 diff = end - data[data.size() - 1];
			if (glm::dot(diff, diff) <= dis)
				data.pop_back();
			else
				break;
		}
	}

	std::vector<glm::vec3> Path_1dot5(Scene* scene, std::vector<glm::vec3> vertexes) {
		//vertexes = LoadPath((path + std::string("path_3_border.f10")).c_str());
		//auto vertexes = Path_2(scene);
		float distanceBeetwenPaths = 2 * r - r / 3.0f;
		//vertexes.erase(vertexes.begin(), vertexes.begin() + 2);
		glm::vec3 UpLeft = { 3,0,3 };

		std::vector<glm::vec3> points;
		glm::vec3 direction_z{ 0, 0, 1 };
		glm::vec3 direction_x{ 1, 0, 0 };

		const glm::vec3 up{ 0, 0, 1 };
		const glm::vec3 down{ 0, 0, -1 };
		const glm::vec3 right{ 1, 0, 0 };
		const glm::vec3 left{ -1, 0, 0 };

		glm::vec3 directionVertical = up;
		glm::vec3 directionHorizontal = right;

		const float halfDistanceBeetwenPaths = distanceBeetwenPaths * 0.5f;
		//points.push_back(frezStartPos);
		points.push_back({ -75 + halfDistanceBeetwenPaths, frezStartPos.y, -75 - distanceBeetwenPaths * 2});

		glm::vec3 startPos = { halfDistanceBeetwenPaths, 0, -distanceBeetwenPaths * 2 };
		glm::vec3 pos{ halfDistanceBeetwenPaths, 0, -distanceBeetwenPaths * 2 };
		points.push_back(TransformOrigin(pos));
		glm::vec3 min{ -halfDistanceBeetwenPaths * 0.5f, 0, 0 },
			max{ 3 - halfDistanceBeetwenPaths * 0.5f, 5, 3 };
		for(int i =0; i < 45; i++) {
			
			auto block = LegalPosition(vertexes, pos, directionVertical, 0); // obliczanie pozycji przeszkody
			
			float additionalScalar = 1;
			//if (i == 5 || i == 6 || i == 7 || i == 14 || i == 19 )
			//	additionalScalar = 0.5f;
			//else if( i == 17 || i == 12 || i == 15)
			//	additionalScalar = 0.2f;
			//else if (i == 4 || i == 13 || i == 20 ) {
			//	//directionVertical = SwitchDirection(directionVertical, up, down);
			//	additionalScalar = 0.0f;
			//}
			if (i == 4 || i == 6  || i == 32 || i == 19) {
				additionalScalar = 0.5f;
			}

			if (i == 21) {
				additionalScalar = 0.1f;
			}

			if(i == 14 || i == 16 || i == 18) {
				additionalScalar = 0.7f;
			}
			
			if (i == 38) {
				additionalScalar = 0.7f;
			}

			// nie napotkano przeszkody
			if (isnan(block.first) ) { 
				// obcinanie do krawêdzi
				ClampPos(pos + directionVertical * 10.0f, min, max, pos); 
				points.push_back(TransformOrigin(pos));
				pos += directionHorizontal * distanceBeetwenPaths * additionalScalar;

				if (pos.x > max.x)
				{
					directionHorizontal = SwitchDirection(directionHorizontal, left, right);
					ClampPos(pos, min, max, pos);

					// robimy zwrot, specyficzne dla tego modelu
					pos.x += distanceBeetwenPaths;
					points.push_back(TransformOrigin(pos));
					float tmp = pos.x;
					pos = UpLeft + startPos;
					pos.x = tmp;
					points.push_back(TransformOrigin(pos));
					pos.x = 3 - r;
					points.push_back(TransformOrigin(pos));


				}
				else if (pos.x < min.x)
					break;
				points.push_back(TransformOrigin(pos));
				directionVertical = SwitchDirection(directionVertical, up, down);
			}
			else {
				glm::vec3 border = block.second - directionVertical * halfDistanceBeetwenPaths;
				points.push_back(TransformOrigin(border));

				auto posiblePos = LegalPosition(vertexes, border, directionHorizontal, 0);
				if (isnan(posiblePos.first) ||
					glm::dot(border - posiblePos.second, border - posiblePos.second) > distanceBeetwenPaths * distanceBeetwenPaths) {

					pos = pos - directionVertical * 10.0f + directionHorizontal * distanceBeetwenPaths * additionalScalar;
					pos = LegalPosition(vertexes, pos, directionVertical, 0).second - directionVertical * halfDistanceBeetwenPaths * 0.15f;
					if (isnan(pos.x))
						break;
				}
				else
				{
					pos = pos - directionVertical * 10.0f + directionHorizontal * distanceBeetwenPaths * additionalScalar;
					pos = LegalPosition(vertexes, pos, directionVertical, 0).second - directionVertical * halfDistanceBeetwenPaths * 0.15f;
					if (isnan(pos.x))
						break;
				}
				points.push_back(TransformOrigin(pos));
				directionVertical = SwitchDirection(directionVertical, up, down);
			}
			
		}
		/*auto last = points[points.size() - 1];
		last.y = frezStartPos.y;
		points.push_back(last);*/
		//points.push_back(frezStartPos);

		return points;
	/*	std::string outFile = path + std::string("path_2_base.f10");
		SavePath(outFile.c_str(), points);*/
	}



	glm::vec3 SwitchDirection(glm::vec3 d_current, glm::vec3 d1, glm::vec3 d2) {
		if (d_current == d1)
			return d2;
		else
			return d1;
	}

	bool ClampPos(glm::vec3 pos, const glm::vec3& min, const glm::vec3& max, glm::vec3& result) {
		result = pos;
		if (pos.x > max.x) pos.x = max.x;
		if (pos.y > max.y) pos.y = max.y;
		if (pos.z > max.z) pos.z = max.z;
		if (pos.x < min.x) pos.x = min.x;
		if (pos.y < min.y) pos.y = min.y;
		if (pos.z < min.z) pos.z = min.z;
		bool clamped = result == pos;
		result = pos;
		return clamped;
	}

	std::pair<float, glm::vec3> LegalPosition(const std::vector<glm::vec3> points, glm::vec3 start, glm::vec3 direction, float dis) {
		const float epsilon = 0.0001;
		float s = M_FLOAT_MAX;
		float a = glm::dot(direction, direction);
		float aInvers = 1.0f / a;
		float disPow = dis * dis;
		float smalestDistance = M_FLOAT_MAX;
		for (auto point : points) {
			glm::vec3 startMPoint = start - point;
			float startMPointsPow = glm::dot(startMPoint, startMPoint);
			float b = 2 * glm::dot(startMPoint, direction);
			float c = startMPointsPow - disPow;
			float delta = b * b - 4 * a * c;

			float s_tmp;
			float tmpDis;
			if (delta < 0){
				s_tmp = -b / (2 * a);
				s_tmp = s_tmp < 0 ? 0 : s_tmp;
				auto dif = start + direction * s_tmp - point;
				tmpDis = glm::dot(dif, dif) - disPow;

				if (tmpDis < epsilon)
					tmpDis = 0;
			}
			else {
				float s_1 = (-b - sqrt(delta)) / (2 * a);
				s_1 = s_1 < 0 ? M_FLOAT_MAX : s_1;
				float s_2 = (-b + sqrt(delta)) / (2 * a);
				s_2 = s_2 < 0 ? M_FLOAT_MAX : s_2;

				s_tmp = min(s_1, s_2);
				tmpDis = 0;
			}

			if (tmpDis < smalestDistance || 
				(tmpDis == smalestDistance && s_tmp < s)) {
				s = s_tmp;
				smalestDistance = tmpDis;
			}
		}

		if (s >= M_FLOAT_MAX || smalestDistance > 0)
			return { NAN, start * NAN};

		return { s, start + direction * s };
	}



	std::vector<glm::vec3> LoadPath(const char* pathToFile)
	{
		std::vector<glm::vec3> path;
		std::string  tmpPath{ pathToFile };
		int exeStartPos = tmpPath.find_last_of('.') + 1;

		std::string line;
		std::ifstream myfile(pathToFile);
		int numberOfLines = std::count(std::istreambuf_iterator<char>(myfile),
			std::istreambuf_iterator<char>(), '\n');
		path.clear();
		path.reserve(numberOfLines);
		myfile.seekg(0);
		float scaleInvers = 1.0f / scale;
		if (myfile.is_open())
		{
			while (std::getline(myfile, line))
			{
				//std::cout << line << '\n';
				int x = line.find('X') + 1;
				int y = line.find('Y') + 1;
				int z = line.find('Z') + 1;
				glm::vec3 pos{ std::stof(&line[x]), std::stof(&line[z]), std::stof(&line[y]) };
				pos = (pos - bease) * scaleInvers + center;

				path.push_back(pos);
			}
			myfile.close();
		}

		else std::cout << "Unable to open file";
		return path;
	}

	std::vector<std::pair<std::shared_ptr<Figure>, std::pair<IntersectionAble*, IntersectionAblePuffed>>> TakeAllFiguresAndIntersectionAble(Scene* scene, float r) {
		std::vector<std::pair<std::shared_ptr<Figure>, std::pair<IntersectionAble*, IntersectionAblePuffed>>> figures;
		figures.reserve(scene->figureContainers.size() * 2);
		int ii = 0;
		for (int i = 0; i < scene->Size(); i++) {
			auto fig = scene->at(i).get();
			IntersectionAble* casted = dynamic_cast<IntersectionAble*>(fig);
			float signe = 1.0f;
			if (ii == 5 || ii == 9)
				signe = -1.0f;
			if (casted)
			{
				figures.push_back({ scene->at(i), {casted, IntersectionAblePuffed(casted, signe * r) } });
				ii++;
			}
		}
		return figures;
	}

	void GenerateObjectConstrains(Scene* scene) {

#pragma region Initialize
		std::vector<std::pair<std::shared_ptr<Figure>, std::pair<IntersectionAble*, IntersectionAblePuffed>>> figures = TakeAllFiguresAndIntersectionAble(scene, r_08_frez_k);
		if (figures.size() < 10)
			return;
		Intersection intersection{};

	
		std::map<int, std::vector<std::vector<glm::vec2>>> objectParams;
		for (int i = 0; i < figures.size(); i++)
			objectParams.insert({ i, {} });
	#pragma endregion
		// intersection every surface with base
#pragma region IntersectionsWithBase 

		auto bezierS = BezierSurfaceC0::CreateSurfaceShard_ptr("bezier", 1, 1, { 1.5f, 0, 1.5f });

		bezierS->ScaleAlong(bezierS->transpose->GetPosition(), { 1.125, 0.0, 1.125 });
		std::vector<glm::vec4> startParams_baseIntersection{
			{}, {},  // ignor for crowns
			{0.689233, 0.611476, 3.578446, 2.721919},
			{0.437978, 0.124236, 4.462134, 1.051075}, // tail_1
			{0.774002, 0.311689, 0.600445, 1.810303}, 
			{0.688584, 0.747081, 14.054398, 2.153436},


			{0.392950, 0.469432, 1.182027, 4.059142},
			{0.731423, 0.719965, 2.067727, 4.518651}, // tail_2
			{0.275151, 0.460658, 1.975291, 4.339540}, 
			{0.140157, 0.408783, 11.768969, 4.406428},
		};
		auto bezierPuffed = IntersectionAblePuffed(bezierS.get(), r_08_frez_k);
		std::vector<std::shared_ptr<IntersectionCurve>>interCurves;

		for (int i = 2; i < startParams_baseIntersection.size(); i++) {
			auto fig = figures[i];
			glm::vec4 params;
			params = startParams_baseIntersection[i];
			/*if(i == 5)
				params = intersection.RandomTheClosetToEachOther(&bezierPuffed, &fig.second.second);*/

			


			bool suces = false;
			auto positions = TakePositions(params, &bezierPuffed, &fig.second.second, suces);
			if (!suces)
				continue;

			/*positions.second.erase(std::remove_if(positions.second.begin(), positions.second.end(),
				[n = 0](int) mutable { return ++n % 2 == 0; }),
				positions.second.end());*/
			Reduce(positions.second);
			objectParams[i].push_back(positions.second);
			Texture cos{ GL_TEXTURE_2D };
			interCurves.push_back(std::make_shared< IntersectionCurve>(positions.second, figures[i].first, &figures[i].second.second, scene));
		}
#pragma endregion

#pragma region FishBodyWithBodyParts
	 //cia³o ryby 1 czêœci ryby pierwszej
		std::vector<glm::vec4> startParams{
			{11.387382, 14.631113, 1.517934, 3.329505}, // body leftFin 
			{3.360984, 14.406018, 4.322404, 3.783720},	// body taile 
			{9.316753, 4.148162, 1.224576, 1.578465},	// body right fin

			//{10.754529, 0.925698, 0.707498, 1.003828},	// body - back - up
			{10.761684, 0.916646, 0.710975, 1.006114},		// body - back close to center
			{11.462418, 2.163230, 0.912601, 0.743262},		// body - back far from center
			{11.901263, 2.242390, 1.160305, 0.613488},	// body - back - right
			{12.333655, 0.089156, 1.503938, 1.389227},	// body - back - left

			{14.449626, 1.838129, 3.357511, 4.385929},	// body 2 - tail 1
			{14.449626, 1.838129, 3.357511, 4.385929},	// body 1 - tail 2
		};
		//std::vector<std::shared_ptr<IntersectionCurve>>interCurves;
		interCurves.reserve(30);
		// cia³o ryby a reszta
		int body_1_id = 5;
		int body_2_id = 9;
		auto body_1 = figures[body_1_id];
		auto body_2 = figures[body_2_id];
		for (int i = 0; i < 7; i++) {
			int id = i + 2;
			if (i >= 3)
				id = 0;

			auto fig = figures[id];
			glm::vec4 params;
			params = startParams[i];

			bool suces = false;
			auto positions = TakePositions(params, &body_1.second.second, &fig.second.second, suces);
			if (!suces)
				continue;

			Reduce(positions.first);
			Reduce(positions.second);

			if(id == 0 && i <= 4) // specjalny case dla up back
			{
				auto ids = TheLargerDistanceBetweenIds(positions.second, &fig.second.second);
				if (ids.first < positions.second.size() - ids.first) {
					positions.first.erase(	positions.first.begin(),	std::next(positions.first.begin(),	ids.second));
					positions.second.erase(	positions.second.begin(),	std::next(positions.second.begin(), ids.second));
				}
				else
				{
					positions.first.erase(	std::next(positions.first.begin(),	ids.second), positions.first.end());
					positions.second.erase(	std::next(positions.second.begin(),	ids.second), positions.second.end());
				}
			}

			objectParams[body_1_id].push_back(positions.first);
			objectParams[id].push_back(positions.second);

			/*interCurves.push_back(std::make_shared< IntersectionCurve>(positions.first, figures[body_1_id].first, &figures[body_1_id].second.second, scene));
			interCurves.push_back(std::make_shared< IntersectionCurve>(positions.second, figures[id].first, &figures[id].second.second, scene));
			*/
			
			// pdbijamy dla rybki numer dwa bo ma takie same parametrybo jest tylko obrócona
			int secondId = id + 4;
			if (id == 0) // jesli mamy korone
				secondId = 1;

			objectParams[body_2_id].push_back(positions.first);
			objectParams[secondId].push_back(positions.second);

			//interCurves.push_back(std::make_shared< IntersectionCurve>(positions.first, figures[body_2_id].first, &figures[body_2_id].second.second, scene));
			//interCurves.push_back(std::make_shared< IntersectionCurve>(positions.second, figures[secondId].first, &figures[secondId].second.second, scene));
		}
#pragma endregion
#pragma region FishBodyWithAnotherFishTail
		bool suces = false;

		// fish_1 tail 2 ////////////////
		int tail_2_id = 7;
		auto tail_2 = figures[tail_2_id];
		auto positions = TakePositions(startParams[6], &body_1.second.second, &tail_2.second.second, suces);


		Reduce(positions.first);
		Reduce(positions.second);

		objectParams[body_1_id].push_back(positions.first);
		objectParams[tail_2_id].push_back(positions.second);


		// fish_2 tail 1 ////////////////
		int tail_1_id = 3;
		auto tail_1 = figures[tail_1_id];
		positions = TakePositions(startParams[7], &body_2.second.second, &tail_1.second.second, suces);


		Reduce(positions.first);
		Reduce(positions.second);

		objectParams[body_2_id].push_back(positions.first);
		objectParams[tail_1_id].push_back(positions.second);
#pragma endregion
		/*for (int i = 0; i < figures.size(); i++) {
			for (int j = 0; j < objectParams[j].size(); j++) {
				if (j > 0)
					continue;
				Texture cos{ GL_TEXTURE_2D };
				interCurves.push_back(std::make_shared< IntersectionCurve>(objectParams[i][j], figures[i].first, &figures[i].second.second, scene));
			}
		}*/
	#pragma region ConectIntersectionsVector
		std::vector<glm::vec2> fish_1_crown;
		std::vector<glm::vec2> fish_2_crown;
		{

			int k = 0;
			for (auto& pair : objectParams) {
				int kk = 0;
				std::vector<glm::vec2> result;
				result.reserve(pair.second.size() * pair.second[0].size());
				result.append_range(pair.second[0]);
				pair.second.erase(pair.second.begin());
				int numberOfintersections = pair.second.size();
				int intersectionID = -1;
				for (auto& vectorParam : pair.second)
				{
					intersectionID++;
					// dodajemy wynik wczeœniej jak mamy cia³o ryby i korone 

					if (k == 5){
						if (intersectionID == 3)
						{
							fish_1_crown = vectorParam;
							continue;
						}
					}
					else if (k == 9)
					{
						if (intersectionID == 3)
						{
							fish_2_crown = vectorParam;
							continue;
						}
					}
					
					if (!(k == 0 || k == 1) &&
						!((k == 5 || k == 9) && intersectionID > 3 && intersectionID < 7) )
					{
						auto intersectionsPoints = intersection.PosibleIntersections(result, vectorParam);
						if (k == 5 || k == 9)
							for (auto ip : intersectionsPoints) {
								auto addPoint = [=](glm::vec3 pos) {std::shared_ptr <Point> point2 = std::make_shared<Point>();
								point2->transpose->SetObjectPosition(pos);
								scene->AddFigure(point2);
									};
								auto insertAble = &figures[pair.first].second.second;
								auto tmpPar = result[ip.m_id_1.first];
								addPoint(insertAble->Parametrization(tmpPar.x, tmpPar.y));

								tmpPar = result[ip.m_id_1.second];
								addPoint(insertAble->Parametrization(tmpPar.x, tmpPar.y));

								tmpPar = ip.m_vector_2->at(ip.m_id_2.first);
								addPoint(insertAble->Parametrization(tmpPar.x, tmpPar.y));

								tmpPar = ip.m_vector_2->at(ip.m_id_2.second);
								addPoint(insertAble->Parametrization(tmpPar.x, tmpPar.y));
							}
						//if(k == 9 )
						if (k == 5) {
							if (intersectionID == 1) { // czemuœ wykrywa mi siê za du¿o punktów
								intersectionsPoints.pop_back();
								intersectionsPoints.erase(intersectionsPoints.begin());
							}
							else if (intersectionID == 7)
							{
								intersectionsPoints.erase(intersectionsPoints.begin());
							}
						}
						if (k == 9) {
							if (intersectionID == 1) { // czemuœ wykrywa mi siê za du¿o punktów
								intersectionsPoints.pop_back();
								intersectionsPoints.erase(intersectionsPoints.begin());
							}
							else if (intersectionID == 7)
							{
								intersectionsPoints.pop_back();
							}
						}
						

						if (intersectionsPoints.size() < 2)
							continue;

						// zak³adam, ¿e result jest zawsze pêtl¹
						vectorIntersectionInfo intersection_1{ intersectionsPoints[0] };
						vectorIntersectionInfo intersection_2{ intersectionsPoints[1] };

						int length = intersection_2.m_id_1.first - intersection_1.m_id_1.first;
						bool selector = length > result.size() / 2;
						if ( k == 2 || k == 6 || k == 8)
							selector = !selector;
						if (selector)
						{// boki nie s¹ potrzebne
							result.erase(result.begin() + intersection_2.m_id_1.second, result.end());
							result.erase(result.begin(), result.begin() + intersection_1.m_id_1.second);
						}
						else {// œrodek nie jest potrzebny
							std::vector<glm::vec2> tmpResult;
							int sizeOfResult = result.size();
							result.insert(result.end(), result.begin(), result.begin() + intersection_1.m_id_1.second);
							result.erase(result.begin(), result.begin() + intersection_2.m_id_1.second);
							std::reverse(result.begin(), result.end());
						}

						// usuwamy zbêdne dane w wektorze
						if (intersection_1.m_id_2.first < intersection_2.m_id_2.first) {
							vectorParam.erase(vectorParam.begin() + intersection_2.m_id_2.second, vectorParam.end());
							vectorParam.erase(vectorParam.begin(), vectorParam.begin() + intersection_1.m_id_2.second);

							std::reverse(vectorParam.begin(), vectorParam.end());
						}
						else
						{
							vectorParam.erase(vectorParam.begin() + intersection_1.m_id_2.second, vectorParam.end());
							vectorParam.erase(vectorParam.begin(), vectorParam.begin() + intersection_2.m_id_2.second);

						}
						// dodajemy rezultat
						result.append_range(vectorParam);
					}
					else { // ³¹czymy koncówki gdy jest korona
						if(k != 5 && k != 9)
							ConnectTheClosetsEnds(result, vectorParam);
						else {
							if(k == 5)
								ConnectTheClosetsEnds(fish_1_crown, vectorParam);
							else if(k == 9)
								ConnectTheClosetsEnds(fish_2_crown, vectorParam);

						}
					}
				}
				k++;
				pair.second.erase(pair.second.begin(), pair.second.end());
				pair.second.push_back(result);
			}
		}
	#pragma endregion
#pragma CreateIntersactionCurves
		{
			std::string pathToDirectory = outPathConstrains + "/testOfBetterSolution/";
			if (!(CreateDirectory(pathToDirectory.c_str(), NULL) ||
				ERROR_ALREADY_EXISTS == GetLastError()))
			{
				throw "Failed to create directory.";
			}
			

			fish_1_crown.push_back(fish_1_crown[0]);
			fish_2_crown.push_back(fish_2_crown[0]);
			for (int i = 0; i < figures.size(); i++) {
				objectParams[i][0].push_back(objectParams[i][0][0]);
				if (i == 0)
					Texture tmp{ GL_TEXTURE_2D };
				IntersectionAble* inter = figures[i].second.first;
				glm::vec2 field_u = inter->Field_u();
				glm::vec2 field_v = inter->Field_v();
				std::string path = pathToDirectory + std::to_string(i);
				std::string constrains = std::format(".{}_{}_{}_{}", field_u.x, field_u.y, field_v.x, field_v.y);

				interCurves.push_back(std::make_shared< IntersectionCurve>(objectParams[i][0], figures[i].first, &figures[i].second.second, scene));
				
				FileManager::saveVec2VectorToFile(objectParams[i][0], path + "_" + constrains);
				if (i == 5) {
					interCurves.push_back(std::make_shared< IntersectionCurve>(fish_1_crown, figures[i].first, &figures[i].second.second, scene));
					FileManager::saveVec2VectorToFile(fish_1_crown, path + "_inner" + constrains);
				}
				else if (i == 9) {
					interCurves.push_back(std::make_shared< IntersectionCurve>(fish_2_crown, figures[i].first, &figures[i].second.second, scene));
					FileManager::saveVec2VectorToFile(fish_2_crown, path + "_inner" + constrains);
				}
			}

		}
#pragma endregion

	#pragma region ShowResults
		scene->AddFigure(bezierS);
		// add add the end to visualated
		for (int i = 0; i < interCurves.size(); i++) {
			scene->AddFigure(interCurves[i]);
		}
	#pragma endregion
	}

	std::pair<int, int> TheLargerDistanceBetweenIds(std::vector<glm::vec2>& params, IntersectionAble* inter) {
		float dis = -1;
		std::pair<int, int> ids{ -1, -1 };
		for (int i = 0; i < params.size() - 1; i++) {
			auto param_1 = params[i];
			auto param_2 = params[i + 1];
			auto pos_1 = inter->Parametrization(param_1.x, param_1.y);
			auto pos_2 = inter->Parametrization(param_2.x, param_2.y);
			glm::vec3 diff = pos_1 - pos_2;
			float tmpDis = glm::dot(diff, diff);
			if (tmpDis > dis) {
				dis = tmpDis;
				ids.first = i;
				ids.second = i + 1;
			}
		}
		return ids;
	}

	std::vector<glm::vec3> TheHole(Scene* scene, float holeTolerance) {
		//inicjalizacja
		std::vector<std::pair<std::shared_ptr<Figure>, std::pair<IntersectionAble*, IntersectionAblePuffed>>> figures = TakeAllFiguresAndIntersectionAble(scene, r_08_frez_k);
		if (figures.size() < 10)
			return {};
		std::vector<std::vector<glm::vec2>> vectorOfParamsVectors;
		Intersection intersection{};
		figures.erase(figures.begin(), figures.begin() + 2);

		// przeciêcia z bezierem

		auto bezierS = BezierSurfaceC0::CreateSurfaceShard_ptr("bezier", 1, 1, { 1.5f, 0, 1.5f });

		bezierS->ScaleAlong(bezierS->transpose->GetPosition(), { 1.125, 0.0, 1.125 });
		std::vector<glm::vec4> startParams_baseIntersection{
			
			{0.689233, 0.611476, 3.578446, 2.721919},
			{0.437978, 0.124236, 4.462134, 1.051075}, // tail_1
			{0.774002, 0.311689, 0.600445, 1.810303},
			{0.500074, 0.255490, 4.688413, 13.901965},


			{0.392950, 0.469432, 1.182027, 4.059142},
			{0.731423, 0.719965, 2.067727, 4.518651}, // tail_2
			{0.275151, 0.460658, 1.975291, 4.339540},
			{0.140157, 0.408783, 11.768969, 4.406428},
		};
		auto bezierPuffed = IntersectionAblePuffed(bezierS.get(), r_08_frez_k);
		std::vector<std::shared_ptr<IntersectionCurve>>interCurves;

		for (int i = 0; i < startParams_baseIntersection.size(); i++) {
			auto fig = figures[i];
			glm::vec4 params;
			params = startParams_baseIntersection[i];



			bool suces = false;
			auto positions = TakePositions(params, &bezierPuffed, &fig.second.second, suces);
			if (!suces)
				continue;

			Reduce(positions.second);
			vectorOfParamsVectors.push_back(positions.first);
		//	scene->AddFigure(std::make_shared< IntersectionCurve>(positions.first, bezierS, scene));
		}


		auto fin_1_body_1 = intersection.PosibleIntersections(vectorOfParamsVectors[0], vectorOfParamsVectors[3]);
		auto body_1_tail_1 = intersection.PosibleIntersections(vectorOfParamsVectors[3], vectorOfParamsVectors[1]);
		auto tail_1_body_2 = intersection.PosibleIntersections(vectorOfParamsVectors[1], vectorOfParamsVectors[7]);

		auto body_2_fin_1 = intersection.PosibleIntersections(vectorOfParamsVectors[7], vectorOfParamsVectors[4]);
		auto body_2_tail_2 = intersection.PosibleIntersections(vectorOfParamsVectors[7], vectorOfParamsVectors[5]);
		auto tail_2_body_1 = intersection.PosibleIntersections(vectorOfParamsVectors[5], vectorOfParamsVectors[3]);

		std::vector<std::vector<vectorIntersectionInfo>> intersections{
			fin_1_body_1, body_1_tail_1, tail_1_body_2,
			body_2_fin_1, body_2_tail_2, tail_2_body_1,
		};
		std::vector<vectorIntersectionInfo> allPoints;
		for (auto vector : intersections)
			allPoints.insert(allPoints.end(), vector.begin(), vector.end());


		const int startId = TheClosedId(vectorOfParamsVectors[0], glm::vec2{ 0.5f, 0.5f });
		int currentId = startId;
		std::vector<glm::vec2> *startParamsVector = &vectorOfParamsVectors[0];
		std::vector<glm::vec2> *currentParamsVector = startParamsVector;
		int direction = 1;
		std::vector<glm::vec3> results;
		results.push_back(frezStartPos);
		results.push_back(frezStartPos);

		std::shared_ptr<BezierInterpolated> bezierInterpolaited = std::make_shared<BezierInterpolated>();
		int switchCount = -1;
		std::vector<glm::vec2> intersectionHoleParams;

		do {
			std::shared_ptr <Point> point = std::make_shared<Point>();
			if (currentId > currentParamsVector->size() - 1) {
				currentId = 0;
				direction = 1;
			}
			else if (currentId < 0) {
				currentId = currentParamsVector->size() - 1;
				direction = -1;
			}
			auto param = currentParamsVector->at(currentId);
			intersectionHoleParams.push_back(param);
			auto position = bezierPuffed.Parametrization(param.x, param.y) - glm::vec3{0, r_08_frez_k, 0};
			results.push_back(TransformOrigin(position));
			point->transpose->SetObjectPosition(position);
			scene->AddFigure(point);
			bezierInterpolaited->Add(point.get());

			for (auto info : allPoints) {
				if (info.ShouldMove(currentParamsVector, param))
				{
					switchCount++;
					auto next = info.MoveToNextCurve(currentParamsVector, param).value();
					std::cout << currentId << std::endl;

					currentParamsVector = next.first;
					currentId = next.second.second;
//					std::cout << switchCount << " " << glm::to_string(info.pos) << " " << current_t << std::endl;
//					auto der = currentCure->Derivative(current_t);
//					auto derLength = glm::dot(der, der);
//					nextParam = 0.5f;
					break;
				}
			}
			currentId += direction;

		} while (startId != currentId || startParamsVector != currentParamsVector);


		results[1] = results[2];
		results[1].y = frezStartPos.y;

		float radiusInBezierSpace = r_08_frez_k / (3.0f * 1.125f);
		GeneratorOfPathInsideConstrain accuratePath;
		auto insideOfHole = accuratePath.TheHole(intersectionHoleParams, bezierS.get(),
			maxHeight  * 0.5f, radiusInBezierSpace, radiusInBezierSpace * holeTolerance);
		std::for_each(insideOfHole.begin(), insideOfHole.end(), [=](glm::vec3& p) {
			p = TransformOrigin(p);
			});

		results.push_back(results[results.size() - 1]);
		results[results.size() - 1].y = frezStartPos.y;
		results.push_back(insideOfHole[0]);
		results[results.size() - 1].y = frezStartPos.y;
		results.append_range(insideOfHole);

		results.push_back(results[results.size() - 1]);
		results[results.size() - 1].y = frezStartPos.y;
		results.push_back(frezStartPos);

		scene->AddFigure(std::make_shared< IntersectionCurve>(intersectionHoleParams, bezierS, scene));
		scene->AddFigure(bezierInterpolaited);
		scene->AddFigure(bezierS);

		return results;
		/*std::string outFile = path + std::string("path_5_hole.k08");
		SavePath(outFile.c_str(), results);*/
	}

	int TheClosedId(std::vector<glm::vec2>& params, glm::vec2 point) {
		float dis = M_FLOAT_MAX;
		int id = -1;
		int i = -1;
		for (auto p : params) {
			i++;
			auto diff = p - point;
			float tmpDis = glm::dot(diff, diff);
			if (tmpDis < dis) {
				dis = tmpDis;
				id = i;
			}
		}
		return id;
	}

	void ConnectTheClosetsEnds(std::vector<glm::vec2>& a, std::vector<glm::vec2>& b) {
		float dis = M_FLOAT_MAX;
		glm::ivec2 contact;
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++) {
				auto diff = a[i * (a.size() - 1)] - b[j * (b.size() - 1)];
				float tmpDis = glm::dot(diff, diff);
				if (dis > tmpDis) {
					contact = { i, j };
					dis = tmpDis;
				}
			}
		if (contact.x == 0)
			std::reverse(a.begin(), a.end());
		if (contact.y == 1)
			std::reverse(b.begin(), b.end());
		a.append_range(b);
	}

	inline glm::vec3 TransformOrigin(const glm::vec3& position) {
		return (position - center) * scale + bease;
	}
	void Reduce(std::vector<glm::vec2>& vector) {
		for (int i = vector.size() - 2; i > 1; i -= 2) {
			vector.erase(std::next(vector.begin(), i));
		}
	}

	std::pair<std::vector<glm::vec2>, std::vector<glm::vec2>> TakePositions(glm::vec4 params, IntersectionAble* object_a, IntersectionAble* object_b, bool& suces) 
	{
		Intersection intersection{};
		try {
			params = intersection.FirstIntersectionPoint(object_a, object_b, params, epsilon);
		}
		catch (...) {
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hConsole, 12);
			std::cout << std::endl << "ERROR" << std::endl;
			SetConsoleTextAttribute(hConsole, 15);
			suces = false;
			return {};
		}

		if (!intersection.IsIntersected(object_a, object_b, params))
		{
			suces = false;
			return{};
		}
		std::cout << params.x << " " << params.y << " " << params.z << " " << params.w << std::endl;

		auto positions = intersection.IntersectionFrame(params, object_a, object_b, 0.005, epsilon);
		suces = true;
		return positions;
	}
};