#pragma once

namespace Armillary::Armor::CriticalStrike {
	class CriticalListener :
		public ISingleton<CriticalListener>,
		public RE::BSTEventSink<RE::CriticalHit::Event> {

	public:
		bool RegisterListener();
	private:
		RE::BSEventNotifyControl ProcessEvent(const RE::CriticalHit::Event* a_event, RE::BSTEventSource<RE::CriticalHit::Event>*) override;
		RE::BGSPerk* duelistPerk{ nullptr };
		RE::SpellItem* duelistProc{ nullptr };
	};

	bool RegisterListener();
}