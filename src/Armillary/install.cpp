#include "Armillary/install.h"
#include "Armillary/Alchemy/hooks.h"

namespace Armillary {
	bool InstallPatches()
	{
		Alchemy::Hooks::Install();
		return true;
	}
}