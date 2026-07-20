#!/bin/bash

echo "🧹 Limpiando..."
pkgman uninstall -y dusk 2>/dev/null
rmmime application/x-vnd.Dusk 2>/dev/null
make clean
rm -rf dusk_package *.hpkg

echo "🔧 Compilando Dusk..."
# El Makefile va a leer automáticamente res/Dusk.rdef y meter el ícono
make -j$(nproc)

BIN_PATH=$(find . -type f -name "Dusk" ! -path "*/dusk_package/*" | head -n 1)

if [ -z "$BIN_PATH" ]; then
    echo "❌ Error: No se pudo encontrar el binario compilado."
    exit 1
fi

echo "📁 Armando paquete..."
PKG_DIR="dusk_package"
mkdir -p "$PKG_DIR/apps"
cp -a "$BIN_PATH" "$PKG_DIR/apps/Dusk"

cat <<EOF > "$PKG_DIR/.PackageInfo"
name            dusk
version         1.0.0-1
architecture    x86_64
summary         "Night Shift Haiku"
description     "Blue light filter designed natively for Haiku."
packager        "Nahuel Tello <ntello@upe.edu.ar>"
vendor          "Unarix"
licenses        { "MIT" }
copyrights      { "Copyright 2026 Tu Nombre" }
provides {
    dusk = 1.0.0-1
}
requires {
    haiku >= r1~beta4
}
EOF

echo "📦 Generando archivo .hpkg..."
package create -C "$PKG_DIR" dusk-1.0.0-1-x86_64.hpkg
