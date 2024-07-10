#include "Armillary/OneHanded/events.h"

namespace Armillary::OneHanded::Events {
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

	bool LoadUnloadEventListener::RegisterListener()
	{
		auto* eventHolder = RE::ScriptEventSourceHolder::GetSingleton();
		if (!eventHolder) return false; 
		
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

		auto* foundProc = GetSpellVariable("ARM_OneHanded_SPL_DuelistProc"sv, &properties);
		auto* foundCountdown = GetSpellVariable("ARM_OneHanded_SPL_DuelistProcCountdown"sv, &properties);
		auto* foundPerk = GetPerkVariable("ARM_OneHanded_PRK_020_Duelist"sv, &properties);

		if (!CachedActorRegister::GetSingleton()->RegisterAbilities(foundPerk, foundProc, foundCountdown)) return false;
		eventHolder->AddEventSink(this);
		return true;
	}

	RE::BSEventNotifyControl LoadUnloadEventListener::ProcessEvent(const RE::TESObjectLoadedEvent* a_event, RE::BSTEventSource<RE::TESObjectLoadedEvent>*)
	{
		if (!a_event) return continueEvent;
		auto* eventForm = RE::TESForm::LookupByID(a_event->formID);
		auto* eventActor = eventForm ? eventForm->As<RE::Actor>() : nullptr;
		if (!eventActor) return continueEvent;

		auto* actorSingleton = CachedActorRegister::GetSingleton();
		if (a_event->loaded) {
			if (actorSingleton->IsActorManaged(eventActor)) return continueEvent;
			actorSingleton->ManageActor(eventActor);
		}
		else {
			if (!actorSingleton->IsActorManaged(eventActor)) return continueEvent;
			actorSingleton->UnManageActor(eventActor);
		}
		return continueEvent;
	}

	bool CachedActorRegister::UnManageActor(RE::Actor* a_actor)
	{
		this->_actorCache.erase(a_actor);
		a_actor->RemoveAnimationGraphEventSink(this);
		return true;
	}

	bool CachedActorRegister::ManageActor(RE::Actor* a_actor)
	{
		this->_actorCache.try_emplace(a_actor, true);
		a_actor->AddAnimationGraphEventSink(this);
		return true;
	}

	bool CachedActorRegister::IsActorManaged(RE::Actor* a_actor)
	{
		return this->_actorCache.contains(a_actor);
	}

	bool CachedActorRegister::RegisterAbilities(RE::BGSPerk* a_duelistPerk, RE::SpellItem* a_duelistProc, RE::SpellItem* a_duelistCountdown)
	{
		if (!(a_duelistProc && a_duelistCountdown && a_duelistPerk)) return false;

		duelistPerk = a_duelistPerk;
		duelistProc = a_duelistProc;
		duelistCountdown = a_duelistCountdown;
		return true;
	}

	RE::BSEventNotifyControl CachedActorRegister::ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>*)
	{
		if (!a_event) return continueEvent;
		if (a_event->tag != "blockStartOut"sv) return continueEvent;

		auto eventFormID = a_event->holder->formID;
		auto* eventForm = RE::TESForm::LookupByID(eventFormID);
		auto* eventActor = eventForm ? eventForm->As<RE::Actor>() : nullptr;
		if (!eventActor) return continueEvent;
		if (eventActor->HasSpell(duelistCountdown)) return continueEvent;
		if (!eventActor->HasPerk(duelistPerk)) return continueEvent;

		auto* leftData = eventActor->GetEquippedEntryData(true);
		auto* leftObject = leftData ? leftData->object : nullptr;
		auto* leftShield = leftObject ? leftObject->As<RE::TESObjectARMO>() : nullptr;
		bool hasShield = leftShield ? leftShield->IsShield() : false;
		if (hasShield) return continueEvent;

		auto* eventMagicCaster = eventActor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
		if (!eventMagicCaster) return continueEvent;

		eventMagicCaster->CastSpellImmediate(duelistProc, false, eventActor, 1.0f, false, 0.0f, eventActor);
		eventMagicCaster->CastSpellImmediate(duelistCountdown, false, eventActor, 1.0f, false, 0.0f, eventActor);
		return continueEvent;
	}

	bool Install()
	{
		return LoadUnloadEventListener::GetSingleton()->RegisterListener();
	}
}