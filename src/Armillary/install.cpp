#include "Armillary/install.h"
#include "Armillary/Alchemy/hooks.h"
#include "Armillary/Papyrus/papyrus.h"

namespace Armillary {
	bool InstallPatches()
	{
		Alchemy::Hooks::Install();
		SKSE::GetPapyrusInterface()->Register(Papyrus::RegisterFunctions);
		return true;
	}
}