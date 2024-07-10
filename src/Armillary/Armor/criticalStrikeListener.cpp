#include "Armillary/Armor/criticalStrikeListener.h"

namespace Armillary::Armor::CriticalStrike {
	inline RE::BGSPerk* GetPerkVariable(std::string_view a_varName,
		RE::BSTScrapHashMap<RE::BSFixedString, RE::BSScript::Variable>* a_map) {
		if (!a_map->contains(a_varName)) return nullptr;

		auto& var = a_map->find(a_varName)->second;
		if (var.IsNoneObject() || !var.IsObject()) return nullptr;
		auto* foundObject = var.Unpack<RE::BGSPerk*>();
		if (!foundObject) return nullptr;

		return foundObject;
	}

	inline RE::SpellItem* GetSpellVariable(std::string_view a_varName,
		RE::BSTScrapHashMap<RE::BSFixedString, RE::BSScript::Variable>* a_map) {
		if (!a_map->contains(a_varName)) return nullptr;

		auto& var = a_map->find(a_varName)->second;
		if (var.IsNoneObject() || !var.IsObject()) return nullptr;
		auto* foundObject = var.Unpack<RE::SpellItem*>();
		if (!foundObject) return nullptr;

		return foundObject;
	}

	bool RegisterListener()
	{
		return CriticalListener::GetSingleton()->RegisterListener();
	}

	bool CriticalListener::RegisterListener()
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

		duelistPerk = GetPerkVariable("ARM_Armor_PRK_070_CombatRush"sv, &properties);
		duelistProc = GetSpellVariable("ARM_Armor_SPL_CombatRushProc"sv, &properties);
		if (!(duelistPerk && duelistProc)) return false;

		_loggerDebug("Registering Critical Hit Listener...");
		RE::CriticalHit::GetEventSource()->AddEventSink<RE::CriticalHit::Event>(this);
		return true;
	}

	RE::BSEventNotifyControl CriticalListener::ProcessEvent(const RE::CriticalHit::Event* a_event, RE::BSTEventSource<RE::CriticalHit::Event>*)
	{
		if (!a_event) return RE::BSEventNotifyControl::kContinue;

		auto* eventAggressor = a_event->aggressor;
		auto* eventActor = eventAggressor ? eventAggressor->As<RE::Actor>() : nullptr;
		if (!eventActor) return RE::BSEventNotifyControl::kContinue;
		if (!eventActor->HasPerk(duelistPerk)) return RE::BSEventNotifyControl::kContinue;

		auto* wornArmor = eventActor->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kBody);
		if (!wornArmor) return RE::BSEventNotifyControl::kContinue;
		if (!wornArmor->HasKeywordString("ARM_Armor_KWD_ArmorTypeLightCuirass"sv)) return RE::BSEventNotifyControl::kContinue;

		auto* eventMagicCaster = eventActor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
		if (!eventMagicCaster) return RE::BSEventNotifyControl::kContinue;

		eventMagicCaster->CastSpellImmediate(duelistProc, false, eventActor, 1.0f, false, 0.0f, eventActor);
		return RE::BSEventNotifyControl::kContinue;
	}
}