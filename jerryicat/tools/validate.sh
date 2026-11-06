#!/usr/bin/env bash
# Host tests for fork logic. Firmware build and device checks are separate gates.
set -euo pipefail

repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"
build_dir="$(mktemp -d /tmp/jerryicat-host-tests.XXXXXX)"
trap 'rm -rf -- "${build_dir}"' EXIT
cc="${CC:-cc}"

"${cc}" -std=c11 -Wall -Wextra -Werror \
    -I"${repo_root}/jerryicat/src" \
    "${repo_root}/jerryicat/tests/test_jerryicat_nav.c" \
    "${repo_root}/jerryicat/src/jerryicat_nav.c" \
    -o "${build_dir}/test_jerryicat_nav"
"${build_dir}/test_jerryicat_nav"

"${cc}" -std=c11 -Wall -Wextra -Werror \
    -I"${repo_root}/jerryicat/plays/plus-one" \
    "${repo_root}/jerryicat/tests/test_plus_one_logic.c" \
    -o "${build_dir}/test_plus_one_logic"
"${build_dir}/test_plus_one_logic"

echo "Fork host tests: PASS"
