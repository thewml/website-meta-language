##
##  TEST.pl -- WML Test Suite utility functions
##  Copyright (c) 1997 Ralf S. Engelschall, All Rights Reserved.
##

package WmlTest;

use strict;
use warnings;
use autodie;

use Test::More ();
use File::Temp qw/ tempdir tempfile /;
use IO::All qw/ io /;

my @files_to_del;

my $dir = tempdir( CLEANUP => 1 );

sub init
{
    return;
}

sub tmpfile
{
    my ( $fh, $filename ) = tempfile( DIR => $dir );

    print {$fh} @_;

    return $filename;
}

sub tmpfile_with_name
{
    my $name = shift;

    open my $fh, '>', $name;
    print {$fh} @_;
    close($fh);

    push @files_to_del, $name;

    return $name;
}

sub generic
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ( $pass, $in, $out, $opt ) = @_;

    # local($tmpfile1, $tmpfile2, $tmpfile3, $rc);
    my $tmpfile1 = tmpfile($in);
    my $tmpfile2 = tmpfile($out);
    my $tmpfile3 = tmpfile();
    my $rc       = system("$ENV{WML} -p$pass $opt $tmpfile1 >$tmpfile3");

    Test::More::ok( !$rc, "generic system wml" );

    # $rc = system("cmp $tmpfile2 $tmpfile3");

    Test::More::is(
        io()->file($tmpfile3)->all(),
        io()->file($tmpfile2)->all(),
        "generic cmp"
    );
}

sub add_files
{
    push @files_to_del, @_;
}

sub cleanup
{
    foreach my $fn (@files_to_del)
    {
        eval { unlink($fn); };
    }
}

1;
