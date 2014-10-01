##
##  TEST.pl -- WML Test Suite utility functions
##  Copyright (c) 1997 Ralf S. Engelschall, All Rights Reserved.
##

package WmlTest;

use strict;
use warnings;
use autodie;

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

=begin foo

sub generic {
    local ($pass, $in, $out, $opt) = @_;
    local($tmpfile1, $tmpfile2, $tmpfile3, $rc);
    $tmpfile1 = &tmpfile(qq#$in#);
    $tmpfile2 = &tmpfile(qq#$out#);
    $tmpfile3 = &tmpfile;
    $rc = &system("$ENV{WML} -p$pass $opt $tmpfile1 >$tmpfile3");
    print ($rc == 0 ? "ok\n" : "not ok\n");
    $rc = &system("cmp $tmpfile2 $tmpfile3");
    print ($rc == 0 ? "ok\n" : "not ok\n");
}

=end foo

=cut

sub cleanup {
    foreach my $fn (@files_to_del) {
        unlink ($fn);
    }
}

1;
