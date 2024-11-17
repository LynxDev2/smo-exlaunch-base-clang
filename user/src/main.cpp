#include <mallow/config.hpp>
#include <mallow/logging/logSinks.hpp>
#include <mallow/mallow.hpp>

#include "ActorFactory/actorPatches.h"

#define IS_EMU false

static void setupLogging() {
    using namespace mallow::log::sink;
    // This sink writes to a file on the SD card.
    static FileSink fileSink = FileSink("sd:/mallow.log");
    addLogSink(&fileSink);

    // This sink writes to a network socket on a host computer. Raw logs are sent with no
    auto config = mallow::config::getConfig();
    if (config["logger"]["ip"].is<const char*>()) {
        bool tryReconnect = config["logger"]["reconnect"] | false;
        static NetworkSink networkSink = NetworkSink(
            config["logger"]["ip"],
            config["logger"]["port"] | 3080,
            tryReconnect
        );
        if (networkSink.isSuccessfullyConnected() || tryReconnect)
            addLogSink(&networkSink);
        else
            mallow::log::logLine("Failed to connect to the network sink");
    } else {
        mallow::log::logLine("The network logger is unconfigured.");
        if (config["logger"].isNull()) {
            mallow::log::logLine("Please configure the logger in config.json");
        } else if (!config["logger"]["ip"].is<const char*>()) {
            mallow::log::logLine("The IP address is missing or invalid.");
        }
    }
    #if IS_EMU
        static DebugPrintSink debugPrintSink = DebugPrintSink();
        addLogSink(&debugPrintSink);
    #endif
}

using mallow::log::logLine;

//Mod code

struct nnMainHook : public mallow::hook::Trampoline<nnMainHook>{
    static void Callback(){
        nn::fs::MountSdCardForDebug("sd");
        mallow::config::loadConfig(true);

        setupLogging();
        logLine("Hello from smo!");
        Orig();
    }
};

extern "C" void userMain() {
    exl::hook::Initialize();
    nnMainHook::InstallAtSymbol("nnMain");
    ca::actorPatches();
}
