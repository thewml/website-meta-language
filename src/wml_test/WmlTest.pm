##
##  TEST.pl -- WML Test Suite utility functions
##  Copyright (c) 1997 Ralf S. Engelschall, All Rights Reserved.
##

package WmlTest;

use strict;
use warnings;
use autodie;

use Test::More ();
use File::Temp qw/ tempfile /;

my @files_to_del;

sub init {
    return;
}

sub tmpfile {
    my ($fh, $filename) = tempfile("wml-test-temp-XXXXXXXXX");

    print {$fh} @_;

    return $filename;
}

sub tmpfile_with_name {
    my $name = shift;

    open my $fh, '>', $name;
    print {$fh} @_;
    close ($fh);

    push @files_to_del, $name;

    return $name;
}

sub generic {
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ($pass, $in, $out, $opt) = @_;
    # local($tmpfile1, $tmpfile2, $tmpfile3, $rc);
    my $tmpfile1 = tmpfile($in);
    my $tmpfile2 = tmpfile($out);
    my $tmpfile3 = tmpfile();
    my $rc = system("$ENV{WML} -p$pass $opt $tmpfile1 >$tmpfile3");

    Test::More::ok (!$rc, "generic system wml");
    $rc = system("cmp $tmpfile2 $tmpfile3");

    Test::More::ok (!$rc, "generic cmp");
}

sub cleanup {
    foreach my $fn (@files_to_del) {
        unlink ($fn);
    }
}

1;
