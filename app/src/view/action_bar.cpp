#include "view/action_bar.hpp"
#include "view/stremio_theme.hpp"

#include <borealis/views/hint.hpp>

ActionBar::ActionBar() {
    this->setAxis(brls::Axis::ROW);
    this->setAlignItems(brls::AlignItems::CENTER);
    this->setJustifyContent(brls::JustifyContent::CENTER);
    this->setHeight(58);
    this->setPadding(8, 34, 8, 34);
    this->setBackgroundColor(nvgRGBA(4, 7, 18, 218));
    this->setLineTop(1);
    this->setLineColor(nvgRGBA(255, 255, 255, 28));
    this->setFocusable(false);
}

void ActionBar::setActions(const std::vector<Item>& actions) {
    this->clearViews(true);

    for (const auto& action : actions) {
        auto* item = new brls::Box();
        item->setAxis(brls::Axis::ROW);
        item->setAlignItems(brls::AlignItems::CENTER);
        item->setMarginLeft(13);
        item->setMarginRight(13);
        item->setPadding(5, 10, 5, 10);
        item->setCornerRadius(12);
        item->setBackgroundColor(nvgRGBA(255, 255, 255, 16));
        item->setFocusable(false);

        auto* key = new brls::Label();
        key->setText(brls::Hint::getKeyIcon(action.button));
        key->setFontSize(24);
        key->setTextColor(stremio_theme::TEXT);
        item->addView(key);

        auto* label = new brls::Label();
        label->setText(action.label);
        label->setFontSize(20);
        label->setTextColor(stremio_theme::TEXT_DIM);
        label->setMarginLeft(7);
        item->addView(label);

        this->addView(item);
    }
}
