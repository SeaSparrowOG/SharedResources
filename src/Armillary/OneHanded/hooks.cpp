#include "Armillary/OneHanded/hooks.h"

namespace Armillary::OneHanded::Hooks {
	inline RE::BGSPerk* GetPerkVariable(std::string_view a_varName, 
		RE::BSTScrapHashMap<RE::BSFixedString, RE::BSScript::Variable>* a_map) {
		if (!a_map->contains(a_varName)) return nullptr;

		auto& var = a_map->find(a_varName)->second;
		if (var.IsNoneObject() || !var.IsObject()) return nullptr;
		auto* foundObject = var.Unpack<RE::BGSPerk*>();
		if (!foundObject) return nullptr;

		return foundObject;
	}

	inline RE::SpellItem* GetSpellVariable(std::string_view a_varName,
		RE::BSTScrapHashMap<RE::BSFixedString, RE::BSScript::Variable>* a_map) {
		if (!a_map->contains(a_varName)) return nullptr;

		auto& var = a_map->find(a_varName)->second;
		if (var.IsNoneObject() || !var.IsObject()) return nullptr;
		auto* foundObject = var.Unpack<RE::SpellItem*>();
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

	inline bool IsVulnerable(RE::Actor* a_target, RE::EffectSetting* a_duelistProc)
	{
		bool isStaggered = a_target->actorState2.staggered > 0;
		bool isDrawingBow = a_target->GetAttackState() == RE::ATTACK_STATE_ENUM::kBowDrawn;

		auto* highProcess = a_target->GetHighProcess();
		auto* attackData = highProcess ? highProcess->attackData.get() : nullptr;
		bool isPowerAttacking = attackData ? attackData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack) : false;
		
		//janky jank jank
		auto* equippedLeft = a_target->GetEquippedObject(true);
		auto* equippedRight = a_target->GetEquippedObject(false);

		auto* equippedLeftSpell = equippedLeft ? equippedLeft->As<RE::SpellItem>() : nullptr;
		auto* equippedRightSpell = equippedRight ? equippedRight->As<RE::SpellItem>() : nullptr;
		bool castingLeft = equippedLeftSpell ? a_target->IsCasting(equippedLeftSpell) : false;
		bool castingRight = equippedRightSpell ? a_target->IsCasting(equippedRightSpell) : false;

		bool isWarding = false;
		if (castingLeft) {
			for (auto* effect : equippedLeftSpell->effects) {
				if (!effect->baseEffect) continue;
				if (effect->baseEffect->data.associatedSkill != RE::ActorValue::kWardPower) continue;
				isWarding = true;
				break;
			}
		}

		if (!isWarding && castingRight) {
			for (auto* effect : equippedRightSpell->effects) {
				if (!effect->baseEffect) continue;
				if (effect->baseEffect->data.associatedSkill != RE::ActorValue::kWardPower) continue;
				isWarding = true;
				break;
			}
		}

		if (isWarding) return false;
		return castingLeft || castingRight || isPowerAttacking || isDrawingBow || isStaggered || a_target->HasMagicEffect(a_duelistProc);
	}

	bool CombatHit::PreloadForms()
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

		duelistPerk = GetPerkVariable("ARM_OneHanded_PRK_020_Duelist"sv, &properties);
		opportunistPerk = GetPerkVariable("ARM_OneHanded_PRK_050_ArmorBreaker"sv, &properties);
		wardancerPerk = GetPerkVariable("ARM_OneHanded_PRK_100_Wardancer"sv, &properties);
		maceRank2Perk = GetPerkVariable("ARM_OneHanded_PRK_060_Paladin"sv, &properties);
		axeRank2Perk = GetPerkVariable("ARM_OneHanded_PRK_060_Carver"sv, &properties);
		gladiatorPerk = GetPerkVariable("ARM_OneHanded_PRK_075_Gladiator"sv, &properties);

		//Spells

