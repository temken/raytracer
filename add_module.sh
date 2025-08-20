#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage:
  ./add_module.sh [-n NAMESPACE] [-f] MODULE_NAME        # create
  ./add_module.sh -d MODULE_NAME                          # delete

Creates:
  include/MODULE_NAME.hpp
  src/MODULE_NAME.cpp
  tests/test_MODULE_NAME.cpp

Options:
  -n NAMESPACE   Wrap class in a namespace (supports nested like foo::bar).
  -f             Overwrite existing files if they already exist.
  -d             Delete the module files (prompts Y/n).
  -h             Show this help.

Behavior:
  - Create: generates header/source/test; `git add`s them if in a repo.
  - Delete: shows the three paths, asks to confirm, then removes files.
  - Run from project root (expects ./include, ./src, ./tests).
USAGE
}

# ---- arg parsing ----
NAMESPACE=""
FORCE=0
DELETE=0

while getopts ":n:fdh" opt; do
  case "$opt" in
    n) NAMESPACE="$OPTARG" ;;
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

# Basic C++ identifier check for class name
if [[ ! "$MODULE" =~ ^[A-Za-z_][A-Za-z0-9_]*$ ]]; then
  echo "Error: MODULE_NAME '$MODULE' is not a valid C++ identifier." >&2
  exit 1
fi

HDR_PATH="include/${MODULE}.hpp"
SRC_PATH="src/${MODULE}.cpp"
TEST_PATH="tests/test_${MODULE}.cpp"

# ---- helper: namespace open/close ----
ns_open() {
  local ns="$1"
  [[ -z "$ns" ]] && return 0
  IFS=':' read -ra parts <<< "${ns//::/:}"
  local fixed=()
  for p in "${parts[@]}"; do
    [[ -n "$p" ]] && fixed+=("$p")
  done
  for p in "${fixed[@]}"; do
    echo "namespace $p {"
  done
}

ns_close() {
  local ns="$1"
  [[ -z "$ns" ]] && return 0
  IFS=':' read -ra parts <<< "${ns//::/:}"
  local fixed=()
  for p in "${parts[@]}"; do
    [[ -n "$p" ]] && fixed+=("$p")
  done
  for ((i=${#fixed[@]}-1; i>=0; i--)); do
    echo "} // namespace ${fixed[i]}"
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
for dir in include src tests; do
  if [[ ! -d "$dir" ]]; then
    echo "Error: missing directory ./$dir" >&2
    exit 1
  fi
done

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

# ---- generate source (.cpp) ----
{
  echo "#include \"${MODULE}.hpp\""
  echo
  ns_open "$NAMESPACE"
  [[ -n "$NAMESPACE" ]] && echo
  # Implementation goes here, e.g.:
  # ReturnType ${MODULE}::Method(...) { ... }
  [[ -n "$NAMESPACE" ]] && ns_close "$NAMESPACE"
  echo
} > "$SRC_PATH"

# ---- generate test ----
{
  echo "#include \"gtest/gtest.h\""
  echo
  echo "#include \"${MODULE}.hpp\""
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

# ---- git add if applicable ----
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