#!/usr/bin/env bash

set -eu

root_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
tmp_dir=$(mktemp -d)
trap 'rm -rf "$tmp_dir"' EXIT

src="$tmp_dir/retry_policy_test.cpp"
bin="$tmp_dir/retry_policy_test"

cat >"$src" <<'CPP'
#include "api/retry_policy.hpp"

#include <chrono>
#include <cstdlib>

using namespace std::chrono_literals;

static void require(bool condition) {
    if (!condition) std::exit(1);
}

int main() {
    require(!api::retryDecision(0, 3, 400, "").retry);
    require(!api::retryDecision(0, 3, 401, "").retry);
    require(!api::retryDecision(0, 3, 403, "").retry);
    require(!api::retryDecision(0, 3, 404, "").retry);

    auto rateLimited = api::retryDecision(0, 3, 429, "2");
    require(rateLimited.retry);
    require(rateLimited.delay == 2s);

    auto missingRetryAfter = api::retryDecision(0, 3, 429, "");
    require(missingRetryAfter.retry);
    require(missingRetryAfter.delay == 1s);

    auto serverError = api::retryDecision(1, 3, 503, "");
    require(serverError.retry);
    require(serverError.delay == 2s);

    auto transportError = api::retryDecision(0, 3, 0, "");
    require(transportError.retry);
    require(transportError.delay == 1s);

    require(!api::retryDecision(0, 3, 200, "").retry);
    require(!api::retryDecision(2, 3, 503, "").retry);
}
CPP

${CXX:-c++} -std=c++17 -I"$root_dir/app/include" "$src" -o "$bin"
"$bin"
printf 'Retry policy checks passed\n'
