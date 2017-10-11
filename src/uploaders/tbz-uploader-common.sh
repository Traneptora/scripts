#!/bin/sh
. tbz-common.sh

if is_unset TBZ_UPLOADER_COMMON_H ; then #ifndef TBZ_UPLOADER_COMMON_H
TBZ_UPLOADER_COMMON_H="true" #define TBZ_UPLOADER_COMMON_H

tbz_uploader_common(){
	local returned_url="$(curl "$@")"
	printf '%s\n' "$returned_url"
	if is_set DISPLAY ; then
		printf '%s' "$returned_url" | xclip
		xclip -o -t text/plain | xclip -i -selection clipboard -t text/plain
	fi
}

fi #endif TBZ_UPLOADER_COMMON_H
