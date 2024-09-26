#pragma once
#include "glm/glm.hpp"
#include "ShaderManadement/Texture.h"
#include "IntersectionResultInfo.h"
#include <map>

struct textureInfo {
	const std::vector<float>& data;
	float min;
	float max;
};

class IntersectionAble {
	Texture resultIntersection{ GL_TEXTURE_2D };
	std::vector<float> resultTextureData = std::vector<float>(256*256);
	std::map<GLuint, textureInfo> activeTextures;
public:
	const int N = 256;

	virtual glm::vec3 Parametrization	(float v, float u) = 0;
	virtual glm::vec3 Derivative_u(float v, float u) = 0;
	virtual glm::vec3 Derivative_uu(float v, float u) = 0;

	virtual glm::vec3 Derivative_v(float v, float u) = 0;
	virtual glm::vec3 Derivative_vv(float v, float u) = 0;

	virtual glm::vec3 Derivative_vu(float v, float u) = 0;
	virtual glm::vec3 Derivative_uv(float v, float u) = 0;
	virtual glm::vec2 Field_u() = 0;
	virtual glm::vec2 Field_v() = 0;
	virtual glm::bvec2 CanWrap() = 0;

	GLuint GetTextureId() {
		return resultIntersection.ID;
	}

	// show texture if value is between min and max
	void AddActiveTexture(IntersectionResultInfo* iInfo, float min, float max) {
		textureInfo textureInfo{ iInfo->GetResultData(), min, max };
		if (activeTextures.find(iInfo->TextureId()) == activeTextures.end())
		{
			IncludeDataInResult(textureInfo.data, min, max);
			activeTextures.insert({ iInfo->TextureId(), textureInfo });
		}
		else
		{
			activeTextures.erase(iInfo->TextureId()); 
			activeTextures.insert({ iInfo->TextureId(), textureInfo });
			RecrateTextureData();
		}
		RecrateTexture();
	}

	void RemoveActiveTexture(GLuint id) {
		if (activeTextures.find(id) == activeTextures.end())
			return;
		activeTextures.erase(id);
		RecrateTextureData();
		RecrateTexture();
	}

	IntersectionAble() {
		RecrateTextureData();
		RecrateTexture();
	}


private:
	void IncludeDataInResult(const std::vector<float>& dataToInclude, float min, float max) {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				if (resultTextureData[N * i + j] >= 1 &&
					(dataToInclude[N * i + j] < min || dataToInclude[N * i + j] > max))
				{
					resultTextureData[N * i + j] = 0;
				}
			}
		}
	}

	void RecrateTextureData() {
		std::fill(resultTextureData.begin(), resultTextureData.end(), 1);
		for (const auto& info : activeTextures) {
			IncludeDataInResult(info.second.data, info.second.min, info.second.max);
		}
	}

	void RecrateTexture() {
		resultIntersection.Recreat();
		resultIntersection.Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, N, N, 0, GL_RED, GL_FLOAT, resultTextureData.data());
		resultIntersection.Unbind();
	}
};
