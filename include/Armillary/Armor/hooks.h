#pragma once

namespace Armillary::Armor::Hooks {
	//Triggers when the player's resistances would be calculated
	struct Character__CalculateResistance
	{
		static void   Install();
		static float  thunk(RE::MagicTarget* a_target, RE::MagicItem* a_item, RE::Effect* a_effect, RE::TESBoundObject* a_param4);

		inline static REL::Relocation<decltype(thunk)> func;
	};

	void Install();
}