#pragma once

#include <borealis.hpp>

#include <atomic>
#include <memory>

class StremioLogin : public brls::Box {
public:
    StremioLogin();
    ~StremioLogin() override;

private:
    void begin();
    void poll(int attempt);

    brls::Label* status = nullptr;
    brls::Label* link = nullptr;
    brls::Image* qr = nullptr;
    std::string code;
    std::shared_ptr<std::atomic_bool> alive = std::make_shared<std::atomic_bool>(true);
};
