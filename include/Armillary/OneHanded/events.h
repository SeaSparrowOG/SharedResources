#pragma once

namespace Armillary::OneHanded::Events {
#define continueEvent RE::BSEventNotifyControl::kContinue
	/*
	Main workhorse. Listens for the block event, and casts the duelist proc and cooldown
	spells on the actor that blocked if they have the duelist perk and no shields equipped.
	These spells are used by the One-Handed hooks to determine if a parry is performed.
	*/
	class CachedActorRegister : public SingletonClass<CachedActorRegister>,
		public RE::BSTEventSink<RE::BSAnimationGraphEvent> {
	public:
		bool IsActorManaged(RE::Actor* a_actor);
		bool RegisterAbilities(RE::BGSPerk* a_duelistPerk, RE::SpellItem* a_duelistProc, RE::SpellItem* a_duelistCountdown);
		bool ManageActor(RE::Actor* a_actor);
		bool UnManageActor(RE::Actor* a_actor);
	private:
		RE::BSEventNotifyControl ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>*) override;

		std::unordered_map<RE::Actor*, bool> _actorCache;
		RE::BGSPerk* duelistPerk{ nullptr };
		RE::SpellItem* duelistProc{ nullptr };
		RE::SpellItem* duelistCountdown{ nullptr };
	};

	/*
	Adds and removes actors as necessary from the CachedActorRegister, based on them being loaded or unloaded.
	*/
	class LoadUnloadEventListener : public SingletonClass<LoadUnloadEventListener>,
		public RE::BSTEventSink<RE::TESObjectLoadedEvent> {
	public:
		bool RegisterListener();
	private:
		RE::BSEventNotifyControl ProcessEvent(const RE::TESObjectLoadedEvent* a_event, RE::BSTEventSource<RE::TESObjectLoadedEvent>*) override;
	};

	bool Install();
}