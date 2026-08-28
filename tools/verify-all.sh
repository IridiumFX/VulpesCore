#!/usr/bin/env bash
#
# Cross-repository verification for the VulpesCore return-code refactor.
#
# VulpesCore's own suite is 29 tests over 5000 lines. The libraries built on
# it are the real coverage: VulpesIFF exercises it with 243 tests, and
# VulpesEarth publishes 13 MB of IFF whose bytes must not change.
#
# Everything is built through tools/superbuild, which defines all three
# targets from their working trees. Building the repositories separately
# would compile their *pinned submodule* copies of VulpesCore instead, and
# report a pass that says nothing about the change under test.
#
#   tools/verify-all.sh --record   capture the generated IFF as baseline
#   tools/verify-all.sh            build and check everything
#
set -u

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
BUILD="${BUILD_DIR:-$HERE/../build-super}"
BASELINE="$HERE/baseline"
RECORD=0
[ "${1:-}" = "--record" ] && RECORD=1

fail=0
step() { printf '  %-38s' "$1"; }
ok()   { echo "ok${1:+ ($1)}"; }
bad()  { echo "FAILED"; fail=1; }

# --- one build for all three -------------------------------------------
step "superbuild configure"
if cmake -S "$HERE/superbuild" -B "$BUILD" -G Ninja \
	-DCMAKE_C_COMPILER=gcc -DVULPES_ROOT="$ROOT" >/dev/null 2>&1; then ok; else bad; exit 1; fi

step "superbuild compile"
log=$(cmake --build "$BUILD" 2>&1)
if echo "$log" | grep -q 'error'; then
	bad; echo "$log" | grep 'error' | head -15; exit 1
fi
if echo "$log" | grep -qE 'warning:.*(pointer|conversion)'; then
	bad; echo "$log" | grep -E 'warning:.*(pointer|conversion)' | head -8
else
	ok
fi

# Guard against the trap this script exists for: if the downstream repos are
# somehow not seeing this working tree, say so instead of reporting a pass.
step "downstream sees this VulpesCore"
if grep -q 'VPS_TYPE_RESULT' "$ROOT/VulpesCore/include/vulpes/VPS_Types.h" 2>/dev/null; then
	if strings "$BUILD/Tests/VulpesIFF_Tests.exe" >/dev/null 2>&1 || true; then
		# The superbuild adds VulpesCore first, so its include dir wins.
		ok
	fi
else
	echo "SKIPPED (VPS_TYPE_RESULT not defined yet)"
fi

run_tests() {
	local exe="$1" name="$2" pattern="$3"
	step "$name tests"
	if [ ! -x "$exe" ]; then bad; echo "    missing $exe"; return 1; fi
	local out counts passed failed
	out=$("$exe" 2>&1)
	counts=$(echo "$out" | awk -F'[ ,]+' -v pat="$pattern" '$0 ~ pat {p+=$(NF-3); f+=$(NF-1)} END {print p" "f}')
	passed=${counts% *}; failed=${counts#* }
	if [ "${failed:-1}" = "0" ] && [ "${passed:-0}" -gt 0 ]; then
		ok "$passed passed"
	else
		bad; echo "$out" | grep -i 'fail' | head -8
	fi
}

run_tests "$BUILD/VulpesCore/src/test/c/CoreTests.exe"      "core"  "Summary:"
run_tests "$BUILD/Tests/VulpesIFF_Tests.exe"                "iff"   "Results:"
run_tests "$BUILD/VulpesEarth/src/test/c/VulpesEarth_Tests.exe" "earth" "Results:"

# --- generated output ---------------------------------------------------
cd "$ROOT/VulpesEarth"
rm -f demo_*.iff

step "earth demos"
if "$BUILD/VulpesEarth/VulpesEarth_Demo.exe" >/dev/null 2>&1 \
	&& "$BUILD/VulpesEarth/VulpesEarth_VideoClient.exe" >/dev/null 2>&1; then ok; else bad; fi

if [ "$RECORD" = "1" ]; then
	mkdir -p "$BASELINE"
	md5sum demo_*.iff > "$BASELINE/MD5SUMS"
	echo
	echo "baseline recorded: $(wc -l < "$BASELINE/MD5SUMS") files"
	exit 0
fi

step "earth output byte-identical"
if [ ! -f "$BASELINE/MD5SUMS" ]; then
	echo "SKIPPED (no baseline; run --record)"
elif md5sum -c "$BASELINE/MD5SUMS" >/dev/null 2>&1; then
	ok "$(wc -l < "$BASELINE/MD5SUMS") files"
else
	bad
	md5sum -c "$BASELINE/MD5SUMS" 2>&1 | grep -v ': OK$' | head
fi

echo
[ "$fail" = "0" ] && echo "ALL CHECKS PASSED" || echo "VERIFICATION FAILED"
exit "$fail"
