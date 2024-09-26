#pragma once
#include <vector>
#include<glad/glad.h>

class IntersectionResultInfo {
public:
	virtual const std::vector<float>& GetResultData() = 0;
	virtual GLuint TextureId() = 0;
};