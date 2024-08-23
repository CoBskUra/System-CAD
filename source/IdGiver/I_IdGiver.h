#pragma once
#include <cstdint>

class I_IdGiver {
public:
	virtual void SetFirstFreeId() = 0;
	virtual void FreeId() = 0;
	virtual bool SetId(uint32_t id) = 0;
	virtual uint32_t GetId() = 0;
};