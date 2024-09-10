#include "IdGiver.h"


uint32_t IdGiver::s_firstFreeId = 0;
std::set<uint32_t> IdGiver::s_takenIds = {};
std::stack<uint32_t> IdGiver::s_freedIds = {};

uint32_t IdGiver::GetId()
{
	return id;
}

bool IdGiver::SetId(uint32_t id)
{
	if (id == this->id)
	{
		return true;
	}

	auto searchResult = s_takenIds.find(id);

	if (searchResult != s_takenIds.end())
	{
		return false;
	}

	s_takenIds.insert(id);
	s_freedIds.push(this->id);

	this->id = id;

	return true;
}

void IdGiver::SetFirstFreeId()
{
	while (s_takenIds.find(s_firstFreeId) != s_takenIds.end())
	{
		s_firstFreeId++;
	}

	if (s_freedIds.empty())
	{
		id = s_firstFreeId++;
	}
	else
	{
		id = s_freedIds.top();
		s_freedIds.pop();
	}

	s_takenIds.insert(id);
}

void IdGiver::FreeId() {
	s_takenIds.erase(id);
	s_freedIds.push(id);
	if (id < s_firstFreeId)
		s_firstFreeId = id;
}