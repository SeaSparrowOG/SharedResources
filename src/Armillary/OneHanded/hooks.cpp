#include "Armillary/OneHanded/hooks.h"
#include "Common/utilityFuncs.h"

namespace Armillary::OneHanded::Hooks {
	inline bool IsVulnerable(const RE::Actor* a_target)
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

		return castingLeft || castingRight || isPowerAttacking || isDrawingBow || isStaggered;
	}

	bool CombatHit::PreloadForms()
	{
		auto* foundPerk = UtilityFunctions::GetFormFromQuestScript<RE::BGSPerk>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_PRK_075_Gladiator");
		if (!foundPerk) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundPerk));
		gladiatorPerk = foundPerk;

		foundPerk = UtilityFunctions::GetFormFromQuestScript<RE::BGSPerk>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_PRK_020_Duelist");
		if (!foundPerk) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundPerk));
		duelistPerk = foundPerk;

		foundPerk = UtilityFunctions::GetFormFromQuestScript<RE::BGSPerk>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_PRK_050_ArmorBreaker");
		if (!foundPerk) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundPerk));
		opportunistPerk = foundPerk;

		foundPerk = UtilityFunctions::GetFormFromQuestScript<RE::BGSPerk>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_PRK_100_Wardancer");
		if (!foundPerk) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundPerk));
		wardancerPerk = foundPerk;

		foundPerk = UtilityFunctions::GetFormFromQuestScript<RE::BGSPerk>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_PRK_060_Paladin");
		if (!foundPerk) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundPerk));
		maceRank2Perk = foundPerk;

		foundPerk = UtilityFunctions::GetFormFromQuestScript<RE::BGSPerk>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_PRK_060_Carver");
		if (!foundPerk) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundPerk));
		axeRank2Perk = foundPerk;

		//Spells

		auto* foundSpell = UtilityFunctions::GetFormFromQuestScript<RE::SpellItem>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_SPL_DuelistProc");
		if (!foundSpell) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundSpell));
		duelistProc = foundSpell;

		foundSpell = UtilityFunctions::GetFormFromQuestScript<RE::SpellItem>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_SPL_DuelistMark");
		if (!foundSpell) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundSpell));
		duelistMark = foundSpell;

		foundSpell = UtilityFunctions::GetFormFromQuestScript<RE::SpellItem>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_SPL_DamageArmorOpportunity");
		if (!foundSpell) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundSpell));
		opportunistProc = foundSpell;

		foundSpell = UtilityFunctions::GetFormFromQuestScript<RE::SpellItem>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_SPL_WardancerProc");
		if (!foundSpell) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundSpell));
		wardancerProc = foundSpell;

		foundSpell = UtilityFunctions::GetFormFromQuestScript<RE::SpellItem>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_SPL_MaceDamageArmor");
		if (!foundSpell) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundSpell));
		maceProc = foundSpell;

		foundSpell = UtilityFunctions::GetFormFromQuestScript<RE::SpellItem>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_SPL_AxeAdditionalBleed");
		if (!foundSpell) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundSpell));
		axeProc = foundSpell;

		//Effects

		auto* foundEffect = UtilityFunctions::GetFormFromQuestScript<RE::EffectSetting>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_MGF_DuelistCountdownEffect");
		if (!foundEffect) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundEffect));
		duelistCountdown = foundEffect;

		foundEffect = UtilityFunctions::GetFormFromQuestScript<RE::EffectSetting>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_MGF_MarkCountdownEffect");
		if (!foundEffect) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundEffect));
		markCountdown = foundEffect;
		return true;
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

			if (isOneHanded && attacker->HasPerk(duelistPerk) && IsVulnerable(a_target)) {
				a_hitData->flags.set(RE::HitData::Flag::kCritical);

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
					a_target->DispelEffect(maceProc, ref);
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