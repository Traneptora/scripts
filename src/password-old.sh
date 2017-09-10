#!/bin/sh
set -e

echo -n "Enter domain: "
read -r domain
echo -n "Enter username: "
read -r username
echo -n "Enter index: "
read -r index
echo -n "Enter master password: "
MHASH="$(sha256sum | cut -f1 -d' ' | perl -lne 'print pack "H*", $_' | base64)"

PHASH=$(printf "%s%s%s%s" "$domain" "$username" "$index" "$MHASH" | sha256sum | cut -f1 -d' ' | perl -lne 'print pack "H*", $_' | base64)

printf "%s%s\n" "Df!1@2" "$PHASH"
