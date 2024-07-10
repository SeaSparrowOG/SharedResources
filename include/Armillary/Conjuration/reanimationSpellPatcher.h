#pragma once

namespace Armillary::Conjuration::ReanimationSpellPatcher {
	/*
	Adds the appropriate secondary reanimation effects to all reanimation spells.
	*/
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