#!/bin/bash

GITROOT=$(git rev-parse --show-toplevel)

if [ -d $GITROOT ]; then
     cd $GITROOT
   else
     echo "not a git repository, exiting..."
     exit 1
fi

SCRIPT_DIR="$(dirname "${BASH_SOURCE[0]}")"

source "$SCRIPT_DIR/config.txt"
printf '%s\n' "${FILES[@]}" > "$SCRIPT_DIR/files.txt"

/projects/hep/fs11/share/mnxb11/bin/startmnxb11container.sh root <<ROOT_INPUT
.L src/climate_class.cxx+
.L src/file_processing.cxx+
.L scripts/csv_to_ttree/createTTree.C
createTTree("$SCRIPT_DIR/files.txt");
.q
ROOT_INPUT

exit