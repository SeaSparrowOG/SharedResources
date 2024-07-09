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
		inline static RE::BGSPerk* maceRank2Perk{ nullptr };
		inline static RE::BGSPerk* gladiatorPerk{ nullptr };
		inline static RE::BGSPerk* opportunistPerk{ nullptr };
		inline static RE::BGSPerk* wardancerPerk{ nullptr };

		inline static RE::SpellItem* duelistProc{ nullptr };
		inline static RE::SpellItem* duelistMark{ nullptr };
		inline static RE::SpellItem* duelistCooldown{ nullptr };
		inline static RE::SpellItem* maceProc{ nullptr };
		inline static RE::SpellItem* opportunistProc{ nullptr };
		inline static RE::SpellItem* wardancerProc{ nullptr };

		inline static RE::EffectSetting* duelistCountdown{ nullptr };
		inline static RE::EffectSetting* markCountdown{ nullptr };
	};
}