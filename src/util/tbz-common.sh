#!/bin/sh

# The shebang here is to identify this as an SH script, not a BASH script
# Actually executing this won't do anything interesting.

shell_is_interactive=false

case "$-" in
	*i*)
		shell_is_interactive=true
		;;
	*) 
		shell_is_interactive=false
		set -e
		set -f
		set -u
		;;
esac

if [ -z ${TBZ_COMMON_H_+x} ] ; then #ifndef TBZ_COMMON_H_
TBZ_COMMON_H_="true" #define TBZ_COMMON_H_

print_oneline_warning_message() {
	local message="$*"
	if [ -t 2 ] ; then
		printf '\e[33m\e[1mWarning: \e[21m%s\e[0m\n' "$message" >&2
	else
		printf 'Warning: %s\n' "$message" >&2
	fi
}

print_oneline_error_message(){
	local message="$*"
	if [ -t 2 ] ; then
		printf '\e[31m\e[1mError: \e[21m%s\e[0m\n' "$message" >&2
	else
		printf 'Error: %s\n' "$message" >&2
	fi
}

print_oneline_note_message() {
	local message="$*"
	if [ -t 2 ] ; then
		printf '\e[36m\e[1mNote: \e[21m%s\e[0m\n' "$message" >&2	
	else
		printf 'Note: %s\n' "$message" >&2
	fi
}

message_nonfatal() {
	if is_unset message_printer ; then
		error "Must provide a message type."
	fi
	if [ -z ${1+x} ] ; then
		local message=""
	else
		local message="$1"; shift
	fi
	"${message_printer}" "$message"
	local arg; for arg; do
		printf '%s\n' "$arg"
	done
}

error_nonfatal(){
	message_printer="print_oneline_error_message" message_nonfatal "$@"
}

error(){
	error_nonfatal "$@"
	if check "$shell_is_interactive" ; then
		return 1
	else
		exit 1
	fi
}

warning(){
	message_printer="print_oneline_warning_message" message_nonfatal "$@"
}

note(){
	message_printer="print_oneline_note_message" message_nonfatal "$@"
}

tolower() {
	awk 'BEGIN { print(tolower(ARGV[1])) }' "$*"
}

is_unset(){
	local arg
	for arg; do
		eval "local status=\"\${${arg}+x}\""
		if [ -z "$status" ] ; then
			return 0
		fi
	done
	return 1
}

is_set() {
	if is_unset "$@"; then
		return 1
	else
		return 0
	fi
}

assert_is_set(){
	if is_unset "$@"; then
		error "$* is unset."
	fi
}

check(){
	if [ "$*" = "true" ] ; then
		return 0
	else
		return 1
	fi
}

readfrom(){
	local read_string="$1"; shift
	printf '%s\n' "$read_string" | "$@"
}

matches_grep(){
	# Variables are prefixed because they're globally scoped
	local read_string="$1"; shift
	# This is all a hack to get around sh's lack of arrays
	local num_args="$#"
	local i=1; while [ "$i" -le "$num_args" ] ; do
		eval "local arg_num_${i}=\"\${${i}}\""
	i=$((1+$i)); done
	set --
	local i=1; while [ "$i" -le "$num_args" ] ; do
		eval "local arg=\"\${arg_num_${i}}\""
		set -- "$@" -e "${arg}"
	i=$((1+$i)); done
	readfrom "$read_string" $grep_command $grep_args "$@"
}

matches(){
	grep_command="grep -Eq" grep_args="${grep_args-}" matches_grep "$@"
}

fmatches(){
	grep_command="grep -Fqx" grep_args="${grep_args-}" matches_grep "$@"
}

num_compare() {
	awk -v n1="$1" -v n2="$2" 'BEGIN {print (n1<n2?"-1":(n1>n2?"1":"0")) }'
}

subtract(){
	awk -v n1="$1" -v n2="$2" 'BEGIN { print (n1 - n2) }'
}

is_gt() {
	if [ "$(num_compare "$1" "$2")" -gt 0 ] ; then
		return 0
	else
		return 1
	fi
}

is_lt() {
	if [ "$(num_compare "$1" "$2")" -lt 0 ] ; then
		return 0
	else
		return 1
	fi
}

is_ge() {
	if [ "$(num_compare "$1" "$2")" -ge 0 ] ; then
		return 0
	else
		return 1
	fi
}

is_le() {
	if [ "$(num_compare "$1" "$2")" -le 0 ] ; then
		return 0
	else
		return 1
	fi
}

parse_arguments(){
	if is_unset option_processor naked_argument_processor; then
		error "Must provide option processors."
	fi
	local finished_parsing_args="false"
	local arg; for arg; do
		if ! check "$finished_parsing_args" && matches "$arg" '^--'; then
			if [ "$arg" = "--" ] ; then
				finished_parsing_args="true"
			elif matches "$arg" "=" ; then
				"$option_processor" "$(readfrom "$arg" sed 's/^--\([^=]*\)=\(.*\)$/\1/')" "$(readfrom "$arg" sed 's/^--\([^=]*\)=\(.*\)$/\2/')"
			else
				local trunc_arg="$(readfrom "$arg" tail -c +3)"
				"$option_processor" "$trunc_arg" "$trunc_arg"
			fi
		else
			"$naked_argument_processor" "$arg"
		fi
	done
}

# Includes a file that doesn't have to be in PATH
# It could also be in the same directory as the file doing the including
include(){
	local included_file="$1"
	if command -p -v "$this_script_dirname"/"$included_file" >/dev/null ; then
		. "$this_script_dirname"/"$included_file"
	elif command -p -v "$this_script_abs_dirname"/"$included_file" >/dev/null ; then
		. "$this_script_abs_dirname"/"$included_file"
	elif command -p -v "$included_file" >/dev/null ; then
		. "$included_file"
	else
		error "Cannot find: $included_file"
	fi
}

value_of(){
	# THE VALUUUUEEE
	eval "local the_value=\"\${${1}}\""
	printf '%s' "$the_value"
}

replace_ends_char_(){
	if matches "$ends_char" '/' ; then
		ends_char="$(readfrom "$ends_char" sed 's:/:\\/:')"
	fi
}

chomp_char_from_head(){
	local ends_char="$1"; shift
	replace_ends_char_
	readfrom "$*" sed "s/^[${ends_char}]*//g"
}

chomp_char_from_tail(){
	local ends_char="$1"; shift
	replace_ends_char_
	readfrom "$*" sed "s/[${ends_char}]*\$//g"
}

chomp_char_from_ends(){
	local ends_char="$1"; shift
	replace_ends_char_
	readfrom "$*" sed "s/^[${ends_char}]*//g; s/[${ends_char}]*\$//g"
}

add_dir_to_export_env(){
	if is_set env_var; then
		local new_value="$(chomp_char_from_tail '/' "$*")"
		local curr_value="$(chomp_char_from_ends ':' "$(value_of "$env_var")")"
		if is_set prepend && check "$prepend"; then
			eval "export ${env_var}=${new_value}:${curr_value}"
		else
			eval "export ${env_var}=${curr_value}:${new_value}"
		fi
	else
		error "Pass an env_var" "Example: env_var=PATH prepend=true add_dir_to_export_env $HOME/bin"
	fi
}

if ! check "$shell_is_interactive" ; then
	this_script_fullname="$0"
	this_script="$(basename "$this_script_fullname")"
	this_script_dirname="$(dirname "$this_script_fullname")"
	this_script_abs="$(readlink -f "$this_script_fullname")"
	this_script_abs_dirname="$(dirname "$this_script_abs")"
fi

fi #endif TBZ_COMMON_H_

