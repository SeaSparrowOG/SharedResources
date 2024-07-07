#include "Armillary/OneHanded/hooks.h"

namespace Armillary::OneHanded::Hooks {
	bool CombatHit::PreloadForms()
	{
		return true;
	}

	bool CombatHit::Install()
	{
		if (!PreloadForms()) return false;
		REL::Relocation<std::uintptr_t> target{ REL::ID(38627), 0x1C6 };
		stl::write_thunk_call<CombatHit>(target.address());
		_loggerDebug("Wrote hook - Perk entry combat hit");
		return true;
	}

	void CombatHit::thunk(RE::HitData* a_hitData, RE::TESObjectREFR* a_attacker,
			              RE::TESObjectREFR* a_defender, RE::InventoryEntryData* a_weapon, 
						  bool a_leftHand)
	{

		return func(a_hitData, a_attacker, a_defender, a_weapon, a_leftHand);
	}
	bool Install() {
		CombatHit::Install();
		return true;
	}
}