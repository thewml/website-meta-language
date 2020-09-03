#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long qw / GetOptions /;

use Path::Tiny qw/ path tempdir tempfile cwd /;

my %substitutions;

my $input_fn;
my $output_fn;
my $version;
my $date;

GetOptions(
    "version=s" => \$version,
    "date=s"    => \$date,
    "input=s"   => \$input_fn,
    "output=s"  => \$output_fn,
);

if ( !defined($input_fn) )
{
    die "Input filename not specified!";
}

if ( !defined($output_fn) )
{
    die "Output filename not specified!";
}

path($output_fn)
    ->spew_utf8( path($input_fn)->slurp_utf8() =~
s#^(Website META Language, Version )[0-9\.]+ \([0-9]+-[A-Z][a-z]{2}-[0-9]+\)$#$1$version ($date)#mrs
    );
