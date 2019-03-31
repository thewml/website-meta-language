#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;

my $mp4h;
my $srcdir;
my $input_fn;
my $output_fn;

GetOptions(
    "mp4h=s"   => \$mp4h,
    "srcdir=s" => \$srcdir,
    "input=s"  => \$input_fn,
    "output=s" => \$output_fn,
);

my @flags = ( "-X", 0, "-I", $srcdir, -D, "srcdir=$srcdir" );

open my $mp4h_fh, "-|", $mp4h, @flags, "-D", "format=pod", $input_fn
    or die "Could not open mp4h!";

open my $out_fh, ">", $output_fn
    or die "Could not open output filename for writing - '$output_fn'";

while (<$mp4h_fh>)
{
    s/_LT_/</g;
    s/_GT_/>/g;
    print {$out_fh} $_;
}

close($out_fh);
close($mp4h_fh);

