#pragma once 

namespace Armillary {
	namespace Alchemy {
		namespace Hooks {
			bool Install();

			class SelectedItemMonitor : public SingletonClass<SelectedItemMonitor> {
			public:
				bool InstallHook();
			private:
#ifdef SKYRIM_AE
				static bool MonitorItemSelection(const RE::CraftingSubMenus::AlchemyMenu& a_param1, const uint64_t& a_param2);
#endif

				inline static REL::Relocation<decltype(&MonitorItemSelection)> _originalCall;
			};
		}
	}
}