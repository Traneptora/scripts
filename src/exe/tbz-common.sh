#!/hint/sh

# The shebang here is to identify this as an SH script, not a BASH script
# Actually executing this won't do anything interesting.

if [ -t 2 ] ; then
	printf >&2 '%sError: %s%s%s\n' "$(tput bold)$(tput setaf 1)" "$(tput sgr0)$(tput setaf 1)" 'You are calling a script using the deprecated framework.' "$(tput sgr0)"
else
	printf >&2 'Error: %s\n' 'You are calling a script using the deprecated framework.'
fi

shell_is_interactive_=false

case "$-" in
	*i*)
		shell_is_interactive_=true
		;;
	*) 
		shell_is_interactive_=false
		;;
esac

if [ -z "${TBZ_COMMON_H_+x}" ] ; then #ifndef TBZ_COMMON_H_
TBZ_COMMON_H_="true" #define TBZ_COMMON_H_

if [ "$shell_is_interactive_" != "true" ] && [ "${tbz_common_no_set_efu_-}" != "true" ] ; then
	set -e
	set -f
	set -u
	printf(){
		/usr/bin/printf "$@"
	}
fi

print_oneline_cli_message_(){
	local color="$1"; shift
	local title="$1"; shift
	local uncolor="$1"; shift
	local message="$*"
	if [ -t 2 ] ; then
		printf "${color}%s: ${uncolor}%s\e[0m\n" "$title" "$message" >&2
	else
		printf '%s: %s\n' "$title" "$message" >&2
	fi
}

message_nonfatal_() {
	local color="$1"; shift
	local title="$1"; shift
	local uncolor="$1"; shift
	if [ -z "${1+x}" ] ; then
		local message=""
	else
		local message="$1"; shift
	fi
	print_oneline_cli_message_ "$color" "$title" "$uncolor" "$message"
	local arg; for arg; do
		printf "${color}%s${uncolor}\e[0m\n" "$arg"
	done
}

error_nonfatal(){
	message_nonfatal_ '\e[31m\e[1m' 'Error' '\e[21m' "$@"
}

error(){
	error_nonfatal "$@"
	if checkvar shell_is_interactive_ ; then
		kill -s INT "$$"
	else
		exit 1
	fi
}

warning(){
	message_nonfatal_ '\e[33m\e[1m' 'Warning' '\e[21m' "$@"
}

note(){
	message_nonfatal_ '\e[36m\e[1m' 'Note' '\e[21m' "$@"
}

set_var(){
	eval "$1=$(shell_escape "$2")"
}

_return_value(){
	local status_="$?"
	_return_var="${_return_var:-___}"
	set_var "$_return_var" "$*"
	return "$status_"
}

__()(
	_return_var="___" "$@"
	printf '%s\n' "$___"
)

value_of(){
	# THE VALUUUUEEE
	eval "_return_value \"\${$1}\""
}

tolower() {
	_return_value "$(awk 'BEGIN { print(tolower(ARGV[1])) }' "$*")"
}


# deprecated
is_unset(){
	local arg; for arg; do
		eval "local status=\"\${${arg}+x}\""
		if [ -z "${status-}" ] ; then
			return 0
		fi
	done
	return 1
}

# deprecated
is_set() {
	if is_unset "$@"; then
		return 1
	else
		return 0
	fi
}

# deprecated
assert_is_set(){
	if is_unset "$@"; then
		error "$* is unset."
	fi
}

check(){
	if [ "$*" = "true" ] || [ "$*" = "yes" ]; then
		return 0
	else
		return 1
	fi
}

# deprecated
checkvar(){
	if is_set "$*"; then
		if check "$(__ value_of "$*")" ; then
			return 0
		else
			return 1
		fi
	else
		return 2
	fi
}

readfrom(){
	local read_string="$1"; shift
	_return_value "$(printf '%s\n' "$read_string" | "$@")"
}

matches_grep(){
	local grep_args="$1"; shift
	if is_unset matches_grep_read_string; then
		local read_string="$1"; shift
	else
		local read_string="${matches_grep_read_string-}"
	fi
		
	# This is all a hack to get around sh's lack of arrays
	local num_args="$#"
	local i=1; while [ "$i" -le "$num_args" ] ; do
		eval "local arg_num_${i}=\"\${$i}\""
	i=$((1+i)); done
	set --
	local i=1; while [ "$i" -le "$num_args" ] ; do
		eval "local arg=\"\${arg_num_${i}}\""
		set -- "$@" -e "${arg}"
	i=$((1+i)); done
	
	printf '%s\n' "$read_string" | "${grep_command:-grep}" "$grep_args" "$@"
}

matches(){
	matches_grep -Eq "$@"
}

fmatches(){
	matches_grep -Fqx "$@"
}

num_compare() {
	_return_value "$(awk -v n1="$1" -v n2="$2" 'BEGIN {print (n1 < n2 ? "-1" : (n1 > n2 ? "1" : "0")) }')"
}

subtract(){
	_return_value "$(awk -v n1="$1" -v n2="$2" 'BEGIN { print (n1 - n2) }')"
}

is_gt() {
	num_compare "$1" "$2"
	if [ "$___" -gt 0 ] ; then
		return 0
	else
		return 1
	fi
}

is_lt() {
	num_compare "$1" "$2"
	if [ "$___" -lt 0 ] ; then
		return 0
	else
		return 1
	fi
}

is_ge() {
	num_compare "$1" "$2"
	if [ "$___" -ge 0 ] ; then
		return 0
	else
		return 1
	fi
}

is_le() {
	num_compare "$1" "$2"
	if [ "$___" -le 0 ] ; then
		return 0
	else
		return 1
	fi
}

