#include "ModDrawer/ModDrawer.h"

#include <cstdlib>
#include <mallow/logging/logger.hpp>
#include <mallow/mallow.hpp>

#include <heap/seadHeapMgr.h>
#include <gfx/nvn/seadDebugFontMgrNvn.h>
#include <gfx/seadViewport.h>
#include <common/aglRenderBuffer.h>
#include <common/aglDrawContext.h>

#include "Library/File/FileUtil.h"

#include "System/Application.h"
#include "System/GameDrawInfo.h"

SEAD_SINGLETON_DISPOSER_IMPL(ModDrawer);

#define MAX_MESSAGES 50

ModDrawer::ModDrawer() = default;
ModDrawer::~ModDrawer() = default;

struct DrawModHook : public mallow::hook::Trampoline<DrawModHook>{
    static void Callback(HakoniwaSequence* thisPtr){
        Orig(thisPtr);
        ModDrawer::instance()->draw(thisPtr);
    }
};

//Credits: LunaKit
void ModDrawer::setup(const char* shaderPath, const char* fontPath, const char* tblPath){
    sead::Heap* curHeap = sead::HeapMgr::instance()->getCurrentHeap();

    sead::DebugFontMgrJis1Nvn::createInstance(curHeap);

    if (al::isExistFile(shaderPath) && al::isExistFile(fontPath) && al::isExistFile(tblPath)) {
        sead::DebugFontMgrJis1Nvn::instance()->initialize(curHeap, shaderPath, fontPath, tblPath, 0x100000);
    } else {
        mallow::log::logLine("Drawer debug data not found!");
    }
    sead::TextWriter::setDefaultFont(sead::DebugFontMgrJis1Nvn::instance());

    al::GameDrawInfo* drawInfo = Application::instance()->mGameDrawInfo;

    agl::DrawContext* context = drawInfo->mDrawContext;
    agl::RenderBuffer* renderBuffer = drawInfo->mFirstRenderBuffer;

    sead::Viewport* viewport = new sead::Viewport(*renderBuffer);

    mTextWriter = new sead::TextWriter(context, viewport);

    mTextWriter->setupGraphics(context);

    mTextWriter->mColor = sead::Color4f(1.f, 1.f, 1.f, 0.8f);

    mMessages = new Message[MAX_MESSAGES];

    DrawModHook::InstallAtSymbol("_ZNK16HakoniwaSequence8drawMainEv");
}

void ModDrawer::draw(HakoniwaSequence* seq){
    mTextWriter->beginDraw();
    for(s32 i = 0; i < mMessageCount; i++){
        if (!mMessages[i].tick()){
            mNextMessageIndex = i;
            continue;
        }
        mTextWriter->setCursorFromTopLeft(mMessages[i].pos);
        mTextWriter->setScaleFromFontHeight(mMessages[i].fontSize);
        mTextWriter->mColor = mMessages[i].color;
        if(mMessages[i].text)
            mTextWriter->printf(mMessages[i].text);
    }
    mTextWriter->endDraw();
}

void ModDrawer::addMessage(const char* message, s32 time, sead::Vector2f pos, sead::Color4f color, f32 size){
    mMessages[mNextMessageIndex] = Message{ message, pos, color, size, time };
    mMessageCount++;
    mNextMessageIndex = mMessageCount;
}
