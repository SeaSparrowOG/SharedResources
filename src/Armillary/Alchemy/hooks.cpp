#include "Armillary/Alchemy/hooks.h"

namespace Armillary {
    inline RE::BGSPerk* GetPerkVariable(std::string_view a_varName,
        RE::BSTScrapHashMap<RE::BSFixedString, RE::BSScript::Variable>* a_map) {
        if (!a_map->contains(a_varName)) return nullptr;

        auto& var = a_map->find(a_varName)->second;
        if (var.IsNoneObject() || !var.IsObject()) return nullptr;
        auto* foundObject = var.Unpack<RE::BGSPerk*>();
        if (!foundObject) return nullptr;

        return foundObject;
    }

    inline RE::BGSListForm* GetListVariable(std::string_view a_varName,
        RE::BSTScrapHashMap<RE::BSFixedString, RE::BSScript::Variable>* a_map) {
        if (!a_map->contains(a_varName)) return nullptr;

        auto& var = a_map->find(a_varName)->second;
        if (var.IsNoneObject() || !var.IsObject()) return nullptr;
        auto* foundObject = var.Unpack<RE::BGSListForm*>();
        if (!foundObject) return nullptr;

        return foundObject;
    }

    inline RE::EffectSetting* GetEffectVariable(std::string_view a_varName,
        RE::BSTScrapHashMap<RE::BSFixedString, RE::BSScript::Variable>* a_map) {
        if (!a_map->contains(a_varName)) return nullptr;

        auto& var = a_map->find(a_varName)->second;
        if (var.IsNoneObject() || !var.IsObject()) return nullptr;
        auto* foundObject = var.Unpack<RE::EffectSetting*>();
        if (!foundObject) return nullptr;

        return foundObject;
    }

