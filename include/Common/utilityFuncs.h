#pragma once

namespace UtilityFunctions {
	template <typename T>
	T* GetFormFromQuestScript(std::string_view a_questName, const RE::BSFixedString a_scriptName, const char* a_propertyName) {
		T* response = nullptr;

		const auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
		if (!vm) return response;

		const auto bindPolicy = vm->GetObjectBindPolicy();
		const auto handlePolicy = vm->GetObjectHandlePolicy();

		if (!bindPolicy || !handlePolicy) return response;

		const auto quest = RE::TESForm::LookupByEditorID<RE::TESQuest>(a_questName);
		const auto handle = handlePolicy->GetHandleForObject(RE::TESQuest::FORMTYPE, quest);

		RE::BSTScrapHashMap<RE::BSFixedString, RE::BSScript::Variable> properties;
		std::uint32_t nonConverted;
		bindPolicy->GetInitialPropertyValues(handle, a_scriptName, properties, nonConverted);
		if (!properties.contains(a_propertyName)) return response;

		auto& var = *properties.find(a_propertyName);
		return var.second.Unpack<T*>();
	}
}