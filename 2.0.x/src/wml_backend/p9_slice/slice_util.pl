##
##  slice_util.pl -- Utility functions
##  Copyright (c) 1997-2002 Ralf S. Engelschall. 
##  Copyright (c) 1999-2002 Denis Barbier.
##

package main;

sub verbose {
    my ($str) = @_;

    if ($main::CFG->{OPT}->{X}) {
        $str =~ s|^|** Slice:Verbose: |mg;
        print STDERR $str;
    }
}

sub printerror {
    my ($str) = @_;

    $str =~ s|^|** Slice:Error: |mg;
    print STDERR $str;
}

sub error {
    my ($str) = @_;

    printerror($str);
    exit(1);
}

sub fileerror {
    my $file  = shift;
    my ($str) = @_;

    printerror($str);
    unlink $file;
    exit(1);
}

sub printwarning {
    my ($str) = @_;

    $str =~ s|^|** Slice:Warning: |mg;
    print STDERR $str;
}

1;
##EOF##
