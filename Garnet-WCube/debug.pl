#!/usr/bin/perl

$numargs = scalar(@ARGV);
if ($numargs != 2)
{
    print "Usage: only names of the files must be provided\n";
    exit 1;
}

$inpfile1 = $ARGV[0];
$outfile = $ARGV[1];
open(INP1, $inpfile1);
open(OUT, ">$outfile");

my %seen; # lookup table

$count =0;
while ($line = <INP1>) {
    if ($line =~ /^Msg:(\d+) delivered/) {
    $seen{$1}=$count;
    $count++;
    print $line;
}
}
close(INP1);
open(INP1, $inpfile1);
while ($line = <INP1>) {
if ($line =~ /^Msg:(\d+) entered/) {
    print OUT $line unless exists $seen{$1};
}
}

close(INP1);
close(OUT);
exit 0;
