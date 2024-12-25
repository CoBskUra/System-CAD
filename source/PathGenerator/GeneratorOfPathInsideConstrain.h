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
#include "Helper/MathOperations.h"

class GeneratorOfPathInsideConstrain {
	struct ConstrainData {
		int id;
		std::vector<glm::vec2> params{};
		std::vector<glm::vec2> innerParams{};
		std::vector<glm::vec3> result{};
		glm::vec2 field_u;
		glm::vec2 field_v;
		float offset;
		float maxHeight = 0;
		float needToRise = M_FLOAT_MAX;

		void UpdateMaxHeight(float height) {
			if (maxHeight < height)
				maxHeight = height;
		}
	};

	struct IntersectionsPoint {
		int id;
		glm::vec2 param;
		float s;
		bool isInner = false;
	};

	Intersection intersection{};
public:
	float m_height = 4.0f / 5.0f;

	float m_heightOffset = 1.0f / 5.0f;

public:
	std::vector<glm::vec3> GeneratePathFromConstrains(Scene* scene, std::string path, std::vector<std::pair<std::shared_ptr<Figure>, std::pair<IntersectionAble*, IntersectionAblePuffed>>> figures, float r) {
		if (!FileManager::doesFolderExist(path.c_str()))
		{
			return {};
		}

		std::map<int, ConstrainData > constrains;
		for (auto& fileName : FileManager::getFilesNamesInDirectory(path.c_str()))
		{
			auto pos = fileName.find('.');
			std::string exe = fileName.substr(pos + 1);
			ConstrainData cd;
			int end;
			cd.field_u = ConvertStringToVec2(exe, '_', 0, end);
			cd.field_v = ConvertStringToVec2(exe, '_', end, end);
			cd.id = std::stoi(fileName.substr(0, fileName.find('_')));
			cd.params = FileManager::loadVec2VectorFromFile(path + fileName);
			cd.needToRise = m_height;
			if (cd.id == 3 || cd.id == 7)
				cd.needToRise = -1;
			if (constrains.find(cd.id) == constrains.end())
			{
				float center = cd.field_u.x;
				if (cd.id == 5 || cd.id == 9)
					center = center + (cd.field_u.x + cd.field_u.y) * 0.5f;
				cd.offset = -MathOperations::MinDisFromCenter_Di(1, cd.params, cd.field_u, center, true) - center;
				std::for_each(cd.params.begin(), cd.params.end(), [=](glm::vec2& d) { d.y = MathOperations::SimpleWrap(d.y + cd.offset, cd.field_u); });
				constrains.insert({ cd.id, cd });
			}
			else
			{
				std::for_each(cd.params.begin(), cd.params.end(), [=](glm::vec2& d) { d.y = MathOperations::SimpleWrap(d.y + cd.offset, cd.field_u); });
				constrains.at(cd.id).innerParams = cd.params;
			}
		}


		std::vector<glm::vec3> result;
		result.push_back({ 1.5, m_height, 1.5 });
		for (auto& pair : constrains) {
			ConstrainData &cd = pair.second;

			float disFromBorder = 0.01;
			float step_1 = 0.075;
			float step_2 = 0.075;
			if (cd.id == 0 || cd.id == 1)
			{
				step_1 = 0.025f;
				step_2 = 0.05f;
			}
			else if (cd.id == 5 || cd.id == 9)
			{
				step_1 = 0.1;
				cd.needToRise = constrains[cd.id - 3].maxHeight + 1.5f * r;
			}
			glm::vec2 start{ cd.field_v.x, step_1 };
			glm::vec2 end{ cd.field_v.y, step_1 };
			float border = MathOperations::MaxDisFromCenter_Di(1, cd.params, cd.field_u, 0, true);
			bool wasInner = false;

			int iterations = border / step_1 - 2;
			for (float startLine = step_1; startLine < border - step_1; startLine += step_1) {


				start.y = startLine;
				end.y = startLine;
				
				auto allIntersections = TakeIntersectionsPoints(start, end, cd);
				// sepecial cases
				if (cd.id == 5 || cd.id == 9) {
					if (!wasInner && ContainInnerIntersection(allIntersections))
					{
						wasInner = true;
						cd.needToRise = constrains[0].maxHeight;
					}
					else if (wasInner && !ContainInnerIntersection(allIntersections))
					{
						cd.needToRise = constrains[cd.id - 1].maxHeight + 1.5f * r;
					}
				}



				for (int i = 0; i < allIntersections.size() - 1; i+=2) {
					if (allIntersections[i].isInner && allIntersections[i + 1].isInner)
						continue;

					float sign = allIntersections[i].param.x < allIntersections[i + 1].param.x ? 1 : -1;
					float startCurentStep = allIntersections[i].param.x + disFromBorder * sign;
					float offsetedLine = MathOperations::SimpleWrap(startLine - cd.offset, cd.field_u);

					if (i != 0)
					{
						cd.result.push_back(TakeLastAndSet_y(cd.result, cd.needToRise));

						auto tmpPos = figures[cd.id].second.second.Parametrization(startCurentStep, offsetedLine);
						tmpPos.y = cd.needToRise;
						cd.result.push_back(tmpPos);
					}

					if (sign > 0) {
						for (float currStep = startCurentStep; currStep < allIntersections[i + 1].param.x - disFromBorder; currStep += step_2) {
							glm::vec3 pos = figures[cd.id].second.second.Parametrization(currStep, offsetedLine) - glm::vec3{ 0, r, 0 };
							cd.result.push_back(pos);
							cd.UpdateMaxHeight(pos.y);
						}
					}
					else {
						for (float currStep = startCurentStep; currStep > allIntersections[i + 1].param.x + disFromBorder; currStep -= step_2) {
							glm::vec3 pos = figures[cd.id].second.second.Parametrization(currStep, offsetedLine) - glm::vec3{ 0, r, 0 };
							cd.result.push_back(pos);
							cd.UpdateMaxHeight(pos.y);
						}
					}
				}

				std::swap(start, end);

			}
			//result.push_back(TakeLastAndSet_y(result, m_height));


			if (cd.id == 0 || cd.id == 1) {
				constrains[5].needToRise = cd.maxHeight;
				constrains[9].needToRise = cd.maxHeight;
				float offset = cd.offset;
				auto field = cd.field_u;
				std::for_each(cd.params.begin(), cd.params.end(), [=](glm::vec2& d) { d.y = MathOperations::SimpleWrap(d.y - offset, field); });
				start.x = cd.field_v.x;
				end.x = cd.field_v.y;
				end.y = start.y = (field.x + field.y) * 0.5f;
				auto allIntersections = TakeIntersectionsPoints(start, end, cd);
				cd.result.push_back(TakeLastAndSet_y(cd.result, cd.needToRise));
				for (float currStep = allIntersections[0].param.x; currStep < allIntersections[1].param.x; currStep += 0.025) {
					glm::vec3 pos = figures[cd.id].second.first->Parametrization(currStep, start.y);
					//cd.result.push_back(pos);
					cd.UpdateMaxHeight(pos.y);
				}

			/*	std::for_each(constrains.begin(), constrains.end(), [=](ConstrainData cdTmp) {
					cdTmp.needToRise = cd.maxHeight;
					});*/
			}

			if (cd.id == 3 || cd.id == 7) {
				std::for_each(cd.result.begin(), cd.result.end(), [=](glm::vec3& p) {
					if (p.y == -1)
						p.y = cd.maxHeight;
					});
			}
			//result.push_back(TakeLastAndSet_y(result, m_height));
		}

		std::reverse(constrains[5].result.begin(), constrains[5].result.end());
		std::reverse(constrains[9].result.begin(), constrains[9].result.end());

		int order[]{
			4, 5, 0, 2, 3,
			8, 9, 1, 6, 7
		};
		float tmpHeight = constrains[0].maxHeight;
		for (int i = 0; i < constrains.size(); i++) {
			auto pos = constrains[order[i]].result[0];
			pos.y = tmpHeight;
			result.push_back(pos);
			result.append_range(constrains[order[i]].result);
			result.push_back(TakeLastAndSet_y(constrains[order[i]].result, tmpHeight));
		}

		result.push_back(TakeLastAndSet_y(result, m_height));
		for (int i = 5; i < 10; i += 4) {
			float offset = constrains[i].offset;
			auto field = constrains[9].field_u;
			std::for_each(constrains[i].params.begin(), constrains[i].params.end(), [=](glm::vec2& d) { d.y = MathOperations::SimpleWrap(d.y - offset, field); });
			std::for_each(constrains[i].innerParams.begin(), constrains[i].innerParams.end(), [=](glm::vec2& d) { d.y = MathOperations::SimpleWrap(d.y - offset, field); });
		}
		IntersectionAble* inter_1 = &figures[5].second.second;
		IntersectionAble* inter_2 = &figures[9].second.second;
		auto fish_1_fun = [=](glm::vec2 a)
			{ return inter_1->Parametrization(a.x, a.y) - glm::vec3{ 0, r, 0 }; };
		auto fish_2_fun = [=](glm::vec2 a)
			{ return inter_2->Parametrization(a.x, a.y) - glm::vec3{ 0, r, 0 }; };
		auto ids = IdTheClosestParams(constrains[5].params, fish_1_fun,
			constrains[9].params,fish_2_fun);

		int size;
		/*int size = result.size();
		result.push_back({ 0, 0, 0 });
		for (auto& pos : constrains[9].innerParams)
		{
			result.push_back(fish_2_fun(pos));
		}
		result[size] = result[size + 1];
		result[size].y = tmpHeight;*/

		result.push_back(TakeLastAndSet_y(result, tmpHeight));
		size = result.size();
		result.push_back({ 0, 0, 0 });

		MathOperations::AppendVector<glm::vec3, glm::vec2>(result, constrains[9].params, ids.y + 1, constrains[9].params.size(), fish_2_fun);
		MathOperations::AppendVector<glm::vec3, glm::vec2>(result, constrains[9].params, 0, ids.y + 1, fish_2_fun);
		result[size] = result[size + 1];
		result[size].y = tmpHeight;

		result.push_back(TakeLastAndSet_y(result, tmpHeight));
		size = result.size();
		result.push_back({ 0, 0, 0 });
		MathOperations::AppendVector<glm::vec3, glm::vec2>(result, constrains[5].params, ids.x, constrains[5].params.size(), fish_1_fun);
		MathOperations::AppendVector<glm::vec3, glm::vec2>(result, constrains[5].params, 0, ids.x, fish_1_fun);
		result[size] = result[size + 1];
		result[size].y = tmpHeight;

		result.push_back(TakeLastAndSet_y(result, tmpHeight));
		/*size = result.size();
		result.push_back({ 0, 0, 0 });
		for (auto& pos : constrains[5].innerParams)
		{
			result.push_back(fish_1_fun(pos));
		}

		result.push_back(TakeLastAndSet_y(result, tmpHeight));
		result[size] = result[size + 1];
		result[size].y = tmpHeight;*/

		return result;
	}


