#pragma once

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <string>

namespace api {

struct RetryDecision {
    bool retry = false;
    std::chrono::milliseconds delay{0};
};

inline bool isDeterministicClientError(long statusCode) {
    return statusCode == 400 || statusCode == 401 || statusCode == 403 || statusCode == 404;
}

inline bool isRetryableStatus(long statusCode) {
    return statusCode == 429 || (statusCode >= 500 && statusCode <= 599);
}

inline std::chrono::milliseconds parseRetryAfter(const std::string& retryAfter) {
    char* end = nullptr;
    long seconds = std::strtol(retryAfter.c_str(), &end, 10);
    if (end == retryAfter.c_str() || seconds < 0) return std::chrono::milliseconds{0};
    return std::chrono::seconds(std::min(seconds, 60L));
}

inline std::chrono::milliseconds exponentialBackoff(int attempt) {
    long seconds = 1L << std::min(attempt, 4);
    return std::chrono::seconds(seconds);
}

inline RetryDecision retryDecision(int attempt, int maxAttempts, long statusCode, const std::string& retryAfter) {
    if (attempt + 1 >= maxAttempts) return {};
    if (isDeterministicClientError(statusCode)) return {};
    if (statusCode == 429) {
        auto delay = parseRetryAfter(retryAfter);
        return {true, delay.count() > 0 ? delay : exponentialBackoff(attempt)};
    }
    if (statusCode == 0 || isRetryableStatus(statusCode)) return {true, exponentialBackoff(attempt)};
    return {};
}

}  // namespace api
