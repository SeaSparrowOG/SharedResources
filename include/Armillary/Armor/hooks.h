#pragma once

namespace Armillary::Armor::Hooks {
	//Triggers when the player's resistances would be calculated
	struct Character__CalculateResistance
	{
		static bool   Install();
		static float  thunk(RE::MagicTarget* a_target, RE::MagicItem* a_item, RE::Effect* a_effect, RE::TESBoundObject* a_param4);
		static bool   PreloadForms();

		inline static REL::Relocation<decltype(thunk)> func;
		inline static RE::BGSPerk* UnbreakablePerk{ nullptr };
	};

	//Triggers when the player's skill would be advanced (player.advskill, regular skill use, papyrus...)
	struct Character__UseSkill
	{
		static bool   Install();
		static float  thunk(RE::PlayerCharacter* a_char, RE::ActorValue a_skill, float a_ammount,
			RE::TESForm* a_advanceObject, uint32_t a_advanceAction, 
			bool a_useModifiers, bool a_hideNotification);

		inline static REL::Relocation<decltype(thunk)> func;
	};

	bool Install();
}