#include "Armillary/Alchemy/hooks.h"

namespace INI {
	bool ShouldRebuildINI(CSimpleIniA* a_ini) {
		const char* section = "Alchemy";
		const char* keys[] = {
			"uncommonFormlist",
			"rareFormlist",
			"uniqueFormlist",
			"qualityPerk",
            "stimulants",
            "stimulantsEffectHealth",
            "stimulantsEffectStamina",
            "stimulantsEffectMagicka",
            "slowDeath",
            "slowDeathEffect"};

		int sectionLength = sizeof(keys) / sizeof(keys[0]);
		std::list<CSimpleIniA::Entry> keyHolder;

		a_ini->GetAllKeys(section, keyHolder);
		if (std::size(keyHolder) != sectionLength) return true;
		for (auto* key : keys) {
			if (!a_ini->KeyExists(section, key)) return true;
		}
		return false;
	}

    bool IsHex(std::string const& s) {
        return s.compare(0, 2, "0x") == 0
            && s.size() > 2
            && s.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;
    }

    RE::TESForm* ParseForm(const std::string& a_identifier) {
        if (!IsHex(a_identifier)) return nullptr;

        auto formID = clib_util::string::to_num<RE::FormID>(a_identifier, true);
        auto* baseForm = RE::TESDataHandler::GetSingleton()->LookupForm(formID, "Armillary.esp"sv);
        return baseForm;
    }
}

namespace Armillary {
	bool Alchemy::Hooks::SelectedItemMonitor::InstallHook() 
    {
        _loggerDebug("_________________________________________________");
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
		std::filesystem::path f{ "./Data/SKSE/Plugins/Armillary.ini" };
		bool createEntries = false;
		if (!std::filesystem::exists(f)) {
			std::fstream createdINI;
			createdINI.open(f, std::ios::out);
			createdINI.close();
			createEntries = true;
		}

		CSimpleIniA ini;
		ini.SetUnicode();
		ini.LoadFile(f.c_str());
		if (!createEntries) { createEntries = INI::ShouldRebuildINI(&ini); }

		if (createEntries) {
			ini.Delete("Alchemy", NULL);
            ini.SetValue("Alchemy", "uncommonFormlist", "0xFB06");
            ini.SetValue("Alchemy", "rareFormlist", "0xFB07");
            ini.SetValue("Alchemy", "uniqueFormlist", "0xFB08");
            ini.SetValue("Alchemy", "qualityPerk", "0x5900");
            ini.SetValue("Alchemy", "slowDeath", "0x5901");
            ini.SetValue("Alchemy", "slowDeathEffect", "0x14C09");
            ini.SetValue("Alchemy", "stimulants", "0x5903");
            ini.SetValue("Alchemy", "stimulantsEffectHealth", "0x14C0A");
            ini.SetValue("Alchemy", "stimulantsEffectStamina", "0x14C0B");
            ini.SetValue("Alchemy", "stimulantsEffectMagicka", "0x14C0C");
			ini.SaveFile(f.c_str());
		}

        _loggerDebug("    Fetching uncommon list...");
        auto* uncommonFormlistBase = INI::ParseForm(ini.GetValue("Alchemy", "uncommonFormlist", "0xFB06"));
        auto* uncommonFormlist = uncommonFormlistBase ? uncommonFormlistBase->As<RE::BGSListForm>() : nullptr;
        if (!uncommonFormlist) return false;

        _loggerDebug("    Fetching rare list...");
        auto* rareFormlistBase = INI::ParseForm(ini.GetValue("Alchemy", "rareFormlist", "0xFB07"));
        auto* rareFormlist = rareFormlistBase ? rareFormlistBase->As<RE::BGSListForm>() : nullptr;
        if (!rareFormlist) return false;

        _loggerDebug("    Fetching unique list...");
        auto* uniqueFormlistBase = INI::ParseForm(ini.GetValue("Alchemy", "uniqueFormlist", "0xFB08"));
        auto* uniqueFormlist = uniqueFormlistBase ? uniqueFormlistBase->As<RE::BGSListForm>() : nullptr;
        if (!uniqueFormlist) return false;

        _loggerDebug("    Fetching quality perk...");
        auto* qualityPerkBase = INI::ParseForm(ini.GetValue("Alchemy", "qualityPerk", "0x5900"));
        auto* qualityPerk = qualityPerkBase ? qualityPerkBase->As<RE::BGSPerk>() : nullptr;
        if (!qualityPerk) return false;

        this->uncommonIngredients = uncommonFormlist;
        this->rareIngredients = rareFormlist;
        this->uniqueIngredients = uniqueFormlist;
        this->qualityIngredientsPerk = qualityPerk;
        _loggerDebug("    All forms fetched successfully");
        return true;
    }

