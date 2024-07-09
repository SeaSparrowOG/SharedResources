#include "Armillary/Conjuration/boundWeaponPatcher.h"

namespace Armillary::Conjuration::BoundWeaponPatcher {
	bool PatchBoundWeapons()
	{
		_loggerDebug("Adding bound weapon keyword to bound weapons...");
		_loggerDebug("    Fetching data handler...");
		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) return false;
		_loggerDebug("    Fetching bound weapon keyword...");
		auto* boundWeaponKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("ARM_Conjuration_KWD_WeapTypeBound"sv);
		if (!boundWeaponKeyword) return false;
		_loggerDebug("    Fetching weapon array...");
		auto& weaponArray = dataHandler->GetFormArray<RE::TESObjectWEAP>();
		if (weaponArray.empty()) return false;

		std::size_t patchedWeapons = 0;
		for (auto* weapon : weaponArray) {
			if (!weapon->IsBound()) continue;
			weapon->AddKeyword(boundWeaponKeyword);
			patchedWeapons++;
		}
		_loggerDebug("    Patched {} bound weapons.", patchedWeapons);
		_loggerDebug("_________________________________________________");
		return true;
	}
}