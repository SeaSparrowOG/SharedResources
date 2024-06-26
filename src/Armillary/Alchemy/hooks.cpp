#include "Armillary/Alchemy/hooks.h"

namespace Armillary {
	bool Alchemy::Hooks::SelectedItemMonitor::InstallHook() {
        REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(0, 51343), OFFSET(0x0, 0x2A) };
        SKSE::AllocTrampoline(14);
        auto& trampoline = SKSE::GetTrampoline();
        _originalCall = trampoline.write_call<5>(target.address(), &MonitorItemSelection);
        return true;
	}

    bool Alchemy::Hooks::SelectedItemMonitor::MonitorItemSelection(const RE::CraftingSubMenus::AlchemyMenu& a_param1, const uint64_t& a_param2) {
        auto result = _originalCall(a_param1, a_param2);

        return result;
    }
}

bool Armillary::Alchemy::Hooks::Install() {
    Armillary::Alchemy::Hooks::SelectedItemMonitor::GetSingleton()->InstallHook();
    return true;
}