parse_arguments(){
	local finished_parsing_args="false"
	local arg; for arg; do
		if ! check "${finished_parsing_args-}" && matches "$arg" '^--'; then
			if [ "$arg" = "--" ] ; then
				finished_parsing_args="true"
			elif matches "$arg" "=" ; then
				local name; local value
				readfrom "$arg" sed 's/^--\([^=]*\)=\(.*\)$/\1/'; name="$___"
				readfrom "$arg" sed 's/^--\([^=]*\)=\(.*\)$/\2/'; value="$___"
				"${option_processor:?}" "$name" "$value"
			else
				readfrom "$arg" tail -c +3
				"${option_processor:?}" "$___" "$___"
			fi
		else
			"${naked_argument_processor:?}" "$arg"
		fi
	done
}

command_exists(){
	command -v "$1" >/dev/null
}

# Includes a file that doesn't have to be in PATH
# It could also be in the same directory as the file doing the including
# deprecated
include(){
	local included_file="$1"; shift
	if [ -e "$this_script_dirname"/"$included_file" ] ; then
		. "$this_script_dirname"/"$included_file"
	elif [ -e "$this_script_abs_dirname"/"$included_file" ] ; then
		. "$this_script_abs_dirname"/"$included_file"
	elif [ -e ./"$included_file" ] ; then
		. ./"$included_file"
	elif command_exists "$included_file"; then
		. "$included_file"
	else
		error "Cannot find: $included_file"
	fi
}

iamroot(){
	test "$(id -u)" = "0"
}

# NICE MEME
iamgroot(){
	iamroot
}

shell_escape() {
	local arg; for arg; do
		arg="$(printf %s "$arg" | sed "s/'/'\\\\''/g"; printf 'x\n')"
		arg="${arg%?}"
		printf "'%s' " "$arg" | sed "s/^''//" | sed "s/\([^\\\\]\)''/\1/g"
	done
	printf '\n'
}


shecho(){
	local retvalue=""
	local arg; for arg; do
		readfrom "$arg" sed -r "s/'/'\\\\''/g"
		retvalue="${retvalue}'${___}' "
	done
	readfrom "$retvalue" sed "s/^''//g; s/\([^\\\\]\)''/\\1/g"
}

_add_to_export_env0(){

	local add_value="$1"

	eval "local curr_value=\"\${${env_var:?}-}\""
	curr_value=${curr_value%%${separator_char}}
	curr_value=${curr_value##${separator_char}}

	local my_sep=""
	if [ -n "${curr_value-}" ] ; then
		my_sep="${separator_char:?}"
	fi


	if check "${prepend-}"; then
		local new_value="${add_value}${my_sep}${curr_value}"
	else
		local new_value="${curr_value}${my_sep}${add_value}"
	fi
	eval "export ${env_var}=\"\${new_value}\""
}

add_to_export_env() {
	local arg; for arg; do
		_add_to_export_env0 "$arg"
	done
}

add_dir_to_export_env0_(){
	___="$1"
	___="${___%%/}"
	if [ "${#___}" = 0 ] ; then ___="/"; fi
	local new_value="$___"
	separator_char=':' add_to_export_env "$new_value"
}

add_dirs_to_export_env(){
	local arg; for arg; do
		add_dir_to_export_env0_ "$arg"
	done
}

add_to_search_path(){
	env_var=PATH add_dirs_to_export_env "$@"
}

lessfrom(){
	"$@" | less
}

tbz_cleanup_temp_files_(){
	eval "rm -f -- $tbz_common_temp_files_"
}

create_temp_file(){
	local temp_file
	temp_suffix=${temp_suffix:-"$1"}
	if [ -n "${temp_suffix:+x}" ]; then
		temp_file="$(mktemp --suffix=".${temp_suffix-}")"
	else
		temp_file="$(mktemp)"
	fi
	tbz_common_temp_files_="${tbz_common_temp_files_} $(__ shecho "$temp_file")"
	_return_value "$temp_file"
}

# Don't give it a stupid name
add_cleanup_routine(){
	tbz_cleanup_routines_="$tbz_cleanup_routines_ $1"
}

tbz_cleanup_(){
	local routine
	for routine in $tbz_cleanup_routines_; do
		eval "$routine"
	done
	trap - INT QUIT TERM HUP ALRM EXIT
	echo
	case "$SIGNAL" in
		EXIT)
			;;
		*)
			kill -s "${SIGNAL}" "$$"
			;;
	esac
}

tbz_cleanup_routines_=""
tbz_common_temp_files_=""

add_cleanup_routine "tbz_cleanup_temp_files_"

if ! check "$shell_is_interactive_" ; then
	trap 'SIGNAL=INT  tbz_cleanup_' INT
	trap 'SIGNAL=QUIT tbz_cleanup_' QUIT
	trap 'SIGNAL=TERM tbz_cleanup_' TERM
	trap 'SIGNAL=HUP  tbz_cleanup_' HUP
	trap 'SIGNAL=ALRM tbz_cleanup_' ALRM
fi

trap 'SIGNAL=EXIT tbz_cleanup_' EXIT

if ! check "$shell_is_interactive_" ; then
	this_script_fullname="$0"
	this_script="$(basename "$this_script_fullname")"
	this_script_dirname="$(dirname "$this_script_fullname")"
	this_script_abs="$(readlink -f "$this_script_fullname")"
	this_script_abs_dirname="$(dirname "$this_script_abs")"
	export this_script_fullname
	export this_script
	export this_script_dirname
	export this_script_abs
	export this_script_abs_dirname
fi

fi #endif TBZ_COMMON_H_

