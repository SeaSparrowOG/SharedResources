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

		auto* foundSpell = UtilityFunctions::GetFormFromQuestScript<RE::SpellItem>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_SPL_GladiatorProc");
		if (!foundSpell) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundSpell));
		gladiatorProc = foundSpell;

		foundSpell = UtilityFunctions::GetFormFromQuestScript<RE::SpellItem>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_SPL_DuelistProc");
		if (!foundSpell) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundSpell));
		duelistProc = foundSpell;

		foundSpell = UtilityFunctions::GetFormFromQuestScript<RE::SpellItem>
			("ARM_Framework_QST_ArmillaryMaintenanceQuest"sv, "ARM_ObjectHolder", "ARM_OneHanded_SPL_DamageArmorOpportunity");
		if (!foundSpell) return false;
		_loggerDebug("Setting: {}", _debugEDID(foundSpell));
		opportunistProc = foundSpell;
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
		if (const auto* attacker = a_hitData->aggressor.get().get()) {
			auto* hitWeapon = a_hitData->weapon;
			bool isOneHanded = hitWeapon ? hitWeapon->IsOneHandedAxe() ||
				hitWeapon->IsOneHandedSword() ||
				hitWeapon->IsOneHandedMace() ||
				hitWeapon->IsOneHandedDagger() :
				false;

			if (isOneHanded && attacker->HasPerk(duelistPerk) && IsVulnerable(a_target)) {
				a_hitData->flags.set(RE::HitData::Flag::kCritical);
				auto* castingSource = a_target->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);

				if (castingSource) {
					castingSource->CastSpellImmediate(duelistProc, false, a_target, 1.0f, false, 0.0f, a_target);
					if (attacker->HasPerk(opportunistPerk)) {
						castingSource->CastSpellImmediate(opportunistProc, false, a_target, 1.0f, false, 0.0f, a_target);
					}
				}
			}

			if (a_target->HasPerk(gladiatorPerk) && a_target->HasSpell(gladiatorProc)) {
				a_hitData->totalDamage = 0.0f;
			}
		}

		return func(a_target, a_hitData);
	}
	bool Install() {
		return CombatHit::Install();
	}
}