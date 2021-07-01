#!/bin/sh


if command -v sha3-256sum >/dev/null 2>/dev/null; then
    SHA3_256SUM="sha3-256sum"
elif command -v sha3sum >/dev/null 2>/dev/null; then
    SHA3_256SUM="sha3sum -a 256"
else
    printf '%s\n' "Cannot find sha3-256sum or sha3sum"
    exit 1
fi


STORED_MHASH="${STORED_MHASH:-/tmp/stored_mhash}"

if [ -e "$STORED_MHASH" ] ; then
	MHASH_AGE="$(perl -e 'print -M $ARGV[0];' "$STORED_MHASH")"
	if  [ -n "$(awk -v n1="$MHASH_AGE" -v n2="0.25" 'BEGIN { print(n1 > n2 ? "x" : "" ) }')" ] ; then
		rm -f -- "$STORED_MHASH"
	else
		MHASH="$(xz -d --format=raw --lzma1=dict=8MiB,lc=1,lp=1,pb=1,mode=normal,nice=64,mf=bt4,depth=0 <"$STORED_MHASH")"
	fi
fi

printf '%s' "Enter domain: "
read -r domain
default_username="leo.izen@gmail.com"
printf 'Enter username [%s]: ' "$default_username"
read -r username
if [ -z "$username" ] ; then
	username="$default_username"
fi
default_index="0"
printf 'Enter index [%s]: ' "$default_index"
read -r index
if [ -z "$index" ] ; then
	index="$default_index"
fi
default_maxlength="32"
printf 'Enter max password length [%s]: ' "$default_maxlength"
read -r maxlength
if [ -z "$maxlength" ] ; then
	maxlength="$default_maxlength"
elif [ ! "$maxlength" -ge 8 -o ! "$maxlength" -le 50 ] ; then
	printf 'Invalid maxlength: %s\n' "$maxlength" >&2
	exit 2
fi

if [ -z "${MHASH+x}" ] ; then
	printf "Enter master password: "
	# The cat pipe here causes the commands to be greedy
	# They won't exit until cat has sent EOF
	MHASH="$(cat | sh -c "$SHA3_256SUM" | cut -f1 -d' ' | perl -lne 'print pack "H*", $_' | base64)"
	printf "%s" "$MHASH" | xz -z --format=raw --lzma1=dict=8MiB,lc=1,lp=1,pb=1,mode=normal,nice=64,mf=bt4,depth=0 >"$STORED_MHASH"
fi


PHASH="$(printf '%s%s%s%s%s' "$domain" "$username" "$index" "$MHASH" "$maxlength" | sh -c "$SHA3_256SUM" | cut -f1 -d' ' | perl -lne 'print pack "H*", $_' | base64)"

printf '%s%s' "Df!1@2" "$PHASH" | head -c "$maxlength"
printf '\n'
