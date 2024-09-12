#pragma once
#include "Figures/Bezier/BezierCurve.h"
#include "Figures/Bezier/Surface/BezierSurface.h"

static class TransformSurfaceUsingCurve {
	static glm::vec3 ClosetNormal(glm::vec3 lastNormal, glm::vec3 newNormal, glm::vec3 lastPos, glm::vec3 newPos, glm::vec3 axis) {
		glm::vec3 theCloset = newNormal;
		for (int i = 0; i < 100; i++) {
			float rad = i * M_PI * 2.0f / (float)100;
			glm::vec3 tmpNorm = MathOperations::RotationAlongAxis(newNormal, rad, axis);
			if (glm::distance(lastNormal + lastPos, tmpNorm + newPos) < glm::distance(lastNormal + lastPos, theCloset + newPos)) {
				theCloset = tmpNorm;
			}
		}

		return theCloset;
	}

	static glm::vec3 CalculateTangent(glm::vec3 vec) {
		glm::vec3 tang = glm::normalize(glm::cross(vec, glm::vec3{ 1, 0, 0 }));
		if (isnan(tang.x) || isnan(tang.y) || isnan(tang.z))
			tang = glm::normalize(glm::cross(vec, glm::vec3{ 0, 1, 0 }));
		return tang;
	}
public:
	static void Transform(BezierSurface* surf, BezierCurve* curve, float r) {
		auto size = surf->SurfaceSize();
		float maxVerticalPoints = size.x * 4 - 1;
		float maxHorizontalPoints = size.y * 4 - 1;

		glm::vec3 lastNorm = CalculateTangent(glm::normalize(curve->Derivative(1)));
		glm::vec3 lastPos = curve->PositionOnCurve(1);

		for (int verticalId = size.x - 1; verticalId >= 0; verticalId--) {
			for (int k1 = 3; k1 >= 0; k1--) {
				float t = static_cast<float>(verticalId * 4 + k1) / maxVerticalPoints;
				glm::vec3 pos = curve->PositionOnCurve(t);
				glm::vec3 der = curve->Derivative(t);
				der = glm::normalize(der);
				glm::vec3 normal = CalculateTangent(der);
				normal = ClosetNormal(lastNorm, normal, lastPos, pos, der);
				lastNorm = normal;
				lastPos = pos;

				std::set<Figure*> uniquePoints;
				std::vector<Figure*> uniquePointsVectors;
				for (int horizontalId = size.y - 1; horizontalId >= 0; horizontalId--) {
					for (int k2 = 3; k2 >= 0; k2--) {
						Figure* point = surf->TakePoint(verticalId, horizontalId, k1, k2);
						if (!uniquePoints.contains(point))
						{
							uniquePoints.insert(point);
							uniquePointsVectors.push_back(point);
						}
					}
				}
				for (int i = 0; i < uniquePointsVectors.size(); i++) {
					float rotationRad = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(uniquePointsVectors.size());
					Figure* point = uniquePointsVectors[i];
					point->transpose->SetObjectPosition(pos + normal * r);
					point->RotationAlong(der, pos, rotationRad);
				}
			}
		}
	}
};