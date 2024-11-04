#include <mallow/config.hpp>
#include <mallow/mallow.hpp>

#include "ActorFactory/actorPatches.h"
#include "ModDrawer/ModDrawer.h"
#include "heap/seadHeapMgr.h"

static void setupLogging() {
    using namespace mallow::log::sink;
    // This sink writes to a file on the SD card.
    static FileSink fileSink = FileSink("sd:/mallow.log");
    addLogSink(&fileSink);

    // This sink writes to a network socket on a host computer. Raw logs are sent with no
    auto config = mallow::config::getConfig();
    if (config["logger"]["ip"].is<const char*>()) {
        static NetworkSink networkSink = NetworkSink(
            config["logger"]["ip"],
            config["logger"]["port"] | 3080
        );
        if (networkSink.isSuccessfullyConnected())
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
}

using mallow::log::logLine;

//Mod code

static const char* DBG_FONT_PATH = "DebugData/Font/nvn_font_jis1.ntx";
static const char* DBG_SHADER_PATH = "DebugData/Font/nvn_font_shader_jis1.bin";
static const char* DBG_TBL_PATH = "DebugData/Font/nvn_font_jis1_tbl.bin";

struct nnMainHook : public mallow::hook::Trampoline<nnMainHook>{
    static void Callback(){
        nn::fs::MountSdCardForDebug("sd");
        mallow::config::loadConfig(true);

        setupLogging();
        logLine("Hello from smo!");
        Orig();
    }
};

struct GameSystemInitHook : public mallow::hook::Trampoline<GameSystemInitHook>{
    static void Callback(void* thisPtr){
        ModDrawer::createInstance(sead::HeapMgr::instance()->getCurrentHeap());
        ModDrawer::instance()->setup(DBG_SHADER_PATH, DBG_FONT_PATH, DBG_TBL_PATH);
        ModDrawer::instance()->addMessage("Hello message", 6000, sead::Vector2f(40.f, 40.f));
        Orig(thisPtr);
    }
};

extern "C" void userMain() {
    exl::hook::Initialize();
    nnMainHook::InstallAtSymbol("nnMain");
    GameSystemInitHook::InstallAtSymbol("_ZN10GameSystem4initEv");
    ca::actorPatches();
}
