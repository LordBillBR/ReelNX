#include <borealis.hpp>
#ifdef __SWITCH__
#include <switch.h>
#include <filesystem>
#elif defined(__PSV__)
#include <psp2/vshbridge.h>
#elif defined(ANDROID)
#include <SDL2/SDL.h>
#include <jni.h>
#elif defined(__APPLE__)
#include <SystemConfiguration/SystemConfiguration.h>
#elif defined(__linux__)
#include <borealis/platforms/desktop/steam_deck.hpp>
#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "utils/config.hpp"
#include "utils/dialog.hpp"
#include "utils/thread.hpp"
#include "api/http.hpp"

using namespace brls::literals;

std::string AppVersion::getVersion() { return APP_VERSION; }

std::string AppVersion::getPackageName() { return BUILD_PACKAGE_NAME; }

std::string AppVersion::getCommit() { return BUILD_TAG_SHORT; }

std::string AppVersion::getPlatform() {
#ifdef __SWITCH__
    return "NX";
#elif defined(__PSV__)
    return "PSVita";
#elif defined(__PS4__)
    return "PS4";
#elif defined(ANDROID)
    return "Android";
#elif defined(__APPLE__)
    return "macOS";
#elif defined(__linux__)
    if (getenv("SteamDeck")) return "SteamDeck";
    return "Linux";
#elif defined(_WIN32)
#if defined(_M_ARM64)
    return "Windows-arm64";
#else
    return "Windows";
#endif
#else
#error "Unsupport platform"
#endif
}

std::string AppVersion::getDeviceName() {
#ifdef __SWITCH__
    SetSysDeviceNickName nick;
    if (R_SUCCEEDED(setsysGetDeviceNickname(&nick))) {
        return nick.nickname;
    }
#elif defined(__PSV__)
    if (vshSblAimgrIsGenuineDolce()) {
        return "PSTV";
    } else if (vshSblAimgrIsGenuineVITA()) {
        char cid[0x20];
        if (_vshSblAimgrGetConsoleId(cid) >= 0) {
            if (cid[7] == 0x14 || cid[7] == 0x18) {
                return "PSVita Slim";
            }
        }
        return "PSVita";
    }
#elif defined(ANDROID)
    JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    jclass clazz = env->FindClass("android/os/Build");
    if (clazz) {
        jfieldID fid = env->GetStaticFieldID(clazz, "MODEL", "Ljava/lang/String;");
        jstring jname = (jstring)env->GetStaticObjectField(clazz, fid);
        const char* name = env->GetStringUTFChars(jname, nullptr);
        std::string device_name = name;
        env->ReleaseStringUTFChars(jname, name);
        env->DeleteLocalRef(jname);
        env->DeleteLocalRef(clazz);
        return device_name;
    }
#elif defined(_WIN32)
    DWORD bufsize = MAX_PATH;
    std::wstring buf(bufsize, '\0');
    if (GetComputerNameW(buf.data(), &bufsize)) {
        std::string name(bufsize * 3, '\0');
        WideCharToMultiByte(CP_UTF8, 0, buf.data(), bufsize, name.data(), name.size(), nullptr, nullptr);
        return name.data();
    }
#elif defined(__APPLE__)
    CFStringRef nameRef = SCDynamicStoreCopyComputerName(nullptr, nullptr);
    if (nameRef) {
        std::vector<char> name(CFStringGetLength(nameRef) * 3);
        CFStringGetCString(nameRef, name.data(), name.size(), kCFStringEncodingUTF8);
        CFRelease(nameRef);
        return name.data();
    }
#elif defined(__linux__)
    char name[256];
    if (!gethostname(name, sizeof(name))) {
        return name;
    }
#endif
    return fmt::format("{} for {}", getPackageName(), getPlatform());
}

bool AppVersion::needUpdate(std::string latestVersion) { return false; }

void AppVersion::checkUpdate(int delay, bool showUpToDateDialog) {
    // In-app auto-update is disabled for ReelNX. CI builds artifacts, and
    // production releases are expected to come from signed/tagged releases.
    if (showUpToDateDialog) brls::sync([]() { Dialog::show("main/setting/others/up2date"_i18n); });
}
