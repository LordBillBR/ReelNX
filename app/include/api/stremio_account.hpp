#pragma once

#include "api/http.hpp"
#include "api/stremio.hpp"

#include <borealis/core/thread.hpp>
#include <nlohmann/json.hpp>

#include <functional>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

namespace stremio {

struct LoginQr {
    std::string code;
    std::string link;
    std::string qrcode;
};

inline std::string accountString(const nlohmann::json& j, const char* key) {
    auto it = j.find(key);
    return it != j.end() && it->is_string() ? it->get<std::string>() : std::string();
}

inline nlohmann::json accountResult(const std::string& body) {
    auto j = nlohmann::json::parse(body);
    if (j.contains("error")) {
        auto message = j["error"].is_object() ? accountString(j["error"], "message") : j["error"].dump();
        throw std::runtime_error(message.empty() ? "Stremio API error" : message);
    }
    if (!j.contains("result")) throw std::runtime_error("Stremio API returned no result");
    return j["result"];
}

inline void createLoginQr(std::function<void(LoginQr)> then, OnError error) {
    brls::async([then, error]() {
        try {
            auto result =
                accountResult(HTTP::get("https://link.stremio.com/api/v2/create?type=Create", HTTP::Timeout{20000}));
            LoginQr qr{accountString(result, "code"), accountString(result, "link"), accountString(result, "qrcode")};
            if (qr.code.empty() || qr.qrcode.empty()) throw std::runtime_error("invalid QR response");
            brls::sync([then, qr]() { then(qr); });
        } catch (const std::exception& ex) {
            if (error) brls::sync([error, message = std::string(ex.what())]() { error(message); });
        }
    });
}

inline void downloadLoginQr(const std::string& url, std::function<void(std::string)> then, OnError error) {
    brls::async([url, then, error]() {
        try {
            auto png = HTTP::get(url, HTTP::Timeout{20000});
            if (png.empty()) throw std::runtime_error("empty QR image");
            brls::sync([then, png = std::move(png)]() { then(png); });
        } catch (const std::exception& ex) {
            if (error) brls::sync([error, message = std::string(ex.what())]() { error(message); });
        }
    });
}

inline void pollLoginQr(const std::string& code, std::function<void(std::string)> then, OnError error) {
    brls::async([code, then, error]() {
        try {
            auto result = accountResult(
                HTTP::get("https://link.stremio.com/api/v2/read?type=Read&code=" + code, HTTP::Timeout{20000}));
            auto authKey = accountString(result, "authKey");
            if (authKey.empty()) authKey = accountString(result, "auth_key");
            if (authKey.empty()) throw std::runtime_error("waiting");
            brls::sync([then, authKey]() { then(authKey); });
        } catch (const std::exception& ex) {
            if (error) brls::sync([error, message = std::string(ex.what())]() { error(message); });
        }
    });
}

inline void syncAccountAddons(
    const std::string& configDir, const std::string& authKey, std::function<void(size_t)> then, OnError error) {
    brls::async([configDir, authKey, then, error]() {
        try {
            nlohmann::json request = {{"type", "AddonCollectionGet"}, {"authKey", authKey}, {"update", true}};
            auto result = accountResult(HTTP::post("https://api.strem.io/api/addonCollectionGet", request.dump(),
                HTTP::Header{"Content-Type: application/json"}, HTTP::Timeout{30000}));
            std::vector<std::string> streamAddons;
            std::vector<std::string> subtitleAddons;
            size_t count = 0;
            for (auto& addon : result.value("addons", nlohmann::json::array())) {
                auto base = accountString(addon, "transportUrl");
                if (base.empty()) continue;
                auto manifest = addon.value("manifest", nlohmann::json::object());
                for (auto& resource : manifest.value("resources", nlohmann::json::array())) {
                    auto name = resource.is_string() ? resource.get<std::string>() : accountString(resource, "name");
                    if (name == "stream" &&
                        std::find(streamAddons.begin(), streamAddons.end(), base) == streamAddons.end())
                        streamAddons.push_back(base);
                    if (name == "subtitles" &&
                        std::find(subtitleAddons.begin(), subtitleAddons.end(), base) == subtitleAddons.end())
                        subtitleAddons.push_back(base);
                }
                if (!streamAddons.empty() || !subtitleAddons.empty()) ++count;
            }
            if (streamAddons.empty()) throw std::runtime_error("no stream addon in Stremio account");
            STREAM_ADDONS.clear();
            for (auto& addon : streamAddons) STREAM_ADDONS.push_back(normalizeAddonUrl(addon));
            STREAM_ADDON = STREAM_ADDONS.front();
            SUBTITLES_ADDONS.clear();
            for (auto& addon : subtitleAddons) SUBTITLES_ADDONS.push_back(normalizeAddonUrl(addon));
            SUBTITLES_ADDON = SUBTITLES_ADDONS.empty() ? "" : SUBTITLES_ADDONS.front();
            saveConfig(configDir);
            brls::sync([then, count]() { then(count); });
        } catch (const std::exception& ex) {
            if (error) brls::sync([error, message = std::string(ex.what())]() { error(message); });
        }
    });
}

}  // namespace stremio
