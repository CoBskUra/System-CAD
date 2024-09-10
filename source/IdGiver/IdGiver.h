#pragma once
#include "I_IdGiver.h"
#include <set>
#include <stack>

class IdGiver : public I_IdGiver {
public:
	void SetFirstFreeId() override;
	void FreeId() override;
	bool SetId(uint32_t id) override;
	uint32_t GetId() override;
private:
	uint32_t id;
	static uint32_t s_firstFreeId;
	static std::set<uint32_t> s_takenIds;
	static std::stack<uint32_t> s_freedIds;
};