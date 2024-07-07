#pragma once

namespace Armillary::OneHanded::Hooks {
	bool Install();

	//Hook called when any actor hits another actor.
	struct CombatHit
	{
		static bool   Install();
		static void   thunk(RE::HitData* a_hitData, RE::TESObjectREFR* a_attacker, RE::TESObjectREFR* a_defender, RE::InventoryEntryData* a_weapon, bool a_left);
		static bool   PreloadForms();

		inline static REL::Relocation<decltype(thunk)> func;
	};
}