	glm::vec3 TakeLastAndSet_y(std::vector<glm::vec3>& vector, float y) {
		auto lastPos = vector[vector.size() - 1];
		lastPos.y = y;
		return lastPos;
	}

	std::vector<IntersectionsPoint> TakeIntersectionsPoints(glm::vec2 start, glm::vec2 end, ConstrainData& cd) {
		auto outInter = IntersectionPoints(start, end, cd.params, false);
		auto inInter = IntersectionPoints(start, end, cd.innerParams, true);

		if (outInter.size() % 2 != 0 || outInter.size() == 0 || inInter.size() % 2 != 0)
		{
			std::cout << "cos";
			throw "Too many or too little intersection points";
		}
		std::vector< IntersectionsPoint> allIntersections;
		allIntersections.append_range(outInter);
		allIntersections.append_range(inInter);
		std::sort(allIntersections.begin(), allIntersections.end(), [](IntersectionsPoint& a, IntersectionsPoint& b)
			{
				return a.s < b.s;
			});
		return allIntersections;
	}


	glm::vec2 ConvertStringToVec2(std::string s, char det, int offset, int& end) {
		glm::vec2 result{ 0, 0 };
		int pos = s.find(det, offset);
		result.x = std::stof(s.substr(0, pos));
		int pos2 = s.find(det, pos + 1);
		result.y = std::stof(s.substr(pos + 1, pos2 - pos));
		end = pos2 + 1;
		return result;
	}

