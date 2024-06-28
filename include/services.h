#pragma once
#include "Armillary/install.h"
#include "Common/papyrus.h"

namespace Services {

	bool InstallDataLoadedPatches()
	{
		_loggerDebug("_________________________________________________");
		_loggerDebug("Registering Papyrus Functions...");
		SKSE::GetPapyrusInterface()->Register(Papyrus::RegisterFunctions);
		_loggerDebug("Done.");

		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		bool success = true;
		if (dataHandler->LookupModByName("Armillary.esp"sv)) {
			if (!Armillary::InstallPatches()) success = false;
		}
		return success;
	}
}