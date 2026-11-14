#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"
cd -- "$SCRIPT_DIR/../.."
source "$SCRIPT_DIR/config.txt"

if command -v root >/dev/null 2>&1; then
    ROOT_COMMAND=(root)
elif [[ -x /projects/hep/fs11/share/mnxb11/bin/startmnxb11container.sh ]]; then
    ROOT_COMMAND=(/projects/hep/fs11/share/mnxb11/bin/startmnxb11container.sh root)
else
    printf 'ROOT is unavailable. Run this script in the MNXB11 environment.\n' >&2
    exit 1
fi

# Keep the generated input list out of the tracked files.
file_list=$(mktemp "$SCRIPT_DIR/.files.XXXXXX")
trap 'rm -f -- "$file_list"' EXIT
printf '%s\n' "${FILES[@]}" > "$file_list"

"${ROOT_COMMAND[@]}" -l -b <<ROOT_INPUT
if (!gSystem->CompileMacro("src/climate_class.cxx", "k")) gSystem->Exit(1);
if (!gSystem->CompileMacro("src/file_processing.cxx", "k")) gSystem->Exit(1);
{
    Int_t error = 0;
    gROOT->ProcessLine(".x scripts/csv_to_ttree/createTTree.C(\\"$file_list\\")", &error);
    if (error != 0) gSystem->Exit(1);
}
.q
ROOT_INPUT
