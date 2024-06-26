#include "Armillary/Papyrus/papyrus.h"

namespace Armillary::Papyrus {
	std::vector<int> GetVersion(STATIC_ARGS) 
	{
		std::vector<int> response{};
		response.push_back(Version::MAJOR);
		response.push_back(Version::MINOR);
		response.push_back(Version::PATCH);

		return response;
	}

	void Bind(VM& a_vm) 
	{
		BIND(GetVersion);
	}

	bool RegisterFunctions(VM* a_vm)
	{
		Bind(*a_vm);
		return true;
	}
}