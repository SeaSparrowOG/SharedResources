#include "Armillary/install.h"
#include "Armillary/Alchemy/hooks.h"
#include "Armillary/Armor/hooks.h"
#include "Armillary/Conjuration/boundWeaponPatcher.h"
#include "Armillary/Conjuration/reanimationSpellPatcher.h"

namespace Armillary {
	bool InstallPatches()
	{
		_loggerDebug("Preparing to Install Armillary.");
		_loggerDebug("_________________________________________________");
		_loggerDebug("Patching Armor:");
		_loggerDebug("_________________________________________________");
		Armor::Hooks::Install();
		_loggerDebug("_________________________________________________");
		_loggerDebug("Patching Alchemy:");
		_loggerDebug("_________________________________________________");
		if (!Alchemy::Hooks::Install()) return false;
		_loggerDebug("Patching Conjuration:");
		_loggerDebug("_________________________________________________");
		if (!Conjuration::BoundWeaponPatcher::PatchBoundWeapons()) return false;
		if (!Conjuration::ReanimationSpellPatcher::ReanimationSpellPatcher::GetSingleton()->PatchReanimationSpells()) return false;
		return true;
	}
}