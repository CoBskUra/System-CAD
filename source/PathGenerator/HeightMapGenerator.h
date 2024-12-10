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
#include <ctime>    
#include <future>

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
	const float r = 10.0f * 0.5f / 50.0f;
	const float scale = 50;
	const glm::vec3 center{ 1.5, 0, 1.5 };
	const glm::vec3 bease{ 0, baseHeight, 0 };

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
		if (ImGui::Button("Sziezki_1.5"))
		{
			Path_1dot5(scene);
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

		auto lastPos = positions[positions.size() - 1];
		lastPos.y = 60;
		positions.push_back(lastPos);
		positions.push_back({0, 60, 0});


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
		auto future_body_1_taile_1 = std::async(std::launch::async, &Intersection::PosibleIntersections_2, &intersection,
			interCurves[0].get(), interCurves[1].get(), r);
		auto future_body_1_leftFin_1 = std::async(std::launch::async, &Intersection::PosibleIntersections_2, &intersection,
			interCurves[0].get(), interCurves[2].get(), r);
		auto future_body_1_rightFin_1 = std::async(std::launch::async, &Intersection::PosibleIntersections_2, &intersection,
			interCurves[0].get(), interCurves[3].get(), r);

		auto future_body_1_taile_2 = std::async(std::launch::async, &Intersection::PosibleIntersections_2, &intersection,
			interCurves[0].get(), interCurves[5].get(), r);

		auto future_body_2_taile_2 = std::async(std::launch::async, &Intersection::PosibleIntersections_2, &intersection,
			interCurves[4].get(), interCurves[5].get(), r);

		auto future_body_2_leftFin_2 = std::async(std::launch::async, &Intersection::PosibleIntersections_2, &intersection,
			interCurves[4].get(), interCurves[6].get(), r);
		auto future_body_2_rightFin_2 = std::async(std::launch::async, &Intersection::PosibleIntersections_2, &intersection,
			interCurves[4].get(), interCurves[7].get(), r);

		auto future_body_2_taile_1 = std::async(std::launch::async, &Intersection::PosibleIntersections_2, &intersection,
			interCurves[4].get(), interCurves[1].get(), r);

		auto future_taile_1_taile_1 = std::async(std::launch::async, &Intersection::PosibleIntersections_2, &intersection,
			interCurves[1].get(), interCurves[1].get(), r);
		auto future_taile_2_taile_2 = std::async(std::launch::async, &Intersection::PosibleIntersections_2, &intersection,
			interCurves[5].get(), interCurves[5].get(), r);

		// Pobieranie wyników
		auto body_1_taile_1 = future_body_1_taile_1.get();
		auto body_1_leftFin_1 = future_body_1_leftFin_1.get();
		auto body_1_rightFin_1 = future_body_1_rightFin_1.get();

		auto body_1_taile_2 = future_body_1_taile_2.get();
		auto body_2_taile_2 = future_body_2_taile_2.get();

		auto body_2_leftFin_2 = future_body_2_leftFin_2.get();
		auto body_2_rightFin_2 = future_body_2_rightFin_2.get();

		auto body_2_taile_1 = future_body_2_taile_1.get();

		auto taile_1_taile_1 = future_taile_1_taile_1.get();
		auto taile_2_taile_2 = future_taile_2_taile_2.get();

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
		results.push_back({ 0, 60, 0 });
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
					auto next = info.MoveToNextCurve(currentCure, current_t);
					std::cout << current_t << std::endl;
					if (currentCure == next.first || switchCount == 9 || switchCount == 4)
					{
						int numberOfOut = 2;
						current_t -= nextParam * numberOfOut;
						float lest = info.t1_1 < info.t2_1 ? info.t1_1 : info.t2_1;
						if(switchCount == 4)
							lest = !(info.t1_1 < info.t2_1) ? info.t1_1 : info.t2_1; // mam to gdzieœ boto ostatnia zmiana a ja jestem zmêczony. Problem jest taki, ¿e to nie jest samo przeciêcie, wiêc za³o¿enie, ¿e idziemy z mniejszej wartoœci do wiêkszej jest b³êdne.
						float larger_1 = lest == info.t1_1 ? info.t1_2 : info.t2_2;
						//if (switchCount == 4 || switchCount == 9 || switchCount == 8)
						if ( switchCount == 3)
							larger_1 = lest + (larger_1 - lest) * 0.3f;
						else if(switchCount == 8)
							larger_1 = lest + (larger_1 - lest) * 0.8f;
						else
							larger_1 = lest + (larger_1 - lest) * 0.5f;
						//else
						//	larger_1 = lest;
						nextParam = (larger_1 - current_t) / numberOfOut;
						for (int i = 0; i < numberOfOut; i++) {
							results.pop_back();
							scene->DeleteLastFigure();
							bezierInterpolaited->Erase(point.get());
						}
						for (int i = 0; i <= 16* numberOfOut; i++) {
							current_t += nextParam * 0.0625 * min(1, i);
							if (current_t > currentCure->MaxValue())
								current_t = current_t - currentCure->MaxValue();
							std::shared_ptr <Point> point2 = std::make_shared<Point>();
							position = intersection.MoveAcrossNormal(current_t, currentCure, r);
							results.push_back((position - center) * scale + bease);
							point2->transpose->SetObjectPosition(intersection.MoveAcrossNormal(current_t, currentCure, r));
							scene->AddFigure(point2);
							bezierInterpolaited->Add(point2.get());
						}

						float larger = info.t1_2 > info.t2_2 ? info.t1_2 : info.t2_2;
						if(currentCure == next.first)
							next.second = larger;
					}

					currentCure = next.first;
					current_t = next.second;
					std::cout << switchCount << " " << glm::to_string(info.pos) << " " << current_t << std::endl;
					auto der = currentCure->Derivative(current_t);
					auto derLength = glm::dot(der, der);
					nextParam = 0.5f;
					if (switchCount == 8 || switchCount == 3)
					{
						nextParam = 0.125f;
					}
					break;
				}
			}
			current_t += nextParam;

		} while (fabsf(current_t - start_t) > nextParam - 0.001 || startCure != currentCure);
		scene->AddFigure(bezierInterpolaited);

		results[1].x = results[3].x;
		results[1].z = - 75 - 5.0f - 0.1f;

		results[2] = results[1];
		results[2].y = baseHeight;
		auto last = results[results.size() - 1];
		last.y = 60;
		results.push_back(last);
		results.push_back({ 0, 60, 0 });

		std::string outFile = std::string("D:/fast_acess/Studia/MINI CAD-CAM/Programowanie urz¹dŸeñ sterowanych numerycznie/Projekty/symulator/MillingCutterSimulator/Paths/sciezki_2.f10");
		SavePath(outFile.c_str(), results);



		for (int i = 0; i < interCurves.size(); i++) {
			scene->AddFigure(interCurves[i]);
		}
		scene->AddFigure(bezierS);
	}

	void Path_1dot5(Scene* scene) {
		auto vertexes = LoadPath("D:/fast_acess/Studia/MINI CAD-CAM/Programowanie urz¹dŸeñ sterowanych numerycznie/Projekty/symulator/MillingCutterSimulator/Paths/sciezki_2.f10");
		float distanceBeetwenPaths = 2 * r - r / 5.0f;
		vertexes.erase(vertexes.begin(), vertexes.begin() + 3);

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
		points.push_back({ 0, 60, 0 });
		points.push_back({ -75 + halfDistanceBeetwenPaths, 60, -75 - distanceBeetwenPaths * 2});

		glm::vec3 pos{ halfDistanceBeetwenPaths, 0, -distanceBeetwenPaths * 2 };
		points.push_back(TransformOrigin(pos));
		glm::vec3 min{ halfDistanceBeetwenPaths * 0.5f, 0, 0 },
			max{ 3 - halfDistanceBeetwenPaths * 0.5f, 5, 3 };
		for(int i =0; i < 38; i++) {
			
			auto block = LegalPosition(vertexes, pos, directionVertical, 0); // obliczanie pozycji przeszkody
			
			float additionalScalar = 1;
			if (i == 2 || i == 30 || i == 31 || i == 35 || i == 16 || i == 15)
				additionalScalar = 0.5f;
			else if(i == 3 || i == 22)
				additionalScalar = 0.2f;

			// nie napotkano przeszkody
			if (isnan(block.first)) { 
				// obcinanie do krawêdzi
				ClampPos(pos + directionVertical * 10.0f, min, max, pos); 
				points.push_back(TransformOrigin(pos));
				pos += directionHorizontal * distanceBeetwenPaths * additionalScalar;
				if (pos.x > max.x)
				{
					directionHorizontal = SwitchDirection(directionHorizontal, left, right);
					ClampPos(pos, min, max, pos);
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
					glm::dot(border - posiblePos.second, border - posiblePos.second) > distanceBeetwenPaths * distanceBeetwenPaths)
					pos = border + directionHorizontal * distanceBeetwenPaths;
				else
				{
					pos = pos - directionVertical * 10.0f + directionHorizontal * distanceBeetwenPaths * additionalScalar;
					pos = LegalPosition(vertexes, pos, directionVertical, 0).second - directionVertical * halfDistanceBeetwenPaths * 0.15f;
				}
				points.push_back(TransformOrigin(pos));
				directionVertical = SwitchDirection(directionVertical, up, down);
			}
			
		}
		auto last = points[points.size() - 1];
		last.y = 60;
		points.push_back(last);
		points.push_back({ 0, 60, 0 });

		std::string outFile = std::string("D:/fast_acess/Studia/MINI CAD-CAM/Programowanie urz¹dŸeñ sterowanych numerycznie/Projekty/symulator/MillingCutterSimulator/Paths/") + std::string("sciezki_1.5.f10");
		SavePath(outFile.c_str(), points);
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

	inline glm::vec3 TransformOrigin(const glm::vec3& position) {
		return (position - center) * scale + bease;
	}
};