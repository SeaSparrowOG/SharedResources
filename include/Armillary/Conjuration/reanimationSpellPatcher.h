#pragma once

namespace Armillary::Conjuration::ReanimationSpellPatcher {
	class ReanimationSpellPatcher : public SingletonClass<ReanimationSpellPatcher> {
	public:
		bool PatchReanimationSpells();
	private:
		bool PreloadForms();
		RE::EffectSetting* secondaryReanimationFFAimed{ nullptr };
		RE::EffectSetting* secondaryReanimationFFTargetActor{ nullptr };
		RE::EffectSetting* secondaryReanimationFFContact{ nullptr };
	};
}