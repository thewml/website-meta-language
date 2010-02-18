##
##  expand -- recursive filter to substitute each Perl
##            'require' command with the contents of the file
##
##  Copyright (c) 1995 Ralf S. Engelschall, <rse@engelschall.com>
##
##  Usage: perl unrequire file.pl new_file
##

use strict;
use warnings;

my ($input_fn, $output_fn) = @ARGV;

open my $out_fh, ">", $output_fn
    or die "Could not open '$output_fn' for writing";

ProcessFile($input_fn);

close($out_fh);

sub ProcessFile {
    my ($filename) = @_;

    open(my $fh, "<", $filename)
        or die "Could not open filename.";

    while(my $l = <$fh>) {
        #   include a file
        if (my ($pre, $new_fn, $suf) = 
            $l =~ m|^(.*)require[ \"\(]+([^ \"\(]+)[ \"\)]+;(.*)$|) {
            print {$out_fh} $pre;
            ProcessFile($new_fn);
            print {$out_fh} $suf;
        }
        #   remove a require result code
        if ($l =~ m|^1;$|) {
            next;
        }
        print {$out_fh} $l;
    }
    close($fh);
}


##EOF##