		duelistProc = GetSpellVariable("ARM_OneHanded_SPL_DuelistProc"sv, &properties);
		duelistMark = GetSpellVariable("ARM_OneHanded_SPL_DuelistMark"sv, &properties);
		opportunistProc = GetSpellVariable("ARM_OneHanded_SPL_DamageArmorOpportunity"sv, &properties);
		wardancerProc = GetSpellVariable("ARM_OneHanded_SPL_WardancerProc"sv, &properties);
		maceProc = GetSpellVariable("ARM_OneHanded_SPL_MaceDamageArmor"sv, &properties);
		axeProc = GetSpellVariable("ARM_OneHanded_SPL_AxeAdditionalBleed"sv, &properties);

		//Effects

		duelistCountdown = GetEffectVariable("ARM_OneHanded_MGF_DuelistCountdownEffect"sv, &properties);
		markCountdown = GetEffectVariable("ARM_OneHanded_MGF_MarkCountdownEffect"sv, &properties);

		return (duelistPerk && opportunistPerk && wardancerPerk && maceRank2Perk && axeRank2Perk && gladiatorPerk &&
			duelistProc && duelistMark && opportunistProc && wardancerProc && maceProc && axeProc &&
			duelistCountdown && markCountdown);
	}

	bool CombatHit::Install()
	{
		if (!PreloadForms()) return false;
		REL::Relocation<std::uintptr_t> target{ REL::ID(38627), 0x4A8 };
		stl::write_thunk_call<CombatHit>(target.address());
		_loggerDebug("Wrote hook - Perk entry combat hit");
		return true;
	}

	void CombatHit::thunk(RE::Actor* a_target, RE::HitData* a_hitData)
	{
		if (auto* attacker = a_hitData->aggressor.get().get()) {
			auto* hitWeapon = a_hitData->weapon;
			bool isOneHanded = hitWeapon ? hitWeapon->IsOneHandedAxe() ||
				hitWeapon->IsOneHandedSword() ||
				hitWeapon->IsOneHandedMace() ||
				hitWeapon->IsOneHandedDagger() :
				false;

			auto* castingSource = a_target->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);

			if (isOneHanded && castingSource && a_target->HasMagicEffect(markCountdown) && !a_target->IsBlocking()) {
				if (hitWeapon->IsOneHandedMace() && attacker->HasPerk(maceRank2Perk) && castingSource) {
					castingSource->CastSpellImmediate(maceProc, false, a_target, 1.0f, false, 0.0f, a_target);
				}

				if (hitWeapon->IsOneHandedAxe() && attacker->HasPerk(axeRank2Perk) && castingSource) {
					auto* bleedEffect = *axeProc->effects.begin();
					bleedEffect->effectItem.magnitude = a_target->GetBaseActorValue(RE::ActorValue::kHealth) / 400.0f;
					castingSource->CastSpellImmediate(axeProc, false, a_target, 1.0f, false, 0.0f, a_target);
				}
			}

			if (isOneHanded && attacker->HasPerk(duelistPerk) && IsVulnerable(a_target, duelistCountdown)) {
				a_hitData->flags.set(RE::HitData::Flag::kCritical);
				a_hitData->totalDamage *= 1.25f;

				if (castingSource) {
					if (attacker->HasPerk(opportunistPerk)) {
						castingSource->CastSpellImmediate(opportunistProc, false, a_target, 1.0f, false, 0.0f, a_target);
					}

					if (attacker->HasPerk(wardancerPerk) && a_target->HasMagicEffect(markCountdown)) {
						castingSource->CastSpellImmediate(wardancerProc, false, attacker, 1.0f, false, 0.0f, attacker);
					}
				}
			}

			if (a_target->IsBlocking() && a_target->HasPerk(duelistPerk) && a_target->HasMagicEffect(duelistCountdown)) {
				if (a_target->HasPerk(gladiatorPerk)) {
					a_hitData->totalDamage = 0.0f;
					auto ref = a_target->GetHandle();
					a_target->DispelEffect(duelistCooldown, ref);
				}
				else {
					a_hitData->totalDamage /= 2.0f;
				}

				if (castingSource) {
					castingSource->CastSpellImmediate(duelistMark, false, attacker, 1.0f, false, 0.0f, attacker);
				}

				auto targetHandle = a_target->GetHandle();
				a_target->DispelEffect(duelistProc, targetHandle);
			}
		}

		return func(a_target, a_hitData);
	}
	bool Install() {
		return CombatHit::Install();
	}
}