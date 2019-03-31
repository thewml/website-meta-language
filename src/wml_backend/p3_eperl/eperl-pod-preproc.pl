#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long qw/ GetOptions /;
use Path::Tiny qw/ path /;

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

path($o_fn)->spew_raw( path($i_fn)->slurp_raw =~ s{\@V\@}{$ver}gr );
