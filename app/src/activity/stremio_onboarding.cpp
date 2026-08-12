#include "activity/stremio_onboarding.hpp"

#include "activity/stremio_login.hpp"
#include "api/stremio.hpp"
#include "utils/config.hpp"
#include "view/action_bar.hpp"
#include "view/stremio_theme.hpp"

#include <array>

namespace {

struct OnboardingPage {
    std::string step;
    std::string title;
    std::string body;
    std::string hint;
};

const std::array<OnboardingPage, 4> PAGES = {{
    {"1 of 4", "Welcome to ReelNX",
        "Browse movies, series, anime, your Library, and Continue Watching from an independent controller-first media home screen.",
        "Press A to continue."},
    {"2 of 4", "Controller basics",
        "Use the D-pad or left stick to move focus. Press A for the visible primary action, B to go back, Y to search, and - for addon setup.",
        "Focused cards also show what A will do."},
    {"3 of 4", "Connect Stremio",
        "Best option: scan the QR code, approve login in Stremio, and ReelNX will sync compatible addons. If QR is unavailable, enter an addon URL manually or place it in switch/ReelNX/reelnx-addon.txt.",
        "Press X for QR login or Y for manual URL."},
    {"4 of 4", "Ready to stream",
        "You can revisit this tutorial later from Settings > Show tutorial again. When you are ready, start browsing and pick a title.",
        "Press A to finish."},
}};

}  // namespace

StremioOnboarding::StremioOnboarding() {
    this->setAxis(brls::Axis::COLUMN);
    this->setDimensions(brls::Application::contentWidth, brls::Application::contentHeight);
    this->setPadding(64, 72, 0, 72);
    this->setFocusable(true);
    this->setHideHighlight(true);

    auto* content = new brls::Box();
    content->setAxis(brls::Axis::COLUMN);
    content->setGrow(1.0f);
    content->setJustifyContent(brls::JustifyContent::CENTER);
    content->setMaxWidth(880);
    this->addView(content);

    this->step = new brls::Label();
    this->step->setFontSize(20);
    this->step->setTextColor(stremio_theme::ACCENT_HI);
    this->step->setMarginBottom(14);
    content->addView(this->step);

    this->title = new brls::Label();
    this->title->setFontSize(44);
    this->title->setTextColor(stremio_theme::TEXT);
    this->title->setIsWrapping(true);
    this->title->setMarginBottom(22);
    content->addView(this->title);

    this->body = new brls::Label();
    this->body->setFontSize(25);
    this->body->setTextColor(stremio_theme::TEXT_DIM);
    this->body->setIsWrapping(true);
    this->body->setMarginBottom(24);
    content->addView(this->body);

    this->hint = new brls::Label();
    this->hint->setFontSize(22);
    this->hint->setTextColor(stremio_theme::TEXT);
    this->hint->setIsWrapping(true);
    content->addView(this->hint);

    this->actionBar = new ActionBar();
    this->addView(this->actionBar);

    this->registerAction("Next", brls::BUTTON_A, [this](brls::View*) {
        this->next();
        return true;
    });
    this->registerAction("Back", brls::BUTTON_B, [this](brls::View*) {
        this->previous();
        return true;
    });
    this->registerAction("QR Login", brls::BUTTON_X, [this](brls::View*) {
        if (this->page != 2) return false;
        this->openQrLogin();
        return true;
    });
    this->registerAction("Manual URL", brls::BUTTON_Y, [this](brls::View*) {
        if (this->page != 2) return false;
        this->openManualAddonPrompt();
        return true;
    });

    this->render();
}

void StremioOnboarding::draw(
    NVGcontext* vg, float x, float y, float width, float height, brls::Style style, brls::FrameContext* ctx) {
    stremio_theme::drawOceanBackground(vg, x, y, width, height);
    brls::Box::draw(vg, x, y, width, height, style, ctx);
}

void StremioOnboarding::render() {
    const auto& p = PAGES[this->page];
    this->step->setText(p.step);
    this->title->setText(p.title);
    this->body->setText(p.body);
    this->hint->setText(p.hint);

    std::vector<ActionBar::Item> actions;
    if (this->page > 0) actions.push_back({brls::BUTTON_B, "Back"});
    if (this->page == 2) {
        actions.push_back({brls::BUTTON_X, "QR Login"});
        actions.push_back({brls::BUTTON_Y, "Manual URL"});
    }
    actions.push_back({brls::BUTTON_A, this->page + 1 == PAGES.size() ? "Done" : "Next"});
    this->actionBar->setActions(actions);
}

void StremioOnboarding::next() {
    if (this->page + 1 == PAGES.size()) {
        this->complete();
        return;
    }
    this->page++;
    this->render();
}

void StremioOnboarding::previous() {
    if (this->page == 0) {
        brls::Application::popActivity();
        return;
    }
    this->page--;
    this->render();
}

void StremioOnboarding::openQrLogin() {
    brls::Application::pushActivity(new brls::Activity(new StremioLogin()));
}

void StremioOnboarding::openManualAddonPrompt() {
    brls::Application::getImeManager()->openForText(
        [](const std::string& text) {
            std::string url = stremio::normalizeAddonUrl(text);
            if (url.empty()) {
                brls::Application::notify("No addon saved");
                return;
            }
            stremio::saveAddon(AppConfig::instance().configDir(), url);
            brls::Application::notify("Stream addon saved");
        },
        "Stream addon URL",
        "Paste your addon URL, or put it in switch/ReelNX/reelnx-addon.txt and relaunch.",
        1024, stremio::STREAM_ADDON, 0);
}

void StremioOnboarding::complete() {
    AppConfig::instance().setItem(AppConfig::STREMIO_ONBOARDING_DONE, true);
    brls::Application::popActivity();
}
