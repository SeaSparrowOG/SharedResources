#include "Armillary/Armor/armorPatcher.h"

namespace Armillary::Armor::ArmorCuirassPatcher {
	bool PatchCuirasses()
	{
		_loggerDebug("Adding cuirass keywords to heavy/light cuirasses...");
		_loggerDebug("    Fetching data handler...");
		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) return false;

		_loggerDebug("    Fetching keywords...");
		auto* lightArmorKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("ARM_Armor_KWD_ArmorTypeLightCuirass"sv);
		auto* heavyArmorKeyword = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("ARM_Armor_KWD_ArmorTypeHeavyCuirass"sv);
		if (!lightArmorKeyword || !heavyArmorKeyword) return false;

		_loggerDebug("    Fetching armor array...");
		auto& armorArray = dataHandler->GetFormArray<RE::TESObjectARMO>();
		if (armorArray.empty()) return false;

		std::size_t patchedWeapons = 0;
		for (auto* armor : armorArray) {
			if (!armor->GetPlayable()) continue;
			if (!armor->HasKeywordString("ArmorCuirass")) continue;

			if (armor->HasKeywordString("ArmorLight"sv)) {
				armor->AddKeyword(lightArmorKeyword);
				patchedWeapons++;
			}
			else if (armor->HasKeywordString("ArmorHeavy"sv)) {
				armor->AddKeyword(heavyArmorKeyword);
				patchedWeapons++;
			}
		}
		_loggerDebug("    Patched {} armors.", patchedWeapons);
		_loggerDebug("_________________________________________________");
		return true;
		return true;
	}
}