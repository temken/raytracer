#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage:
  ./add_module.sh [-n NAMESPACE] [-s SUBPATH] [-f] MODULE_NAME        # create
  ./add_module.sh -d [-s SUBPATH] MODULE_NAME                          # delete

Creates:
  src/[SUBPATH/]MODULE_NAME.hpp
  src/[SUBPATH/]MODULE_NAME.cpp
  tests/[SUBPATH/]test_MODULE_NAME.cpp

Options:
  -n NAMESPACE   Wrap class in a namespace (supports nested like foo::bar).
  -s SUBPATH     Optional subfolder (e.g. render, utils/math).
  -f             Overwrite existing files if they already exist.
  -d             Delete the module files (prompts Y/n).
  -h             Show this help.

Behavior:
  - Create: generates header/source/test; `git add`s them if in a repo.
  - Delete: shows the paths, asks to confirm, then removes files.
  - Run from project root (expects ./src and ./tests).
USAGE
}

# ---- arg parsing ----
NAMESPACE=""
FORCE=0
DELETE=0
SUBPATH=""

while getopts ":n:s:fdh" opt; do
  case "$opt" in
    n) NAMESPACE="$OPTARG" ;;
    s) SUBPATH="$OPTARG" ;;
    f) FORCE=1 ;;
    d) DELETE=1 ;;
    h) usage; exit 0 ;;
    \?) echo "Error: Invalid option -$OPTARG" >&2; usage; exit 2 ;;
    :)  echo "Error: Option -$OPTARG requires an argument." >&2; usage; exit 2 ;;
  esac
done
shift $((OPTIND - 1))

if [[ $# -ne 1 ]]; then
  echo "Error: MODULE_NAME is required." >&2
  usage
  exit 2
fi

MODULE="$1"

# Basic C++ identifier check
if [[ ! "$MODULE" =~ ^[A-Za-z_][A-Za-z0-9_]*$ ]]; then
  echo "Error: MODULE_NAME '$MODULE' is not a valid C++ identifier." >&2
  exit 1
fi

SRC_DIR="src"
TEST_DIR="tests"

# Target dirs
SRC_TARGET="${SRC_DIR}${SUBPATH:+/${SUBPATH}}"
TEST_TARGET="${TEST_DIR}${SUBPATH:+/${SUBPATH}}"
mkdir -p "$SRC_TARGET" "$TEST_TARGET"

# Paths
HDR_PATH="${SRC_TARGET}/${MODULE}.hpp"
SRC_PATH="${SRC_TARGET}/${MODULE}.cpp"
TEST_PATH="${TEST_TARGET}/test_${MODULE}.cpp"
REL_HDR_PATH="${SUBPATH:+${SUBPATH}/}${MODULE}.hpp"

# ---- namespace helpers ----
ns_open() {
  local ns="$1"
  [[ -z "$ns" ]] && return
  IFS=':' read -ra parts <<< "${ns//::/:}"
  for p in "${parts[@]}"; do
    [[ -n "$p" ]] && echo "namespace $p {"
  done
}

ns_close() {
  local ns="$1"
  [[ -z "$ns" ]] && return
  IFS=':' read -ra parts <<< "${ns//::/:}"
  local arr=()
  for p in "${parts[@]}"; do
    [[ -n "$p" ]] && arr+=("$p")
  done
  for ((i=${#arr[@]}-1;i>=0;i--)); do
    echo "} // namespace ${arr[i]}"
  done
}

# ---- delete mode ----
if [[ $DELETE -eq 1 ]]; then
  echo "Do you really want to delete these files:"
  printf "  %s\n" "$HDR_PATH" "$SRC_PATH" "$TEST_PATH"
  read -r -p "Proceed? [Y/n] " REPLY
  REPLY=${REPLY:-Y}
  case "$REPLY" in
    [yY]|[yY][eE][sS]) ;;
    *) echo "Aborted."; exit 0 ;;
  esac

  in_repo=false
  if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    in_repo=true
  fi

  remove_file() {
    local f="$1"
    if [[ -e "$f" ]]; then
      if $in_repo && git ls-files --error-unmatch "$f" >/dev/null 2>&1; then
        git rm -f "$f"
      else
        rm -f "$f"
        $in_repo && git add -A >/dev/null 2>&1 || true
      fi
      echo "Deleted: $f"
    else
      echo "Note: $f does not exist; skipping."
    fi
  }

  remove_file "$HDR_PATH"
  remove_file "$SRC_PATH"
  remove_file "$TEST_PATH"
  exit 0
fi

# ---- create mode ----
if [[ $FORCE -eq 0 ]]; then
  for f in "$HDR_PATH" "$SRC_PATH" "$TEST_PATH"; do
    [[ -e "$f" ]] && { echo "Error: $f already exists. Use -f to overwrite."; exit 1; }
  done
fi

# ---- generate header ----
{
  echo "#pragma once"
  echo
  ns_open "$NAMESPACE"
  [[ -n "$NAMESPACE" ]] && echo
  echo "class $MODULE {"
  echo "public:"
  echo
  echo "private:"
  echo
  echo "};"
  [[ -n "$NAMESPACE" ]] && echo
  ns_close "$NAMESPACE"
  echo
} > "$HDR_PATH"

# ---- generate source ----
{
  echo "#include \"${REL_HDR_PATH}\""
  echo
  ns_open "$NAMESPACE"
  [[ -n "$NAMESPACE" ]] && echo
  # Implementation goes here
  [[ -n "$NAMESPACE" ]] && ns_close "$NAMESPACE"
  echo
} > "$SRC_PATH"

# ---- generate test ----
{
  echo "#include \"gtest/gtest.h\""
  echo
  echo "#include \"${REL_HDR_PATH}\""
  echo
  if [[ -n "$NAMESPACE" ]]; then
    echo "using namespace ${NAMESPACE};"
    echo
  fi
  echo "TEST(Test${MODULE}, Test1)"
  echo "{"
  echo "    // ARRANGE"
  echo "    // ACT"
  echo "    // ASSERT"
  echo "}"
} > "$TEST_PATH"

# ---- git add ----
if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
  git add "$HDR_PATH" "$SRC_PATH" "$TEST_PATH"
fi

echo "Created:"
echo "  $HDR_PATH"
echo "  $SRC_PATH"
echo "  $TEST_PATH"
if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
  echo "Staged all files with 'git add'."
fi