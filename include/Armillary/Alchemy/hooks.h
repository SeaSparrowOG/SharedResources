#pragma once 

namespace Armillary::Alchemy::Hooks {
	bool Install();

	/*
	Hook for detecting what ingredients are currently selected in the alchemy menu.
	*/
	class SelectedItemMonitor : public SingletonClass<SelectedItemMonitor> {
	public:
		bool PreloadForms();
		bool InstallHook();
		float GetEfficiencyBoostModifier();

	private:
		static bool MonitorItemSelection(const RE::CraftingSubMenus::AlchemyMenu& a_param1, const uint64_t& a_param2);

		inline static REL::Relocation<decltype(&MonitorItemSelection)> _originalCall;

		//Members

		//Efficiency bonus: Used by the quality ingredients perk. It is the % by which to boose
		//crafted potions, based on ingredients.
		float            efficiencyBonus{ 1.0f };

		//Quality Ingredients perk: If the player has this perk, proceed with the calculation.
		RE::BGSPerk*     qualityIngredientsPerk{ nullptr };

		//The following lists each contain the ingredients corresponding to their level name.
		//Filled in the plugin.
		RE::BGSListForm* uncommonIngredients{};
		RE::BGSListForm* rareIngredients{};
		RE::BGSListForm* uniqueIngredients{};
	};

	class CreatedItemMonitoring : public SingletonClass<CreatedItemMonitoring> {
	public:
		bool PreloadForms();
		bool InstallHook();

	private:
		static void ItemCreationMonitor(RE::TESDataHandler* a_dataHandler, RE::AlchemyItem* a_alchemyItem);

		inline static REL::Relocation<decltype(&ItemCreationMonitor)> _originalCall;

		//Members
		//Perk that controls the addition of slow death. If the player has that perk, add
		//the slow death magic effect and modify its duration.
		RE::BGSPerk*       slowDeathPerk{ nullptr };

		//Perk that controls the addition of stimulants. If the player has that perk, add
		//the stimulants magic effect and modify its duration.
		RE::BGSPerk*       stimulantsPerk{ nullptr };

		//The following 2 are the effects used by stimulants and slow death.
		RE::EffectSetting* slowDeathEffect{ nullptr };
		RE::EffectSetting* stimulantsEffectHealth{ nullptr };
		RE::EffectSetting* stimulantsEffectStamina{ nullptr };
		RE::EffectSetting* stimulantsEffectMagicka{ nullptr };
	};
}