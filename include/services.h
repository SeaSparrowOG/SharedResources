#pragma once
#include "Armillary/install.h"

namespace Services {

	bool InstallDataLoadedPatches()
	{
		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		bool success = true;
		if (dataHandler->LookupModByName("Armillary.esp"sv)) {
			if (!Armillary::InstallPatches()) success = false;
		}
		return success;
	}
}