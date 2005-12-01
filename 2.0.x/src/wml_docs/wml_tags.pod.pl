##
##  wml_tags.pod.pl -- create wml_tags.pod file
##  Copyright (c) 1998,1999 Ralf S. Engelschall, All Rights Reserved. 
##

open(OUT, ">wml_tags.pod");
open(IN, "<wml_tags.pod.in");
while (<IN>) {
    if (m|^%%CORE%%|) {
        open(TMP, "<wml_tags.L.main");
        @L = ();
        while (<TMP>) { 
            next if (m|^\s*$|);
            push(@L, $_);
        }
        close(TMP);
        @L = sort(@L);
        $n = 0;
        foreach $l (@L) {
            print OUT " ".$l; 
            $n++;
            if (($n % 10) == 0) {
                $n = 0;
                print OUT "\n";
            }
        }
    }
    if (m|^%%INCL%%|) {
        open(TMP, "<wml_tags.L.incl");
        @L = ();
        while (<TMP>) { 
            next if (m|^\s*$|);
            push(@L, $_);
        }
        close(TMP);
        @L = sort(@L);
        $n = 0;
        foreach $l (@L) {
            print OUT " ".$l; 
            $n++;
            if (($n % 10) == 0) {
                $n = 0;
                print OUT "\n";
            }
        }
    }
    else {
        print OUT $_;
    }
}
close(IN);
close(OUT);

##EOF##
