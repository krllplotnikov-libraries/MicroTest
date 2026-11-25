#!/bin/sh
# Installs MicroTest on Linux.
#   ./install.sh                 -> /usr/local (asks for sudo)
#   ./install.sh ~/.local        -> any other prefix
#   ./install.sh --uninstall     -> removes the installed files
set -e

cd "$(dirname "$0")"

if [ "$1" = "--uninstall" ]; then
	if [ -w "$(head -n 1 build/install_manifest.txt 2>/dev/null)" ]; then
		cmake --build build --target uninstall
	else
		sudo cmake --build build --target uninstall
	fi
	exit 0
fi

PREFIX="${1:-/usr/local}"

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$PREFIX"
cmake --build build

mkdir -p "$PREFIX" 2>/dev/null || true
if [ -w "$PREFIX" ]; then
	cmake --install build
else
	sudo cmake --install build
fi

echo "MicroTest installed to $PREFIX"
