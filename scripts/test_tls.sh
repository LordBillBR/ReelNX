#!/usr/bin/env bash

set -eu

root_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
ca_bundle="$root_dir/resources/certs/ca-bundle.crt"

if ! command -v curl >/dev/null 2>&1; then
    printf 'curl is required\n' >&2
    exit 2
fi

if [[ ! -s "$ca_bundle" ]]; then
    printf 'CA bundle not found: %s\n' "$ca_bundle" >&2
    exit 2
fi

curl_args=(--silent --show-error --fail --location --cacert "$ca_bundle" --connect-timeout 10 --max-time 30)

printf 'Checking a valid certificate...\n'
curl "${curl_args[@]}" https://example.com >/dev/null

printf 'Checking an expired certificate is rejected...\n'
if curl "${curl_args[@]}" https://expired.badssl.com/ >/dev/null 2>&1; then
    printf 'expired.badssl.com was accepted\n' >&2
    exit 1
fi

printf 'Checking a hostname mismatch is rejected...\n'
if curl "${curl_args[@]}" https://wrong.host.badssl.com/ >/dev/null 2>&1; then
    printf 'wrong.host.badssl.com was accepted\n' >&2
    exit 1
fi

printf 'TLS validation checks passed\n'