	bool Alchemy::Hooks::SelectedItemMonitor::InstallHook() 
    {
        _loggerDebug("ARM: Installing Selected Item Monitoring hook...");
        if (!this->PreloadForms()) return false;

        REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(0, 51343), OFFSET(0x0, 0x2A) };
        SKSE::AllocTrampoline(14);
        auto& trampoline = SKSE::GetTrampoline();
        _originalCall = trampoline.write_call<5>(target.address(), &MonitorItemSelection);
        return true;
	}

    bool Alchemy::Hooks::SelectedItemMonitor::MonitorItemSelection(const RE::CraftingSubMenus::AlchemyMenu& a_param1, const uint64_t& a_param2) 
    {
        auto result = _originalCall(a_param1, a_param2);
        auto* associatedPerk = GetSingleton()->qualityIngredientsPerk;
        if (!associatedPerk) return result;
        if (!RE::PlayerCharacter::GetSingleton()->HasPerk(associatedPerk)) return result;

        auto& ingredients = a_param1.potionCreationData.ingredientEntries;
        float fNewValue = 1.0f;
        auto* singleton = GetSingleton();
        auto* uncommonMap = singleton->uncommonIngredients;
        auto* rareMap = singleton->rareIngredients;
        auto* uniqueMap = singleton->uniqueIngredients;

        bool containsUncommon = false;
        bool containsRare = false;
        bool containsUnique = false;

        for (auto* entry = ingredients->begin(); entry != ingredients->end(); ++entry) {
            if (!entry->isSelected) continue;
            auto* ingredient = entry->ingredient->object;
            if (!ingredient) continue;

            if (!containsUncommon && uncommonMap->HasForm(ingredient)) {
                containsUncommon = true;
            }
            else if (!containsRare && rareMap->HasForm(ingredient)) {
                containsRare = true;
            }
            else if (!containsUnique && uniqueMap->HasForm(ingredient)) {
                containsUnique = true;
            }
        }

        if (containsUncommon) fNewValue += 0.1f;
        if (containsRare) fNewValue += 0.25f;
        if (containsUnique) fNewValue += 0.5f;

        singleton->efficiencyBonus = fNewValue;
        return result;
    }

    bool Alchemy::Hooks::SelectedItemMonitor::PreloadForms()
    {
        _loggerDebug("ARM: Selected Item Monitor: Looking up forms..."); 

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

        uncommonIngredients = GetListVariable("ARM_Alchemy_FRL_UnCommonIngredients"sv, &properties);
        rareIngredients = GetListVariable("ARM_Alchemy_FRL_RareIngredients"sv, &properties);
        uniqueIngredients = GetListVariable("ARM_Alchemy_FRL_UniqueIngredients"sv, &properties);
        qualityIngredientsPerk = GetPerkVariable("ARM_Alchemy_PRK_050_QualityIngredients"sv, &properties);

        return (uncommonIngredients && rareIngredients && uniqueIngredients && qualityIngredientsPerk);
    }

    float Alchemy::Hooks::SelectedItemMonitor::GetEfficiencyBoostModifier()
    {
        if (efficiencyBonus < 1.0f || efficiencyBonus > 1.85f) return 1.0f;
        return efficiencyBonus;
    }

    //Created Item Monitoring
    bool Alchemy::Hooks::CreatedItemMonitoring::PreloadForms()
    {
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

        slowDeathEffect = GetEffectVariable("ARM_Alchemy_MGF_SlowDeathEffectFFSelf"sv, &properties);
        stimulantsEffectHealth = GetEffectVariable("ARM_Alchemy_MGF_LastingTreatmentHealthFFSelf"sv, &properties);
        stimulantsEffectStamina = GetEffectVariable("ARM_Alchemy_MGF_LastingTreatmentStaminaFFSelf"sv, &properties);
        stimulantsEffectMagicka = GetEffectVariable("ARM_Alchemy_MGF_LastingTreatmentMagickaFFSelf"sv, &properties);
        slowDeathPerk = GetPerkVariable("ARM_Alchemy_PRK_080_SlowDeath"sv, &properties);
        stimulantsPerk = GetPerkVariable("ARM_Alchemy_PRK_080_Stimulants"sv, &properties);

        return (slowDeathEffect && stimulantsEffectHealth && stimulantsEffectStamina && stimulantsEffectMagicka &&
            slowDeathPerk && stimulantsPerk);
    }

    bool Alchemy::Hooks::CreatedItemMonitoring::InstallHook()
    {
        _loggerDebug("_________________________________________________");
        _loggerDebug("ARM: Installing Item Monitoring hook...");
        if (!this->PreloadForms()) return false;

        REL::Relocation<std::uintptr_t> target{ RELOCATION_ID(0, 36179), OFFSET(0x0, 0x16F) };
        SKSE::AllocTrampoline(14);
        auto& trampoline = SKSE::GetTrampoline();
        _originalCall = trampoline.write_call<5>(target.address(), &ItemCreationMonitor);
        _loggerDebug("_________________________________________________");
        return true;
    }

    void Alchemy::Hooks::CreatedItemMonitoring::ItemCreationMonitor(RE::TESDataHandler* a_dataHandler, RE::AlchemyItem* a_alchemyItem) 
    {
        auto* player = RE::PlayerCharacter::GetSingleton();
        auto* singleton = GetSingleton();

        if (a_alchemyItem->IsPoison()) {
            if (player->HasPerk(singleton->slowDeathPerk)) {
                if (const auto effect = new RE::Effect()) {
                    effect->effectItem.magnitude = 10.0f;
                    effect->effectItem.area = 0;
                    effect->effectItem.duration = 5;
                    effect->baseEffect = singleton->slowDeathEffect;
                    effect->cost = 0.0f;

                    a_alchemyItem->effects.push_back(effect);
                }
            }
        }
        else {
            if (player->HasPerk(singleton->stimulantsPerk)) {
                if (const auto healthEffect = new RE::Effect()) {
                    healthEffect->effectItem.magnitude = 50.0f;
                    healthEffect->effectItem.area = 0;
                    healthEffect->effectItem.duration = 60;
                    healthEffect->baseEffect = singleton->stimulantsEffectHealth;
                    healthEffect->cost = 0.0f;

                    a_alchemyItem->effects.push_back(healthEffect);
                }

                if (const auto staminaEffect = new RE::Effect()) {
                    staminaEffect->effectItem.magnitude = 50.0f;
                    staminaEffect->effectItem.area = 0;
                    staminaEffect->effectItem.duration = 60;
                    staminaEffect->baseEffect = singleton->stimulantsEffectStamina;
                    staminaEffect->cost = 0.0f;

                    a_alchemyItem->effects.push_back(staminaEffect);
                }

                if (const auto magickaEffect = new RE::Effect()) {
                    magickaEffect->effectItem.magnitude = 50.0f;
                    magickaEffect->effectItem.area = 0;
                    magickaEffect->effectItem.duration = 60;
                    magickaEffect->baseEffect = singleton->stimulantsEffectMagicka;
                    magickaEffect->cost = 0.0f;

                    a_alchemyItem->effects.push_back(magickaEffect);
                }
            }
        }

        float newFactor = SelectedItemMonitor::GetSingleton()->GetEfficiencyBoostModifier();
        if (newFactor > 1.0f) {
            auto& effects = a_alchemyItem->effects;
            for (auto* effect : effects) {
                effect->effectItem.magnitude *= newFactor;
            }
        }

        return _originalCall(a_dataHandler, a_alchemyItem);
    }
}

bool Armillary::Alchemy::Hooks::Install() 
{
    return SelectedItemMonitor::GetSingleton()->InstallHook() && 
           CreatedItemMonitoring::GetSingleton()->InstallHook();
}
