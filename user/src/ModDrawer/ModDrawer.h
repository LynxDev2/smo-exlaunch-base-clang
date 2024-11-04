#pragma once

#include <heap/seadDisposer.h>
#include <gfx/seadTextWriter.h>

class HakoniwaSequence;

class ModDrawer{
    SEAD_SINGLETON_DISPOSER(ModDrawer);
    ModDrawer();
    ~ModDrawer();

public:
    void setup(const char* shaderPath, const char* fontPath, const char* tblPath);
    void draw(HakoniwaSequence* seq);
    void addMessage(const char* message, s32 time = -1, sead::Vector2f pos = sead::Vector2f(30.f, 30.f), sead::Color4f color = sead::Color4f(1.f, 1.f, 1.f, 0.8f), f32 size = 20.f);
private:

    struct Message{
        const char* text;
        sead::Vector2f pos;
        sead::Color4f color;
        f32 fontSize;
        s32 framesLeft;

        bool tick(){
            if (framesLeft > 0)
                framesLeft--;
            if (framesLeft <= 0)
                return false;
            return true;
        }
    };

    sead::TextWriter* mTextWriter = nullptr;
    Message* mMessages = nullptr;
    u32 mMessageCount = 0;
    u32 mNextMessageIndex = 0;

};
