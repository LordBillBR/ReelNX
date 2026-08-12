#include "activity/stremio_login.hpp"

#include "api/stremio_account.hpp"
#include "utils/config.hpp"

#include <stb_image.h>

#include <chrono>
#include <thread>

StremioLogin::StremioLogin() {
    this->setAxis(brls::Axis::COLUMN);
    this->setDimensions(brls::Application::contentWidth, brls::Application::contentHeight);
    this->setBackgroundColor(nvgRGB(16, 18, 24));
    this->setPadding(48, 80, 48, 80);
    this->setAlignItems(brls::AlignItems::CENTER);

    auto* title = new brls::Label();
    title->setText("Login no Stremio");
    title->setFontSize(34);
    title->setMarginBottom(18);
    this->addView(title);

    this->status = new brls::Label();
    this->status->setText("Gerando QR code...");
    this->status->setFontSize(22);
    this->status->setHorizontalAlign(brls::HorizontalAlign::CENTER);
    this->status->setMarginBottom(24);
    this->addView(this->status);

    this->qr = new brls::Image();
    this->qr->setWidth(280);
    this->qr->setHeight(280);
    this->qr->setMarginBottom(24);
    this->addView(this->qr);

    this->link = new brls::Label();
    this->link->setFontSize(20);
    this->link->setHorizontalAlign(brls::HorizontalAlign::CENTER);
    this->addView(this->link);

    this->registerAction("Voltar", brls::BUTTON_B, [](brls::View*) {
        brls::Application::popActivity();
        return true;
    });
    this->begin();
}

StremioLogin::~StremioLogin() { this->alive->store(false); }

void StremioLogin::begin() {
    auto alive = this->alive;
    stremio::createLoginQr(
        [this, alive](stremio::LoginQr result) {
            if (!alive->load()) return;
            this->code = result.code;
            this->link->setText(result.link + "\nCodigo: " + result.code);
            stremio::downloadLoginQr(
                result.qrcode,
                [this, alive](std::string png) {
                    if (!alive->load()) return;
                    int width = 0;
                    int height = 0;
                    int channels = 0;
                    auto* rgba = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(png.data()),
                        static_cast<int>(png.size()), &width, &height, &channels, 4);
                    if (!rgba) {
                        this->status->setText("Erro ao decodificar QR code");
                        return;
                    }
                    int texture = nvgCreateImageRGBA(brls::Application::getNVGContext(), width, height, 0, rgba);
                    stbi_image_free(rgba);
                    if (texture <= 0) {
                        this->status->setText("Erro ao criar textura do QR code");
                        return;
                    }
                    this->qr->innerSetImage(texture);
                    this->status->setText("Escaneie o QR com o app Stremio e aprove o login.");
                    this->poll(0);
                },
                [this, alive](const std::string& error) {
                    if (alive->load()) this->status->setText("Erro ao carregar QR: " + error);
                });
        },
        [this, alive](const std::string& error) {
            if (alive->load()) this->status->setText("Erro ao gerar QR: " + error);
        });
}

void StremioLogin::poll(int attempt) {
    if (this->code.empty() || !this->alive->load()) return;
    auto alive = this->alive;
    stremio::pollLoginQr(
        this->code,
        [this, alive](std::string authKey) {
            if (!alive->load()) return;
            this->status->setText("Login aprovado. Sincronizando addons...");
            stremio::syncAccountAddons(
                AppConfig::instance().configDir(), authKey,
                [this, alive](size_t) {
                    if (!alive->load()) return;
                    brls::Application::notify("Addons do Stremio sincronizados");
                    brls::Application::popActivity();
                },
                [this, alive](const std::string& error) {
                    if (alive->load()) this->status->setText("Erro ao sincronizar addons: " + error);
                });
        },
        [this, attempt, alive](const std::string&) {
            if (!alive->load() || attempt >= 60) return;
            brls::async([this, attempt, alive]() {
                std::this_thread::sleep_for(std::chrono::seconds(3));
                if (alive->load())
                    brls::sync([this, attempt, alive]() {
                        if (alive->load()) this->poll(attempt + 1);
                    });
            });
        });
}
