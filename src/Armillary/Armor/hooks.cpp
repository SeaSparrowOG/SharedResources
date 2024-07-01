#include "Armillary/Armor/hooks.h"

namespace Armillary::Armor::Hooks {
	void Character__CalculateResistance::Install()
	{
		func = REL::Relocation<uintptr_t>{ RE::VTABLE_PlayerCharacter[4] }.write_vfunc(0x0A, thunk);
		_loggerDebug("Wrote hook - Character");
	}

	float Character__CalculateResistance::thunk(RE::MagicTarget* a_target, RE::MagicItem* a_item, RE::Effect* a_effect, RE::TESBoundObject* a_param4)
	{
		return func(a_target, a_item, a_effect, a_param4);
	}

	void Install() {
		Character__CalculateResistance::Install();
	}
}