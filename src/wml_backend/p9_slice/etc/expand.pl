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

my $out_buf = "";

sub ProcessFile {
    my ($filename) = @_;

    open(my $fh, "<", $filename)
        or die "Could not open filename '$filename'.";

    LINES:
    while(my $l = <$fh>) {
        #   include a file
        if (my ($pre, $new_fn, $suf) =
            $l =~ m|^(.*)require[ \"\(]+([^ \"\(]+)[ \"\)];(.*)$|) {
            $out_buf .= $pre;
            ProcessFile($new_fn);
            $out_buf .= $suf;
            next LINES;
        }

        if (my ($req) = $l =~ m{\Arequire\s+(\w+);\s*\z}ms)
        {
            ProcessFile("${req}.pm");
            next LINES;
        }
        #   remove a require result code
        if ($l =~ m|^1;$|) {
            next LINES;
        }
        $out_buf .= $l;
    }
    close($fh);
}

ProcessFile($input_fn);

open my $out_fh, ">", $output_fn
    or die "Could not open '$output_fn' for writing";

print {$out_fh} $out_buf;

close($out_fh);


##EOF##
