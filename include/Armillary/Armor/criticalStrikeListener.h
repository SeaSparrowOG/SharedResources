#pragma once

namespace Armillary::Armor::CriticalStrike {
	/*
	Listens for critical strikes, and if the critical strike dealer has the appropriate
	ARMOR perks, casts the appropriate buffs.
	*/
	class CriticalListener :
		public ISingleton<CriticalListener>,
		public RE::BSTEventSink<RE::CriticalHit::Event> {

	public:
		bool RegisterListener();
	private:
		RE::BSEventNotifyControl ProcessEvent(const RE::CriticalHit::Event* a_event, RE::BSTEventSource<RE::CriticalHit::Event>*) override;
		//These aren't the duelist perks, they are combat rush.
		RE::BGSPerk* duelistPerk{ nullptr };
		RE::SpellItem* duelistProc{ nullptr };
	};

	bool RegisterListener();
}