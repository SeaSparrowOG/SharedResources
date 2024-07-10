#include "Armillary/Armor/hooks.h"

namespace Armillary::Armor::Hooks {
	inline RE::BGSPerk* GetPerkVariable(std::string_view a_varName,
		RE::BSTScrapHashMap<RE::BSFixedString, RE::BSScript::Variable>* a_map) {
		if (!a_map->contains(a_varName)) return nullptr;

		auto& var = a_map->find(a_varName)->second;
		if (var.IsNoneObject() || !var.IsObject()) return nullptr;
		auto* foundObject = var.Unpack<RE::BGSPerk*>();
		if (!foundObject) return nullptr;

		return foundObject;
	}

	bool Character__CalculateResistance::PreloadForms()
	{
		const auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
		if (!vm) return false;

		const auto bindPolicy = vm->GetObjectBindPolicy();
		const auto handlePolicy = vm->GetObjectHandlePolicy();

		if (!bindPolicy || !handlePolicy) return false;

		const auto quest = RE::TESForm::LookupByEditorID<RE::TESQuest>("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv);
		const auto handle = handlePolicy->GetHandleForObject(RE::TESQuest::FORMTYPE, quest);

		RE::BSTScrapHashMap<RE::BSFixedString, RE::BSScript::Variable> properties;
		std::uint32_t nonConverted;
		bindPolicy->GetInitialPropertyValues(handle, "ARM_ObjectHolder"sv, properties, nonConverted);

		UnbreakablePerk = GetPerkVariable("ARM_Armor_PRK_100_Unbreakable"sv, &properties);
		if (!UnbreakablePerk) return false;
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