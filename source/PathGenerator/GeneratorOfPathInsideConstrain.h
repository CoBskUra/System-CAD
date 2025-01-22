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
			if (cd.id == 0 || cd.id == 1) {
				FillGaps(cd.params, 0.18);
			}
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
				FillGaps(cd.params, 0.18, 0.65, 6.0f);
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
				step_1 = 0.015625;
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
			bool extraRise = false;
			bool jumpedOverBlade = false;
			bool beforeJumpOverBlade = false;
			bool strangeSpecialCaseWithTail_7 = false;

			int iterations = border / step_1 - 2;
			int i = 0;
			int jumpCount = -1;
			for (float startLine = step_1*0.5f; startLine < border - step_1*0.5f; startLine += step_1) {
				i++;

				start.y = startLine;
				end.y = startLine;

				if (cd.id == 0 || cd.id == 1) {
					float center = (cd.field_u.x + cd.field_u.y) * 0.5f + cd.offset;
					if (startLine < center && startLine + step_1 >  center) {
						start.y = center - 0.0000025;
						end.y = start.y;
						beforeJumpOverBlade = true;
					}
					else if (startLine > center && startLine - step_1 < center) {
						start.y = center + 0.0000025;
						end.y = start.y;
						cd.result.push_back(TakeLastAndSet_y(cd.result, cd.needToRise));
						extraRise = true;
						jumpedOverBlade = true;
					}
				}
				
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
				if (jumpedOverBlade) {
					jumpedOverBlade = false;
					allIntersections[0].param.x += 2.0f * disFromBorder;
				}

				if (beforeJumpOverBlade) {
					beforeJumpOverBlade = false;
					allIntersections[1].param.x -= 4.0f * disFromBorder;
				}


				for (int i = 0; i < allIntersections.size() - 1; i+=2) {
					if (allIntersections[i].isInner && allIntersections[i + 1].isInner)
						continue;

					float sign = allIntersections[i].param.x < allIntersections[i + 1].param.x ? 1 : -1;
					float startCurentStep = allIntersections[i].param.x + disFromBorder * sign;
					float offsetedLine = MathOperations::SimpleWrap(startLine - cd.offset, cd.field_u);

					bool strangeSpecialCaseWithTail_7 = false;
					if (i != 0 || extraRise)
					{
						jumpCount++;
						auto tmpPos = figures[cd.id].second.second.Parametrization(startCurentStep, offsetedLine);
						
						tmpPos.y = cd.needToRise;
						strangeSpecialCaseWithTail_7 = cd.id == 7 && jumpCount == 1;
						if (!strangeSpecialCaseWithTail_7 || tmpPos.y >= r) {
							cd.result.push_back(TakeLastAndSet_y(cd.result, cd.needToRise));
							cd.result.push_back(tmpPos);
						}
						extraRise = false;
					}

					if (sign > 0) {
						for (float currStep = startCurentStep; currStep < allIntersections[i + 1].param.x - disFromBorder; currStep += step_2) {
							glm::vec3 pos = figures[cd.id].second.second.Parametrization(currStep, offsetedLine) - glm::vec3{ 0, r, 0 };
							if (!strangeSpecialCaseWithTail_7 || pos.y >= 0) {
								cd.result.push_back(pos);
								cd.UpdateMaxHeight(pos.y);
							}
						}
						glm::vec3 pos = figures[cd.id].second.second.Parametrization(allIntersections[i + 1].param.x - disFromBorder, offsetedLine) - glm::vec3{ 0, r, 0 };

						if (!strangeSpecialCaseWithTail_7 || pos.y >= 0) {
							cd.result.push_back(pos);
							cd.UpdateMaxHeight(pos.y);
						}

					}
					else {
						for (float currStep = startCurentStep; currStep > allIntersections[i + 1].param.x + disFromBorder; currStep -= step_2) {
							glm::vec3 pos = figures[cd.id].second.second.Parametrization(currStep, offsetedLine) - glm::vec3{ 0, r, 0 };

							if (!strangeSpecialCaseWithTail_7 || pos.y >= 0) {
								cd.result.push_back(pos);
								cd.UpdateMaxHeight(pos.y);
							}

						}
						glm::vec3 pos = figures[cd.id].second.second.Parametrization(allIntersections[i + 1].param.x + disFromBorder, offsetedLine) - glm::vec3{ 0, r, 0 };

						if (!strangeSpecialCaseWithTail_7 || pos.y >= 0) {
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

				cd.result.push_back(TakeLastAndSet_y(cd.result, cd.needToRise));
				int size = cd.result.size();
				cd.result.push_back(TakeLastAndSet_y(cd.result, cd.needToRise));
				MakeTopOfBezierC0(cd, figures[cd.id].second.first, r);
				cd.result[size] = cd.result[size + 1];
				cd.result[size].y = cd.needToRise;

				/*std::for_each(constrains.begin(), constrains.end(), [=](auto& cdTmp) {
					cdTmp.second.needToRise = cd.maxHeight;
					});*/
			}
			

			if (cd.id != 0 && cd.id != 1 &&
				cd.id != 2 && cd.id != 6 &&
				cd.id != 5 && cd.id != 9) {
				GenerateObjectBorder(cd, &figures[cd.id].second.second, r);
			}

			if (cd.id == 3 || cd.id == 7) {
				std::for_each(cd.result.begin(), cd.result.end(), [=](glm::vec3& p) {
					if (p.y == -1)
						p.y = cd.maxHeight;
					});
			}

			if (cd.id == 7) {
				constrains[7].result.erase(
					std::remove_if(constrains[7].result.begin(), constrains[7].result.end(), [](const glm::vec3& v) {
						return v.y < 0;
						}),
					constrains[7].result.end()
				);
			}
			
			//result.push_back(TakeLastAndSet_y(result, m_height));
		}

		std::reverse(constrains[5].result.begin(), constrains[5].result.end());
		std::reverse(constrains[9].result.begin(), constrains[9].result.end());

		

		constrains[7].result;
		int order[]{
			4, 5, 2, 0,  3,
			8, 9, 6, 1,  7
		};
		
		int lenght = sizeof(order) / sizeof(order[0]);;
		float tmpHeight = constrains[0].maxHeight;
		for (int i = 0; i < lenght; i++) {
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
		size = result.size();
		result.push_back({ 0, 0, 0 });
		for (auto& pos : constrains[9].innerParams)
		{
			result.push_back(fish_2_fun(pos));
		}
		result[size] = result[size + 1];
		result[size].y = tmpHeight;

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
		size = result.size();
		result.push_back({ 0, 0, 0 });
		for (auto& pos : constrains[5].innerParams)
		{
			result.push_back(fish_1_fun(pos));
		}

		

		result.push_back(TakeLastAndSet_y(result, tmpHeight));
		result[size] = result[size + 1];
		result[size].y = tmpHeight;

		return result;
	}

	void MakeTopOfBezierC0(ConstrainData& cd, IntersectionAble* inter, float r, float step = 0.025) {
		glm::vec3 start, end;
		float centerOfField_u = (cd.field_u.x + cd.field_u.y) * 0.5f;
		float centerOfField_v = (cd.field_v.x + cd.field_v.y) * 0.5f;
		std::for_each(cd.params.begin(), cd.params.end(), [=](glm::vec2& d) { d.y = MathOperations::SimpleWrap(d.y - cd.offset, cd.field_u); });
		start.x = cd.field_v.x;
		end.x = cd.field_v.y;
		end.y = start.y = centerOfField_u;
		auto allIntersections = TakeIntersectionsPoints(start, end, cd);

		for (float currStep = allIntersections[0].param.x; currStep < centerOfField_v; currStep += step) {
			glm::vec3 pos = MathOperations::MovedAcrossNormalOfParametr_v_CloseTo(currStep, centerOfField_u, inter, r, glm::vec3{ 0, 1, 0 }) - glm::vec3{ 0, r + 0.0005, 0 };
			cd.result.push_back(pos);
			cd.UpdateMaxHeight(pos.y);
		}

		cd.result.push_back(inter->Parametrization(centerOfField_v, centerOfField_u));
		cd.UpdateMaxHeight(cd.result[cd.result.size() - 1].y);

		for (float currStep = centerOfField_v; currStep <= allIntersections[1].param.x; currStep += step) {
			glm::vec3 pos = MathOperations::MovedAcrossNormalOfParametr_v_CloseTo(currStep, centerOfField_u, inter, r, glm::vec3{ 0, 1, 0 }) - glm::vec3{ 0, r + 0.0005, 0 };
			cd.result.push_back(pos);
			cd.UpdateMaxHeight(pos.y);
		}
	}

	void FillGaps(std::vector<glm::vec2>& params, float gapLength, const float radiusProportion = 0.5f, const float additionalSampelingScalar = 6.0f)
	{
		float powGapLength = gapLength * gapLength;
		for (int i = params.size() - 2; i >= 0 ; i--) {
			glm::vec2 param_0 = params[i + 0];
			glm::vec2 param_1 = params[i + 1];

			float powDis = MathOperations::PowDistance(param_0, param_1);
			if (powDis > powGapLength) {
				float dis = sqrtf(powDis);
				float fillingNumber = ceilf(dis / gapLength) * additionalSampelingScalar;
				glm::vec2 disVector = param_1 - param_0;
				glm::vec2 halfDisVector = 0.5f * disVector;
				glm::vec2 paralerVector = -MathOperations::Rotation90Degree(halfDisVector);
				float r = dis * 0.5f;
				
				glm::vec2 directionVector = (param_1 - param_0) / fillingNumber;

				for (int j = 1; j < fillingNumber; j++) {
					float scalar = r * r - powf(static_cast<float>(j) * dis / fillingNumber - r, 2);
					scalar = sqrtf(scalar);
					scalar *= radiusProportion;
					params.insert(params.begin() + i + j, param_0 + directionVector * static_cast<float>(j) + paralerVector * scalar);
				}

				/*glm::vec2 directionVector = (param_1 - param_0) / fillingNumber;
				params.reserve(params.capacity() + fillingNumber * 2); 
				for (int j = 1; j < fillingNumber; j++) {
					params.insert(params.begin() + i + j, param_0 + directionVector * static_cast<float>(j));
				}*/
			}
		}
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

		float disFromBorder = radius * 0.20f;
		HoleInside(intersectionAble, cd, true, disBetweenPaths, disFromBorder);
		std::reverse(cd.result.begin(), cd.result.end());
		cd.result.push_back(TakeLastAndSet_y(cd.result, cd.needToRise));
		HoleInside(intersectionAble, cd, false, disBetweenPaths, disFromBorder);

		return cd.result;
	}

	void HoleInside(IntersectionAble* intersectionAble, ConstrainData& cd, bool towards_v, float disBetweenPaths, float disFromBorder) {


		int id = towards_v ? 0 : 1;
		int reversId = !towards_v ? 0 : 1;

		glm::vec2 field = towards_v ? cd.field_v : cd.field_u;

		float startSelectedDirection = MathOperations::MinDisFromCenter_Di(id, cd.params, field, 0, true);
		float endSelectedDirection = MathOperations::MaxDisFromCenter_Di(id, cd.params, field, 0, true);

		glm::vec2 start{ cd.field_v.x, cd.field_u.x };
		glm::vec2 end{ cd.field_v.y, cd.field_u.y };

		start[id] = end[id] = startSelectedDirection + disFromBorder;
		

		bool liftFrez = false;
		for (float startLine = startSelectedDirection + disFromBorder; startLine < endSelectedDirection - disFromBorder; startLine += disBetweenPaths) {


			start[id] = startLine;
			end[id] = startLine;

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

				float signe = start[reversId] < end[reversId] ? 1.0f : -1.0f;

				auto intPoint_0 = allIntersections[i + 0];
				auto intPoint_1 = allIntersections[i + 1];

				intPoint_0.param[reversId] += disFromBorder * signe;
				intPoint_1.param[reversId] -= disFromBorder * signe;

				if (liftFrez) {
					cd.result.push_back(TakeLastAndSet_y(cd.result, cd.needToRise));

					auto tmpPos = intersectionAble->Parametrization(intPoint_0.param.x, intPoint_0.param.y);
					tmpPos.y = cd.needToRise;
					cd.result.push_back(tmpPos);
				}

				cd.result.push_back(intersectionAble->Parametrization(intPoint_0.param.x, intPoint_0.param.y));
				cd.result.push_back(intersectionAble->Parametrization(intPoint_1.param.x, intPoint_1.param.y));

				liftFrez = true;
			}
			liftFrez = false;
			std::swap(start, end);
		}
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

	void GenerateObjectBorder(ConstrainData& cd, IntersectionAble *inter, float r) {
		cd.result.push_back(TakeLastAndSet_y(cd.result, cd.needToRise));
		int idOfLastFromProvirusPath = cd.result.size();
		cd.result.push_back({});

		for (auto& a : cd.params) {
			cd.result.push_back(inter->Parametrization(a.x, a.y - cd.offset) - glm::vec3{ 0, r, 0 });
		}

		cd.result[idOfLastFromProvirusPath] = cd.result[idOfLastFromProvirusPath + 1];
		cd.result[idOfLastFromProvirusPath].y = cd.needToRise;
	}

};