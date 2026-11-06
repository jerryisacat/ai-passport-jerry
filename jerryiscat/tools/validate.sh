#!/usr/bin/env bash
# Host tests for fork logic. Firmware build and device checks are separate gates.
set -euo pipefail

repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"
build_dir="$(mktemp -d /tmp/jerryiscat-host-tests.XXXXXX)"
trap 'rm -rf -- "${build_dir}"' EXIT
cc="${CC:-cc}"

"${cc}" -std=c11 -Wall -Wextra -Werror \
    -I"${repo_root}/jerryiscat/src" \
    "${repo_root}/jerryiscat/tests/test_jerryiscat_nav.c" \
    "${repo_root}/jerryiscat/src/jerryiscat_nav.c" \
    -o "${build_dir}/test_jerryiscat_nav"
"${build_dir}/test_jerryiscat_nav"

echo "Fork host tests: PASS"