    float Alchemy::Hooks::SelectedItemMonitor::GetEfficiencyBoostModifier()
    {
        if (efficiencyBonus < 1.0f || efficiencyBonus > 1.85f) return 1.0f;
        return efficiencyBonus;
    }

    //Created Item Monitoring
    bool Alchemy::Hooks::CreatedItemMonitoring::PreloadForms()
    {
        std::filesystem::path f{ "./Data/SKSE/Plugins/Armillary.ini" };
        bool createEntries = false;
        if (!std::filesystem::exists(f)) {
            std::fstream createdINI;
            createdINI.open(f, std::ios::out);
            createdINI.close();
            createEntries = true;
        }

        CSimpleIniA ini;
        ini.SetUnicode();
        ini.LoadFile(f.c_str());
        if (!createEntries) { createEntries = INI::ShouldRebuildINI(&ini); }

        if (createEntries) {
            ini.Delete("Alchemy", NULL);
            ini.SetValue("Alchemy", "uncommonFormlist", "0xFB06");
            ini.SetValue("Alchemy", "rareFormlist", "0xFB07");
            ini.SetValue("Alchemy", "uniqueFormlist", "0xFB08");
            ini.SetValue("Alchemy", "qualityPerk", "0x5900");
            ini.SetValue("Alchemy", "slowDeath", "0x5901");
            ini.SetValue("Alchemy", "slowDeathEffect", "0x14C09");
            ini.SetValue("Alchemy", "stimulants", "0x5903");
            ini.SetValue("Alchemy", "stimulantsEffectHealth", "0x14C0A");
            ini.SetValue("Alchemy", "stimulantsEffectStamina", "0x14C0B");
            ini.SetValue("Alchemy", "stimulantsEffectMagicka", "0x14C0C");
            ini.SaveFile(f.c_str());
        }

        _loggerDebug("    Fetching health effect...");
        auto* stimulantsEffectHealthBase = INI::ParseForm(ini.GetValue("Alchemy", "stimulantsEffectHealth", "0x14C0A"));
        auto* stimulantsHealth = stimulantsEffectHealthBase ? stimulantsEffectHealthBase->As<RE::EffectSetting>() : nullptr;
        if (!stimulantsHealth) return false;

        _loggerDebug("    Fetching stamina effect...");
        auto* stimulantsEffectStaminaBase = INI::ParseForm(ini.GetValue("Alchemy", "stimulantsEffectStamina", "0x14C0B"));
        auto* stimulantsStamina = stimulantsEffectStaminaBase ? stimulantsEffectStaminaBase->As<RE::EffectSetting>() : nullptr;
        if (!stimulantsStamina) return false;

        _loggerDebug("    Fetching magicka effect...");
        auto* stimulantsEffectMagickaBase = INI::ParseForm(ini.GetValue("Alchemy", "stimulantsEffectMagicka", "0x14C0C"));
        auto* stimulantsMagicka = stimulantsEffectMagickaBase ? stimulantsEffectMagickaBase->As<RE::EffectSetting>() : nullptr;
        if (!stimulantsMagicka) return false;

        _loggerDebug("    Fetching stimulants perk...");
        auto* stimulantsBase = INI::ParseForm(ini.GetValue("Alchemy", "stimulants", "0x5903"));
        auto* stimulants = stimulantsBase ? stimulantsBase->As<RE::BGSPerk>() : nullptr;
        if (!stimulants) return false;

        _loggerDebug("    Fetching slow death effect...");
        auto* slowDeathEffectBase = INI::ParseForm(ini.GetValue("Alchemy", "slowDeathEffect", "0x14C09"));
        auto* death = slowDeathEffectBase ? slowDeathEffectBase->As<RE::EffectSetting>() : nullptr;
        if (!stimulants) return false;

        _loggerDebug("    Fetching slow death perk...");
        auto* slowDeathBase = INI::ParseForm(ini.GetValue("Alchemy", "slowDeath", "0x5901"));
        auto* slowDeath = slowDeathBase ? slowDeathBase->As<RE::BGSPerk>() : nullptr;
        if (!slowDeath) return false;

        this->slowDeathPerk = slowDeath;
        this->stimulantsPerk = stimulants;
        this->stimulantsEffectHealth = stimulantsHealth;
        this->stimulantsEffectStamina = stimulantsStamina;
        this->stimulantsEffectMagicka = stimulantsMagicka;
        this->slowDeathEffect = death;
        _loggerDebug("    All forms fetched successfully");
        return true;
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
