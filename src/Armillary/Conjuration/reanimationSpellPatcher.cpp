#include "Armillary/Conjuration/reanimationSpellPatcher.h"

namespace Armillary::Conjuration::ReanimationSpellPatcher {
	bool ReanimationSpellPatcher::PreloadForms()
	{
        _loggerDebug("ARM: Reanimation Spell Patcher: Looking up forms...");
        const auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
        if (!vm) {
            return false;
        }

        const auto bindPolicy = vm->GetObjectBindPolicy();
        const auto handlePolicy = vm->GetObjectHandlePolicy();

        if (!bindPolicy || !handlePolicy) {
            return false;
        }

        const auto quest = RE::TESForm::LookupByEditorID<RE::TESQuest>("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv);
        const auto handle = handlePolicy->GetHandleForObject(RE::TESQuest::FORMTYPE, quest);

        RE::BSTScrapHashMap<RE::BSFixedString, RE::BSScript::Variable> properties;
        std::uint32_t nonConverted;
        bindPolicy->GetInitialPropertyValues(handle, "ARM_ObjectHolder", properties, nonConverted);

        for (const auto& [name, var] : properties) {
			if (name == "ARM_Conjuration_MGF_DeadlyPremonitionFFAimed") {
				auto* foundForm = var.Unpack<RE::EffectSetting*>();
				if (!foundForm) {
					continue;
				}

				this->secondaryReanimationFFAimed = foundForm;
			}
			else if (name == "ARM_Conjuration_MGF_DeadlyPremonitionFFContact") {
				auto* foundForm = var.Unpack<RE::EffectSetting*>();
				if (!foundForm) {
					continue;
				}

				this->secondaryReanimationFFContact = foundForm;
			}
			else if (name == "ARM_Conjuration_MGF_DeadlyPremonitionFFTActor") {
				auto* foundForm = var.Unpack<RE::EffectSetting*>();
				if (!foundForm) {
					continue;
				}

				this->secondaryReanimationFFTargetActor = foundForm;
			}
        }
		if (!(secondaryReanimationFFAimed && secondaryReanimationFFContact && secondaryReanimationFFTargetActor)) {
			_loggerDebug("One or more required forms was not found. Aborting load...");
			return false;
		}
		return true;
	}

	bool ReanimationSpellPatcher::PatchReanimationSpells()
	{
		if (!PreloadForms()) {
			_loggerDebug("    Failed to preload forms, aborting load.");
			return false;
		}

		_loggerDebug("Adding additional effects to reanimation spells...");
		_loggerDebug("    Fetching data handler...");
		auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) return false;
		_loggerDebug("    Fetching spell array...");
		auto& spellArray = dataHandler->GetFormArray<RE::SpellItem>();
		if (spellArray.empty()) return false;

		std::size_t patchedSpells = 0;
		for (auto* spell : spellArray) {
			if (!spell->GetPlayable()) continue;
			if (spell->effects.empty()) continue;

			auto& effects = spell->effects;
			auto delivery = RE::MagicSystem::Delivery::kAimed;
			bool isReanimation = false;
			float damage = 50.0f;
			uint32_t area = 0;

			for (auto it = effects.begin(); it != effects.end() && !isReanimation; ++it) {
				auto* baseEffect = (*it)->baseEffect;
				if (!baseEffect) continue;
				if (!baseEffect->HasArchetype(RE::EffectSetting::Archetype::kReanimate)) continue;

				auto castingType = baseEffect->data.castingType;
				if (castingType != RE::MagicSystem::CastingType::kFireAndForget) continue;

				++patchedSpells;
				delivery = baseEffect->data.delivery;
				float strength = (*it)->GetMagnitude();
				if (strength <= 10) {
					damage = 5.0f;
				}
				else if (strength <= 20) {
					damage = 7.0f;
				}
				else if (strength <= 30) {
					damage = 10.0f;
				}
				else if (strength <= 40) {
					damage = 13.0f;
				}
				else {
					damage = 15.0f;
				}

				area = (*it)->GetArea();
				isReanimation = true;
			}
			if (!isReanimation) continue;

			auto effect = new RE::Effect();
			effect->effectItem.area = area;
			effect->effectItem.duration = 5;
			effect->effectItem.magnitude = damage;
			effect->cost = 0.0f;

			if (delivery == RE::MagicSystem::Delivery::kAimed) {
				effect->baseEffect = this->secondaryReanimationFFAimed;
			}
			else if (delivery == RE::MagicSystem::Delivery::kTargetActor) {
				effect->baseEffect = this->secondaryReanimationFFTargetActor;
			}
			else if (delivery == RE::MagicSystem::Delivery::kTouch) {
				effect->baseEffect = this->secondaryReanimationFFContact;
			}
			else {
				continue;
			}
			spell->effects.push_back(effect);
		}

		_loggerDebug("    Patched {} reanimation spells.", patchedSpells);
		_loggerDebug("_________________________________________________");
		return true;
	}
}