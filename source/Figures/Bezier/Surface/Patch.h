#pragma once
#include <cstdint>
#include "IPatch.h"

struct Patch: public IPatch
{
	uint16_t GetId(int x, int y) override { return controlPoints[y * 4 + x]; }
	void SetId(int x, int y, int value) override { controlPoints[y * 4 + x] = value; }
private:
	uint32_t controlPoints[16];
}; 