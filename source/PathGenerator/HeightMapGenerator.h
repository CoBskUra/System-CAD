#pragma once
#include "Figures/Intersection/Intersection.h"
#include <SceneManadement/Scene.h>
#include <limits>
#include <omp.h>
#include <stdio.h>
#include "Figures/Intersection/Intersection.h"
#include <Figures/Bezier/IntersectionCurve.h>
#include <Figures/Bezier/Surface/BezierSurfaceC0.h>
#include "glm/gtx/string_cast.hpp"

class HeightMapGenerator {
	Texture heightTexture{ GL_TEXTURE_2D };
	int resolution_x; int resolution_z;
	glm::vec3 start; glm::vec3 end;
	std::vector<float> heights;
	float maxHeight = 1;
	float baseHeight = 20;
	float tolerance = 0.95;
	float error = 2;
	float epsilon = 0.000001;

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
		if (ImGui::Button("Sciezki_1")) {
			LoadHeights();
			GeneratePaths(tolerance);
		}
		if (ImGui::Button("Sziezki_2"))
		{
			Path_2(scene);
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
			float sample_u = obj->Field_u().y * 200;
			float sample_v = obj->Field_v().y * 200;
			for (int i = 0; i <= sample_v; i++) {
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
		float r = 8;
		float size_x = 150;
		float size_z = 150;
		float size_y = 50;

		glm::vec3 leftBottom{ -size_x * 0.5, 0, -size_z * 0.5 };
		glm::vec3 rightTop{ size_x * 0.5, 0, size_z * 0.5 };

		std::vector<glm::vec3> positions;
		positions.reserve(resolution_x * resolution_z);

		float nextPaths = r * tolerance;
		bool swap = false;
		positions.push_back(glm::vec3{ 0, 60, 0 } );
		positions.push_back(glm::vec3{ 0, 60, 0 }+ leftBottom);
		bool last = true;
		for (float x = 0; x <= size_x || last; x += nextPaths)
		{
			if (x > size_x)
			{
				last = false;
				x = size_x;
			}
			if (!swap)
			{
				auto pos_last = MaxHeight(x, 0, r, size_x, size_z, size_y);
				positions.push_back(pos_last + leftBottom);
				for (float z = 0; z <= size_z; z++) {
					auto pos = MaxHeight(x, z, r, size_x, size_z, size_y);
					positions.push_back(pos + leftBottom);
				}
			}
			else {
				auto pos_last = MaxHeight(x, size_z, r, size_x, size_z, size_y);
				positions.push_back(pos_last + leftBottom);
				for (float z = size_z; z >= 0; z--) {
					auto pos = MaxHeight(x, z, r, size_x, size_z, size_y);
					positions.push_back(pos + leftBottom);
				}
			}
			swap = !swap;
		}

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

		SavePath("D:/fast_acess/Studia/MINI CAD-CAM/Programowanie urz¹dŸeñ sterowanych numerycznie/Projekty/symulator/MillingCutterSimulator/Paths/sciezki.k16", positions);

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

	glm::vec3 MaxHeight(float x, float z, float r, float size_x, float size_z, float scalar_y) {
		glm::vec3 max{ x, baseHeight + error, z};
		float r_invers = 1 / r;
		float sampleSize_x = resolution_x / size_x;
		float sampleSize_z = resolution_z / size_z;
		float inversSampleSize_x = size_x / resolution_x;
		float inversSampleSize_z = size_z / resolution_z;
		int x_sample = resolution_x * r / size_x + 2;
		int z_sample = resolution_z * r / size_z + 2;
		for (int i = -x_sample * 0.5f; i <= x_sample * 0.5f; i++) {
			float r_x = inversSampleSize_x * i;
			int id_x = (x + r_x) * sampleSize_x;
			for (int j = -z_sample *0.5f; j <= z_sample * 0.5f; j++) {
				float r_z = inversSampleSize_z * j;
				int id_z = (z + r_z) * sampleSize_z;
				if (id_x >= 0 && id_z >= 0 && id_x < resolution_x &&
					id_z < resolution_z && r_z * r_x <= r * r && r_z <= r && r_x <= r)
				{
					float frez_h = (r_x * r_x + r_z * r_z) * 0.5 * r_invers;
					//float frez_h = 0;
					float h = baseHeight + heights[id_x * resolution_z + id_z] * scalar_y - frez_h;
					if (max.y < h)
					{
						max.y = h;
					}
				}
			}
		}
		return max;
	}

	void Path_2(Scene* scene) {
		auto bezierS = BezierSurfaceC0::CreateSurfaceShard_ptr("bezier", 1, 1, { 1.5f, 0, 1.5f });
		float r = 10.0f * 0.5 / 50.0f;
		float scale = 50;
		glm::vec3 center{ 1.5, 0, 1.5 };
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
			return;
		// magiczne liczby specjalnie dobrane do modelu rybki
		std::vector<glm::vec4> parametrs{
			{4.18142, 4.8917, 0.515263, 0.90365},		//body_1
			{0.952650, 0.948637, 0.923800, 0.700244},	// { 4.70208, 1.7829, 0.682356, 0.879365 },		//tail_1
			{2.86604, 0.564236, 0.137005, 0.633529},	//left_fin_1
			{2.20182, 0.5176,	0.353158, 0.691447},	//right_fin_1
			{14.7044, 1.15206, 0.665591, 0.749578},		//body_2
			{0.501201, 2.47143, 0.125389, 0.318672},//{2.45367, 0.328755, 0.132424, 0.130977},	//tail_2
			{4.16238, 1.19356, 0.84236, 0.420706},		//left_fin_2
			{4.26343, 0.550016, 0.69163, 0.436241}		//right_fin_2
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
			params = intersection.FirstIntersectionPoint(interObj, bezierS.get(), params);

			if (!intersection.IsIntersected(interObj, bezierS.get(), params))
				continue;
			
			std::cout << params.x << " " << params.y << " " << params.z << " " << params.w << std::endl;
			std::cout << interObj->Parametrization(params.x, params.y).x << " " << interObj->Parametrization(params.x, params.y).y << " " << interObj->Parametrization(params.x, params.y).z << std::endl;
			auto positions = intersection.IntersectionFrame(params, interObj, bezierS.get(), 0.01, epsilon);
			interCurves.push_back(std::make_shared< IntersectionCurve>(positions.second, bezierS, scene));

			if (k == 0 || k == 4 ) // magiczna zimenna upewniaj¹ca siê, ¿e pochodne s¹ w dobrym kierunku
				interCurves[k]->Reverse();

			k++;
		}
		auto t = intersection.TheClosetTo(interCurves[0].get(), {0, 0, 3});
		glm::vec2 params{ t, interCurves[0]->PositionOnCurve(t).x };
		/*params = intersection.IntersectionBezier_2D_XZ(params, interCurves[0].get(), {0, 3}, {1, 0}, 6.0f / 5.0f);*/

		std::cout << params.x << " " << params.y;
		auto body_1_taile_1 = intersection.PosibleIntersections_2(interCurves[0].get(), interCurves[1].get(), r);
		auto body_1_leftFin_1 = intersection.PosibleIntersections_2(interCurves[0].get(), interCurves[2].get(), r);
		auto body_1_rightFin_1 = intersection.PosibleIntersections_2(interCurves[0].get(), interCurves[3].get(), r);

		auto body_1_taile_2 = intersection.PosibleIntersections_2(interCurves[0].get(), interCurves[5].get(), r);

		auto body_2_taile_2 = intersection.PosibleIntersections_2(interCurves[4].get(), interCurves[5].get(), r);

		auto body_2_leftFin_2 = intersection.PosibleIntersections_2(interCurves[4].get(), interCurves[6].get(), r);
		auto body_2_rightFin_2 = intersection.PosibleIntersections_2(interCurves[4].get(), interCurves[7].get(), r);

		auto body_2_taile_1 = intersection.PosibleIntersections_2(interCurves[4].get(), interCurves[1].get(), r);


		auto taile_1_taile_1 = intersection.PosibleIntersections_2(interCurves[1].get(), interCurves[1].get(), r);
		auto taile_2_taile_2 = intersection.PosibleIntersections_2(interCurves[5].get(), interCurves[5].get(), r);

		std::vector<std::vector<cureIntersectionInfo>> intersections{
			body_1_taile_1, body_1_leftFin_1, body_1_rightFin_1, body_1_taile_2,
			body_2_taile_2, body_2_leftFin_2, body_2_rightFin_2, body_2_taile_1,
			taile_1_taile_1, taile_2_taile_2
		};
		std::vector<cureIntersectionInfo> allPoints;
		for (auto vector : intersections)
			allPoints.insert(allPoints.end(), vector.begin(), vector.end());


		std::shared_ptr<BezierInterpolated> bezierInterpolaited = std::make_shared<BezierInterpolated>();
		BezierCurve* startCure = interCurves[0].get();
		float start_t = interCurves[0]->MaxValue() * 0.75;
		BezierCurve* currentCure = startCure;
		float current_t = start_t;
		int switchCount = -1;
		float dis = 0.49;
		float nextParam = 0.5f;
		std::cout << std::endl;
		std::vector<glm::vec3> results;
		results.reserve(1024);
		results.push_back({ 0, 60, 0 });
		results.push_back({ 0, 60, 0 });
		glm::vec3 bease{ 0, baseHeight, 0 };
		do {
			std::shared_ptr <Point> point = std::make_shared<Point>();
			if (current_t > currentCure->MaxValue())
				current_t = 0;
			auto position = intersection.MoveAcrossNormal(current_t, currentCure, r);
			results.push_back((position - center) * scale + bease);
			point->transpose->SetObjectPosition(intersection.MoveAcrossNormal(current_t, currentCure, r));
			scene->AddFigure(point);
			bezierInterpolaited->Add(point.get());

			for (auto info : allPoints) {
				if (info.ShouldMove(currentCure, current_t, nextParam - 0.001))
				{
					switchCount++;
					auto next = info.MoveToNextCurve(currentCure);
					std::cout << current_t << std::endl;
					currentCure = next.first;
					current_t = next.second;
					std::cout << switchCount << " " << glm::to_string(info.pos) << " " << current_t << std::endl;
					auto der = currentCure->Derivative(current_t);
					auto derLength = glm::dot(der, der);
					nextParam = 0.5f;
					if (switchCount == 8)
						nextParam = 0.125f;
					break;
				}
			}
			if ((switchCount == 2 && current_t >= 112.0f) || (switchCount == 7 && current_t >= 120.0))
				nextParam = 0.125f;
			current_t += nextParam;

		} while (fabsf(current_t - start_t) > nextParam - 0.001 || startCure != currentCure);
		scene->AddFigure(bezierInterpolaited);
		
		results[1].x = results[2].x;
		results[1].z = results[2].z;
		SavePath("D:/fast_acess/Studia/MINI CAD-CAM/Programowanie urz¹dŸeñ sterowanych numerycznie/Projekty/symulator/MillingCutterSimulator/Paths/sciezki_2.f10", results);


		/*for (int i = 0; i < interCurves.size(); i++) {
			for (int j = i + 1; j < interCurves.size(); j++) {
				auto posibleIntersectionPoints = intersection.PosibleIntersections(interCurves[i].get(), interCurves[j].get(), r);
				intersectionPoints.push_back(posibleIntersectionPoints);
			}
		}*/
		/*for (auto pip : posibleIntersectionPoints)
		{
			auto p = intersection.IntersectionBezier_2D_XZ(pip, interCurves[0].get(), interCurves[1].get(), r);
			std::cout << glm::to_string(interCurves[0]->PositionOnCurve(p.x)) << "\n" << glm::to_string(interCurves[1]->PositionOnCurve(p.y));
		}*/
		/*std::vector<std::shared_ptr<Point>> points;
		points.reserve(20);
		int cos = 0;
		for (auto params : intersections) {
			int id = 0;
			if (cos < 4)
				id = 0;
			else if (cos < 8)
				id = 4;
			else if (cos == 8)
				id = 1;
			else
				id = 5;

			auto curve = interCurves[id];
			for (auto p : params) {
				std::shared_ptr <Point> point = std::make_shared<Point>();
				point->transpose->SetObjectPosition({p.pos.x, 0, p.pos.y});
				scene->AddFigure(point);
			}
			cos++;
		}*/

		for (int i = 0; i < interCurves.size(); i++) {
			scene->AddFigure(interCurves[i]);
		}
		scene->AddFigure(bezierS);

		/*for (int i = 0; i < 5; i++) {
			std::shared_ptr <Point> start = std::make_shared<Point>();
			start->transpose->SetObjectPosition(intersection.MoveAcrossNormal(interCurves[0]->MaxValue() * 0.75 + i * 10, interCurves[0].get(), r));
			scene->AddFigure(start);
		}*/
	}


};