	std::vector<IntersectionsPoint> IntersectionPoints(glm::vec2 start, glm::vec2 end, std::vector<glm::vec2>& vector, bool isInner) {
		std::vector<IntersectionsPoint> results{};
		for (int i = 0; i < (int)vector.size() - 1; i++) {
			bool isOneIntersection = false;
			float s;
			intersection.VectorIntersection(start, end, 
				vector[i], vector[i + 1], isOneIntersection, s);

			if (isOneIntersection)
				results.push_back(IntersectionsPoint{ i, start + s * (end - start), s, isInner });
		}


		std::sort(results.begin(), results.end(), [](IntersectionsPoint& a, IntersectionsPoint& b)
			{
				return a.s > b.s;
			});
		return results;
	}

	glm::ivec2 IdTheClosestParams(std::vector<glm::vec2> &vector_1, std::function<glm::vec3(glm::vec2)> inter_1, std::vector<glm::vec2>& vector_2, std::function<glm::vec3(glm::vec2)> inter_2) {
		float dis = M_FLOAT_MAX;
		glm::ivec2 ids{ 0, 0 };

		for (int i = 0; i < vector_1.size(); i++) {
			auto p1 = inter_1(vector_1[i]);
			for (int j = 0; j < vector_2.size(); j++) {
				auto p2 = inter_2(vector_2[j]);
				glm::vec3 diff = p1 - p2;
				float tmpDis = glm::dot(diff, diff);
				if (tmpDis < dis) {
					dis = tmpDis;
					ids = { i, j };
				}
			}
		}
		return ids;
	}

