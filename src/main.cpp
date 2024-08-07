#include <spdlog/sinks/basic_file_sink.h>

#include "services.h"

namespace Log {
    void SetupLog() {
        auto logsFolder = SKSE::log::log_directory();
        if (!logsFolder) SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");

        auto pluginName = Version::PROJECT;
        auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
        auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
        auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));

        spdlog::set_default_logger(std::move(loggerPtr));
#ifdef LOG_DEBUG
        spdlog::set_level(spdlog::level::debug);
        spdlog::flush_on(spdlog::level::debug);
#else
        spdlog::set_level(spdlog::level::info);
        spdlog::flush_on(spdlog::level::info);
#endif

        //Pattern
        spdlog::set_pattern("%v");
    }
}

void MessageHandler(SKSE::MessagingInterface::Message* a_message) {
    switch (a_message->type) {
    case SKSE::MessagingInterface::kDataLoaded:
        _loggerInfo("----------------------------------------------------------------");
        if (!Services::InstallDataLoadedPatches()) {
            _loggerError("Installation was unsuccessfull.");
            SKSE::stl::report_and_fail("Failed to install Armillary. Check the logs for details."sv);
        }
        _loggerInfo("Finished startup tasks.");
        _loggerInfo("----------------------------------------------------------------");
        break;
    case SKSE::MessagingInterface::kNewGame:
    case SKSE::MessagingInterface::kPostLoadGame:
        break;
    default:
        break;
    }
}

#ifdef SKYRIM_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
    SKSE::PluginVersionData v;
    v.PluginVersion({ Version::MAJOR, Version::MINOR, Version::PATCH });
    v.PluginName(Version::PROJECT);
    v.AuthorName(Version::AUTHOR);
    v.UsesAddressLibrary();
    v.UsesUpdatedStructs();
    v.CompatibleVersions({
        SKSE::RUNTIME_1_6_1130,
        _1_6_1170 });
    return v;
    }();
#else 
SKSEPluginInfo(
    .Name = Version::PROJECT
)
#endif

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface * a_skse) {
    Log::SetupLog();
    _loggerInfo("Starting up {}.", Version::PROJECT);
    _loggerInfo("Plugin Version: {}.{}.{}", Version::MAJOR, Version::MINOR, Version::PATCH);
    _loggerInfo("Version build:");

#ifdef SKYRIM_AE
    _loggerInfo("    >Latest Version. Maintained by: {}.", Version::AUTHOR);
#else
    _loggerInfo("    >Older Version. Do not report ANY issues with this version.");
#endif
    _loggerInfo("----------------------------------------------------------------");

    SKSE::Init(a_skse);
    auto messaging = SKSE::GetMessagingInterface();
    messaging->RegisterListener(MessageHandler);
    return true;
}