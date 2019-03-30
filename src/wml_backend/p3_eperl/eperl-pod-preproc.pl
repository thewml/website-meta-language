#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;

my $i_fn;
my $o_fn;
my $v_fn;

GetOptions(
    'input=s'       => \$i_fn,
    'output=s'      => \$o_fn,
    'versionfrom=s' => \$v_fn,
);

my $ver;
open my $ver_fh, "<", $v_fn
    or die "Cannot open '$v_fn'";
VER_LOOP:
while ( my $l = <$ver_fh> )
{
    if ( my ($val) = $l =~ m{"(\d+\.\d+\.\d+)"} )
    {
        $ver = $val;
        last VER_LOOP;
    }
}
close($ver_fh);

open my $in, "<", $i_fn
    or die "Cannot open input - '$i_fn'!";

my $buffer;
while ( my $l = <$in> )
{
    $l =~ s{\@V\@}{$ver}g;
    $buffer .= $l;
}
close($in);

open my $out, ">", $o_fn
    or die "Cannot open output - '$o_fn'!";
print {$out} $buffer;
close($out);
