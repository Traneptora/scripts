#!/usr/bin/perl

sub hprint {
	my $number = shift;
	chomp $number;

	my $human_number;
	my $suffix;

	if ($number >= 1099511627776){
		$human_number = sprintf("%.2f", $number / 1099511627776);
		$suffix = "Ti";
	} elsif ($number >= 1073741824){
		$human_number = sprintf("%.2f", $number / 1073741824);
		$suffix = "Gi";
	} elsif ($number >= 1048576){
		$human_number = sprintf("%.2f", $number / 1048576);
		$suffix = "Mi";
	} elsif ($number >= 1024){
		$human_number = sprintf("%.2f", $number / 1024);
		$suffix = "Ki";
	} else {
		$human_number = $number;
		$suffix = "";
	}

	print "$human_number $suffix\n";
}

# This disables STDOUT buffering.
# Don't ask. It's Perl.
select STDOUT;
$|=1;

if ($#ARGV < 0){
	while (<>){
		hprint($_);
	}
} else {
	foreach (@ARGV){
		hprint($_);
	}
}
