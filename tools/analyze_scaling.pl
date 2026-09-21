#!/usr/bin/env perl
use strict;
use warnings;

my $path = shift or die "usage: $0 benchmark.csv\n";
open my $input, '<', $path or die "cannot read $path: $!\n";
my $header = <$input>;
die "expected ranks,threads,seconds header\n" unless defined $header && $header =~ /ranks,threads,seconds/;
my @rows;
while (my $line = <$input>) {
    chomp $line;
    next unless length $line;
    my ($ranks, $threads, $seconds) = split /,/, $line;
    push @rows, [$ranks + 0, $threads + 0, $seconds + 0.0];
}
die "no benchmark rows\n" unless @rows;
my $baseline = $rows[0]->[2];
print "| MPI ranks | OpenMP threads/rank | Seconds | Speedup | Efficiency |\n";
print "|---:|---:|---:|---:|---:|\n";
for my $row (@rows) {
    my ($ranks, $threads, $seconds) = @$row;
    my $workers = $ranks * $threads;
    my $speedup = $baseline / $seconds;
    my $efficiency = 100.0 * $speedup / $workers;
    printf "| %d | %d | %.3f | %.2fx | %.1f%% |\n",
        $ranks, $threads, $seconds, $speedup, $efficiency;
}
