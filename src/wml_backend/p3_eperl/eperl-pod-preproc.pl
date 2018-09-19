#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;

my $i_fn;
my $o_fn;

GetOptions(
    'input=s' => \$i_fn,
    'output=s' => \$o_fn,
);

my $ver = 5;

open my $in, "<", $i_fn
    or die "Cannot open input - '$i_fn'!";

my $buffer;
while (my $l = <$in>)
{
    $l =~ s{\@V\@}{$ver}g;
    $buffer .= $l;
}
close($in);

open my $out, ">", $o_fn
    or die "Cannot open output - '$o_fn'!";
print {$out} $buffer;
close($out);
