#!/usr/bin/perl

use strict;
use warnings;
use Switch;

my @bad_metadata = ("BPS", "DURATION", "NUMBER_OF_FRAMES", "NUMBER_OF_BYTES", "_STATISTICS_WRITING_APP", "_STATISTICS_WRITING_DATE_UTC", "_STATISTICS_TAGS");
my @bad_metadata_opts = ();

foreach my $tag (@bad_metadata){
	push @bad_metadata_opts, ("-metadata:s", "$tag" . "=", "-metadata:s", "$tag" . "-eng=");
}

sub process_option {
	my $name = shift;
	my $value = shift;
	switch ($name){
		case "subs" {

		}
	}
}