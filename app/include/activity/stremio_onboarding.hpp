#pragma once

#include <borealis.hpp>

class ActionBar;

class StremioOnboarding : public brls::Box {
public:
    StremioOnboarding();

    void draw(NVGcontext* vg, float x, float y, float width, float height, brls::Style style,
        brls::FrameContext* ctx) override;

private:
    void render();
    void next();
    void previous();
    void openQrLogin();
    void openManualAddonPrompt();
    void complete();

    brls::Label* step = nullptr;
    brls::Label* title = nullptr;
    brls::Label* body = nullptr;
    brls::Label* hint = nullptr;
    ActionBar* actionBar = nullptr;
    size_t page = 0;
};
