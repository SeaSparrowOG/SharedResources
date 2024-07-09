#pragma once

namespace Armillary::OneHanded::Hooks {
	bool Install();

	//Hook called when any actor hits another actor.
	struct CombatHit
	{
		static bool   Install();
		static void   thunk(RE::Actor* target, RE::HitData* hitData);
		static bool   PreloadForms();

		inline static REL::Relocation<decltype(thunk)> func;
		inline static RE::BGSPerk* duelistPerk{ nullptr };
		inline static RE::BGSPerk* gladiatorPerk{ nullptr };
		inline static RE::BGSPerk* opportunistPerk{ nullptr };

		inline static RE::SpellItem* duelistProc{ nullptr };
		inline static RE::SpellItem* gladiatorProc{ nullptr };
		inline static RE::SpellItem* opportunistProc{ nullptr };
	};
}