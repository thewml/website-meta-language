#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long qw/ GetOptions /;
use File::Temp qw/ tempdir /;
use File::Copy qw/ copy /;
use File::Basename qw/ basename /;

my ( $src, $dest, $sect, $center, $release );

GetOptions(
    'src=s'     => \$src,
    'dest=s'    => \$dest,
    'section=s' => \$sect,
    'center=s'  => \$center,
    'release=s' => \$release,
) or die "Wrong options.";

if ( !defined($src) )
{
    die "src not specified.";
}

if ( !defined($dest) )
{
    die "dest not specified.";
}

if ( !defined($sect) )
{
    die "section not specified.";
}

if ( !defined($center) )
{
    die "center not specified.";
}

if ( !defined($release) )
{
    die "release not specified.";
}

my $dir = tempdir( CLEANUP => 1 );

my $pod;
if ( $src =~ m%/wml_include/% )
{
    $pod = $src;
    $pod =~ s%\A.*/wml_include/%wml::%ms;
    $pod =~ s%/%::%g;
    $pod =~ s%\.(?:src|pl|pm|pod)\z%%;
}
else
{
    $pod = basename( $src, '.src', '.pl', '.pm', '.pod' );
}
$pod = "$pod.pod";

if ( !-e $src )
{
    die "Cannot find '$src'";
}
copy( $src, "$dir/$pod" );
chdir($dir);

use Pod::Man ();
Pod::Man->new( section => $sect, center => $center, release => $release )
    ->parse_from_file( $pod, $dest )
