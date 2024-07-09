#include "Common/papyrus.h"

namespace Papyrus {
	std::vector<int> GetVersion(STATIC_ARGS) {
		return { Version::MAJOR, Version::MINOR, Version::PATCH };
	}

	int GetMajorVersion(STATIC_ARGS) {
		return Version::MAJOR;
	}

	int GetMinorVersion(STATIC_ARGS) {
		return Version::MINOR;
	}

	int GetPatchVersion(STATIC_ARGS) {
		return Version::PATCH;
	}

	bool MeetsMinimumVersion(STATIC_ARGS, int a_minMajor, int a_minMinor, int a_minPatch) {
		if (a_minMajor > Version::MAJOR) return false;
		if (a_minMajor == Version::MAJOR) {
			if (a_minMinor > Version::MINOR) return false;
			if (a_minPatch > Version::PATCH) return false;
		}

		return true;
	}

	void RefreshAbilities(STATIC_ARGS, std::vector<RE::SpellItem*> a_abilities, bool a_add = true) {
		if (a_abilities.empty()) return;
		auto* player = RE::PlayerCharacter::GetSingleton();
		if (!player) return;

		for (auto* ability : a_abilities) {
			if (player->HasSpell(ability)) {
				player->RemoveSpell(ability);
				if (a_add) {
					player->AddSpell(ability);
				}
			}
			else {
				if (!a_add) continue;
				player->AddSpell(ability);
			}
		}
	}

	void RefreshPerks(STATIC_ARGS, std::vector<RE::BGSPerk*> a_perks, bool a_add = true) {
		if (a_perks.empty()) return;
		auto* player = RE::PlayerCharacter::GetSingleton();
		if (!player) return;

		for (auto* perk : a_perks) {
			if (player->HasPerk(perk)) {
				player->RemovePerk(perk);
				if (a_add) {
					player->AddPerk(perk, 1);
				}
			}
			else {
				if (!a_add) continue;
				player->AddPerk(perk, 1);
			}
		}
	}

	void Bind(VM& a_vm) {
		BIND(GetVersion);
		BIND(GetMajorVersion);
		BIND(GetMinorVersion);
		BIND(GetPatchVersion);

		BIND(MeetsMinimumVersion);
		BIND(RefreshAbilities);
		BIND(RefreshPerks);
	}

	bool RegisterFunctions(VM* a_vm) {
		Bind(*a_vm);
		return true;
	}
}