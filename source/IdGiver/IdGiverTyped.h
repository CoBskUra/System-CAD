//#pragma once
//#include "I_IdGiver.h"
//#include <unordered_map>
//#include <string>
//
//class IdGiverTyped : public I_IdGiver {
//public:
//	IdGiverTyped(std::string type);
//	void SetFirstFreeId() override;
//	void FreeId() override;
//	bool SetId(uint32_t id) override;
//	uint32_t GetId() override;
//private:
//	I_IdGiver IdGiver = IdGiver;
//
//	std::string type;
//	static std::unordered_map<std::string type, uint32_t s_firstFreeId> typed_s_firstFreeId;
//	static std::unordered_map< std::string type, std::set<uint32_t> s_takenIds> typed_s_takenIds;
//	static std::unordered_map < std::string type, std::stack<uint32_t> s_freedIds> typed_s_freedIds;
//};