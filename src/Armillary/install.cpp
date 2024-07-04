#include "Armillary/install.h"
#include "Armillary/Alchemy/hooks.h"
#include "Armillary/Armor/hooks.h"
#include "Armillary/Armor/armorPatcher.h"
#include "Armillary/Armor/criticalStrikeListener.h"
#include "Armillary/Conjuration/boundWeaponPatcher.h"
#include "Armillary/Conjuration/reanimationSpellPatcher.h"

namespace Armillary {
	bool InstallPatches()
	{
		_loggerInfo("Preparing to Install Armillary.");
		_loggerInfo("_________________________________________________");
		_loggerInfo("Patching Armor:");
		_loggerInfo("_________________________________________________");
		_loggerInfo("    >Installing Armor Hooks...");
		if (!Armor::Hooks::Install()) return false;
		_loggerInfo("    >Adding new cuirass keywords...");
		if (!Armor::ArmorCuirassPatcher::PatchCuirasses()) return false;
		_loggerInfo("    >Registering critical strike listener...");
		if (!Armor::CriticalStrike::RegisterListener()) return false;
		_loggerInfo("_________________________________________________");
		_loggerInfo("Patching Alchemy:");
		_loggerInfo("_________________________________________________");
		_loggerInfo("    >Installing alchemy hooks...");
		if (!Alchemy::Hooks::Install()) return false;
		_loggerInfo("_________________________________________________");
		_loggerInfo("Patching Conjuration:");
		_loggerInfo("_________________________________________________");
		_loggerInfo("    >Adding new bound weapon keywords...");
		if (!Conjuration::BoundWeaponPatcher::PatchBoundWeapons()) return false;
		_loggerInfo("    >Adding new reanimation spell abilities...");
		if (!Conjuration::ReanimationSpellPatcher::ReanimationSpellPatcher::GetSingleton()->PatchReanimationSpells()) return false;
		return true;
	}
}