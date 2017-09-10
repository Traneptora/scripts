#!/bin/sh
set -e

SHA3_256SUM="$(command -v sha3-256sum || printf '%s -a 256' "$(command -v sha3sum)")"

STORED_MHASH=/tmp/stored_mhash

if [ -e "$STORED_MHASH" ] ; then
	MHASH_AGE=$(perl -e 'print -M $ARGV[0]' "$STORED_MHASH")
	if [ $(printf '%s > 0.25\n' $MHASH_AGE | bc) = "1" ] ; then
		rm -f "$STORED_MHASH"
	else
		MHASH="$(xz -d --format=raw --lzma1=dict=8MiB,lc=3,lp=0,pb=2,mode=normal,nice=64,mf=bt4,depth=0 <"$STORED_MHASH")"
	fi
fi

echo -n "Enter domain: "
read -r domain
echo -n "Enter username [leo.izen@gmail.com]: "
read -r username
if [ -z "$username" ] ; then
	username="leo.izen@gmail.com"
fi
echo -n "Enter index [0]: "
read -r index
if [ -z "$index" ] ; then
	index=0
fi
echo -n "Enter max password length [50]: "
read -r maxlength
if [ -z "$maxlength" ] ; then
	maxlength=50
elif [ ! "$maxlength" -ge 8 -o ! "$maxlength" -le 50 ] ; then
	echo "Invalid maxlength: $maxlength" >&2
	exit 1
fi

if [ -z "$MHASH" ] ; then
	echo -n "Enter master password: "
	MHASH="$(cat | $SHA3_256SUM | cut -f1 -d' ' | perl -lne 'print pack "H*", $_' | base64)"
	printf "%s" "$MHASH" | xz -z --format=raw --lzma1=dict=8MiB,lc=3,lp=0,pb=2,mode=normal,nice=64,mf=bt4,depth=0 >"$STORED_MHASH"
fi
# Yes, the "cat |" is really necessary. Windows implemenation :P


PHASH=$(printf "%s%s%s%s%s" "$domain" "$username" "$index" "$MHASH" "$maxlength" | $SHA3_256SUM | cut -f1 -d' ' | perl -lne 'print pack "H*", $_' | base64)

printf "%s%s" "Df!1@2" "$PHASH" | head -c "$maxlength"
echo
