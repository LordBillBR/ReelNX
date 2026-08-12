#pragma once

#include <borealis.hpp>
#include <vector>

class ActionBar : public brls::Box {
public:
    struct Item {
        brls::ControllerButton button;
        std::string label;
    };

    ActionBar();

    void setActions(const std::vector<Item>& actions);
};
