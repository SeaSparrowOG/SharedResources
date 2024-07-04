#include "Armillary/Armor/hooks.h"
#include "Common/utilityFuncs.h"

namespace Armillary::Armor::Hooks {
	bool Character__CalculateResistance::PreloadForms()
	{
		auto* foundPerk = UtilityFunctions::GetFormFromQuestScript<RE::BGSPerk>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_Armor_PRK_100_Unbreakable");
		if (!foundPerk) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundPerk));
		UnbreakablePerk = foundPerk;
		return true;
	}

	bool Character__CalculateResistance::Install()
	{
		_loggerDebug("Preloading forms for Resistance Calculation...");
		if (!PreloadForms()) return false;
		func = REL::Relocation<uintptr_t>{ RE::VTABLE_PlayerCharacter[4] }.write_vfunc(0x0A, thunk);
		_loggerDebug("Wrote hook - Character: Calculate Resistance");
		return true;
	}

	float Character__CalculateResistance::thunk(RE::MagicTarget* a_target, RE::MagicItem* a_item, RE::Effect* a_effect, RE::TESBoundObject* a_param4)
	{
		float response = func(a_target, a_item, a_effect, a_param4);
		if (const auto target = a_target->GetTargetAsActor()) {
			if (target->HasPerk(UnbreakablePerk)) {
				if (!a_effect->baseEffect) return response;
				auto* wornArmor = target->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kBody);
				if (!wornArmor) return response;
				if (!wornArmor->HasKeywordString("ARM_Armor_KWD_ArmorTypeHeavyCuirass"sv)) return response;

				auto resistValue = a_effect->baseEffect->data.resistVariable;
				if (resistValue != RE::ActorValue::kResistFire
					&& resistValue != RE::ActorValue::kResistFrost
					&& resistValue != RE::ActorValue::kResistShock) {
					return response;
				}
				
				float charResistValue = target->GetActorValue(resistValue);
				charResistValue *= 1.0f + target->GetActorValue(RE::ActorValue::kResistMagic) / 100.0f;
				if (charResistValue / 100.0f > 1.0f - response) {
					if (charResistValue >= 100.0) return 0.00f;
					return 1.0f - charResistValue / 100.0f;
				}
			}
		}
		return response;
	}


	bool Character__UseSkill::Install()
	{
		REL::Relocation<std::uintptr_t> target{ REL::ID(40488), 0x25};
		stl::write_thunk_call<Character__UseSkill>(target.address());
		_loggerDebug("Wrote hook - Character: Use Skill");
		return true;
	}

	float Character__UseSkill::thunk(RE::PlayerCharacter* a_char, RE::ActorValue a_skill, float a_ammount, RE::TESForm* a_advanceObject, 
		uint32_t a_advanceAction, bool a_useModifiers, bool a_hideNotification)
	{
		switch (a_skill) {
		case RE::ActorValue::kLightArmor:
			a_skill = RE::ActorValue::kHeavyArmor;
			break;
		default:
			break;
		}

		return func(a_char, a_skill, a_ammount, a_advanceObject, a_advanceAction, 
			a_useModifiers, a_hideNotification);
	}

	bool Install() {
		return Character__UseSkill::Install() && Character__CalculateResistance::Install();
	}
}