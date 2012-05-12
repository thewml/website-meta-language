##
##  wml_tags.pod.pl -- create wml_tags.pod file
##  Copyright (c) 1998,1999 Ralf S. Engelschall, All Rights Reserved. 
##

use strict;
use warnings;

use Getopt::Long;

my ($src, $dest, $main, $incl);
GetOptions(
    'src=s' => \$src,
    'dest=s' => \$dest,
    'main=s' => \$main,
    'incl=s' => \$incl,
) or "Die! Wrong parameters!";

open my $out_fh, '>', $dest
    or die "Cannot open output_file '$dest'. $!";
open my $in_fh, '<', $src
    or die "Cannot open input_file '$src'. $!";

my (@L);
while (<$in_fh>) {
    if (m|^%%CORE%%|) {
        open( my $tmp_fh, '<', $main)
            or die "Cannot open main file - '$main' - $!";
        @L = ();
        while (<$tmp_fh>) { 
            next if (m|^\s*$|);
            push(@L, $_);
        }
        close($tmp_fh);
        @L = sort(@L);
        my $n = 0;
        foreach my $l (@L) {
            print {$out_fh} " ".$l; 
            $n++;
            if (($n % 10) == 0) {
                $n = 0;
                print {$out_fh} "\n";
            }
        }
    }
    if (m|^%%INCL%%|) {
        open(my $tmp_fh, '<', $incl)
            or die "Cannot open incl file - '$incl' - $!";
        @L = ();
        while (<$tmp_fh>) { 
            next if (m|^\s*$|);
            push(@L, $_);
        }
        close($tmp_fh);
        @L = sort(@L);
        my $n = 0;
        foreach my $l (@L) {
            print {$out_fh} " ".$l; 
            $n++;
            if (($n % 10) == 0) {
                $n = 0;
                print {$out_fh} "\n";
            }
        }
    }
    else {
        print {$out_fh} $_;
    }
}
close($in_fh);
close($out_fh);

##EOF##
