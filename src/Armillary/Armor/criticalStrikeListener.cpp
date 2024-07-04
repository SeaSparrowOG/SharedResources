#include "Armillary/Armor/criticalStrikeListener.h"
#include "Common/utilityFuncs.h"

namespace Armillary::Armor::CriticalStrike {

	bool RegisterListener()
	{
		return CriticalListener::GetSingleton()->RegisterListener();
	}

	bool CriticalListener::RegisterListener()
	{
		auto* foundPerk = UtilityFunctions::GetFormFromQuestScript<RE::BGSPerk>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_Armor_PRK_070_CombatRush");
		if (!foundPerk) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundPerk));
		duelistPerk = foundPerk;

		auto* foundSpell = UtilityFunctions::GetFormFromQuestScript<RE::SpellItem>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_Armor_SPL_CombatRushProc");
		if (!foundSpell) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundSpell));
		duelistProc = foundSpell;

		if (!duelistPerk) return false;
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