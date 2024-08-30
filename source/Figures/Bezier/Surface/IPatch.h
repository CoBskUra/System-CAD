#pragma once
#include <cstdint>

struct IPatch
{
	virtual uint16_t GetId(int x, int y)  = 0;
	virtual void SetId(int x, int y, int value) = 0;

};