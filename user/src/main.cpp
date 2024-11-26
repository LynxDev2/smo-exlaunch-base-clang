#include <mallow/mallow.hpp>

#include "ActorFactory/actorPatches.h"

#include "Library/Controller/InputFunction.h"
#include "ModOptions.h"
#include "Player/PlayerCostumeInfo.h"
#include "Scene/StageScene.h"

using mallow::log::logLine;

struct ScenePlayHook : public mallow::hook::Trampoline<ScenePlayHook> {
    static void Callback(StageScene* thisPtr) {
        if (al::isPadTriggerL(-1) && mallow::config::getConfg<ModOptions>()->myModOption)
            logLine("You pressed L while the game was unpaused and myModOption was enabled");
        Orig(thisPtr);
    }
};

struct SpTestHook : public exl::hook::impl::InlineHook<SpTestHook> {
    static void Callback(exl::hook::InlineCtx* ctx) {
        auto costumeInfo = reinterpret_cast<PlayerCostumeInfo*>(ctx->SP[0xa8]);
        mallow::log::logLine("%s", costumeInfo->mBodyInfo->mCostumeName);
    }
};

struct FloatTestHook : public mallow::hook::Inline<FloatTestHook> {
    static void Callback(exl::hook::InlineFloatCtx* ctx) {
        for (int i = 0; i < 6; ++i) {
            mallow::log::logLine("s%i: %.7f", i, ctx->S[i]);
        }
    }
};

struct SpVerifyHook : public mallow::hook::Inline<SpVerifyHook> {
    static void Callback(exl::hook::InlineCtx* ctx) {
        for (int i = -0x300; i < 0x300; i += 0x08) {
            if (ctx->SP[i] == ctx->X[0]) {
                mallow::log::logLine("SP found! %i", i);
            }
        }
        mallow::log::logLine("SP: %p, X0: %p", ctx->SP.Get(), ctx->X[0]);
    }
};

extern "C" void userMain() {
    exl::hook::Initialize();
    mallow::init::installHooks();
    ca::actorPatches();

    ScenePlayHook::InstallAtSymbol("_ZN10StageScene7exePlayEv");
    //SpTestHook::InstallAtOffset(0x41b704);
    // SpVerifyHook::InstallAtOffset(0x41b738);
    FloatTestHook::InstallAtOffset(0x980008);
}