	std::vector<glm::vec3> TheHole(std::vector<glm::vec2>& paramVector, IntersectionAble* intersectionAble, float height, float radius,
		float disBetweenPaths) {
		ConstrainData cd;
		{
			cd.needToRise = height;
			cd.field_u = intersectionAble->Field_u();
			cd.field_v = intersectionAble->Field_v();
			cd.params = paramVector;
			cd.id = 0;
			cd.offset = 0;
		}

		float start_v = MathOperations::MinDisFromCenter_Di(0, cd.params, cd.field_v, 0, true);
		float end_v = MathOperations::MaxDisFromCenter_Di(0, cd.params, cd.field_v, 0, true);

		float disFromBorder = radius * 0.25f;

		glm::vec2 start{ start_v + disFromBorder, cd.field_u.x };
		glm::vec2 end{ start_v + disFromBorder, cd.field_u.y };

		bool liftFrez = false;
		for (float startLine = start_v + disFromBorder; startLine < end_v - disFromBorder; startLine += disBetweenPaths) {


			start.x = startLine;
			end.x = startLine;

			auto allIntersections = TakeIntersectionsPoints(start, end, cd);
			if (cd.result.size() > 0)
			{
				auto diff = intersectionAble->Parametrization(allIntersections[0].param.x, allIntersections[0].param.y) - cd.result[cd.result.size() - 1];

				if (glm::dot(diff, diff) > disBetweenPaths * 4) {
					liftFrez = true;
				}
			}


			for (int i = 0; i < allIntersections.size() - 1; i += 2) {
				if (allIntersections[i].isInner && allIntersections[i + 1].isInner)
					continue;

				float signe = start.y < end.y ? 1.0f : -1.0f;

				auto intPoint_0 = allIntersections[i + 0];
				auto intPoint_1 = allIntersections[i + 1];

				if (liftFrez) {
					cd.result.push_back(TakeLastAndSet_y(cd.result, cd.needToRise));

					auto tmpPos = intersectionAble->Parametrization(intPoint_0.param.x, intPoint_0.param.y + disFromBorder * signe);
					tmpPos.y = cd.needToRise;
					cd.result.push_back(tmpPos);
				}
				
				cd.result.push_back(intersectionAble->Parametrization(intPoint_0.param.x, intPoint_0.param.y + disFromBorder * signe));
				cd.result.push_back(intersectionAble->Parametrization(intPoint_1.param.x, intPoint_1.param.y - disFromBorder * signe));

				liftFrez = true;
			}
			liftFrez = false;
			std::swap(start, end);
		}

		return cd.result;
	}



	bool ContainInnerIntersection(std::vector<IntersectionsPoint>& vector) {
		return std::any_of(vector.begin(), vector.end(), [](IntersectionsPoint& ip) {
			return ip.isInner;
			});
	}



	void Reduce(std::vector<glm::vec2>& vector) {
		for (int i = vector.size() - 2; i > 1; i -= 2) {
			vector.erase(std::next(vector.begin(), i));
		}
	}


};