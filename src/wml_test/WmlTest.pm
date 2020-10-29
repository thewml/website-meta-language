##  wml_test/WmlTest.pm -- WML Test Suite utility functions
##  Copyright (c) 1997 Ralf S. Engelschall, All Rights Reserved.

package WmlTest;

use strict;
use warnings;
use autodie;

use Carp::Always;

use Test::More ();
use File::Temp qw/ tempdir tempfile /;
use Path::Tiny qw/ cwd path /;

my @files_to_del;

my $orig_cwd = cwd->absolute;
my $dir      = tempdir( CLEANUP => 1 );

sub init
{
    chdir $dir;
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

sub all_passes
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ( $in, $out, $opt ) = @_;
    return generic( "1-9", $in, $out, $opt );
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
        path($tmpfile3)->slurp_raw(),
        path($tmpfile2)->slurp_raw(),
        "generic cmp"
    );
}

sub add_files
{
    push @files_to_del, @_;
}

sub new_tempdir
{
    my $old = cwd()->absolute();
    my $ret = Path::Tiny->tempdir();
    chdir($ret);
    return { orig => $old, new => $ret, };
}

sub cleanup
{
    foreach my $fn (@files_to_del)
    {
        eval { unlink($fn); };
    }
    chdir $orig_cwd;
}

1;
