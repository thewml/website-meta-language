##
##  slice_pass3.pl -- Pass 3
##  Copyright (c) 1997-2002 Ralf S. Engelschall. 
##  Copyright (c) 1999-2002 Denis Barbier.
##

package main;

##
##
##  Pass 3: Output generation
##
##

sub pass3 {
    my ($CFG) = @_;

    my ($slice, $outfile, $chmod, $status, $modifier, $out);
    my ($set, $cmds, $var);
    my ($start, $min, $max);
    my ($entry);

    verbose("\nPass 3: Output generation\n\n");

    foreach $entry (@{$CFG->{OPT}->{O}}) {

        #   determine skip options:
        #     u: a set is undefined
        #     w: a wildcard set does not match
        #     z: result is empty
        #     s: result is only composed of whitespaces
        $status = $CFG->{OPT}->{Y};
        if ($entry =~ s|\#([suwz\d]+)$||) {
            $modifier = $1;
            foreach (qw(u w z s)) {
                ($modifier =~ m/$_(\d+)/) and $status->{$_} = $1;
            }
        }
        if ($entry =~ m|^([_A-Z0-9~!+u%n\-\\^x*{}()@]+):(.+)@(.+)$|) {
            # full syntax
            ($slice, $outfile, $chmod) = ($1, $2, $3);
        }
        elsif ($entry =~ m|^([_A-Z0-9~!+u%n\-\\^x*{}()@]+):(.+)$|) {
            # only slice and file
            ($slice, $outfile, $chmod) = ($1, $2, '');
        }
        elsif ($entry =~ m|^([^@]+)@(.+)$|) {
            # only file and chmod
            ($slice, $outfile, $chmod) = ('ALL', $1, $2);
        }
        else {
            # only file 
            ($slice, $outfile, $chmod) = ('ALL', $entry, '');
        }
        verbose("    file `$outfile': sliceterm='$slice', chmodopts='$chmod'\n");
        #   parse the sliceterm and create corresponding
        #   Perl 5 statement containing Bit::Vector calls
        ($cmds, $var) = SliceTerm::Parse($slice, $status);
    
        #   skip file if requested by options
        if ($status->{u} > 0 and !defined($cmds)) {
                printwarning("Undefined set: skip generation of $outfile\n");
                next if $status->{u} > 1;
        }

        #   just debugging...
        if ($CFG->{OPT}->{X}) {
            verbose("        calculated Perl 5 set term:\n");
            verbose("        ----\n");
            my $x = $cmds; 
            $x =~ s|\n+$||;
            $x =~ s|\n|\n        |g;
            verbose("        $x\n");
            verbose("        ----\n");
        }

        #   now evaluate the Bit::Vector statements
        #   and move result to $set
        eval "$cmds; \$set = $var";
 
        #   now scan the set and write out characters
        #   which have a corresponding bit set.
        $start = 0;
        $out = '';
        while (($start < $set->Size()) &&
               (($min, $max) = $set->Interval_Scan_inc($start))) {
            $out .= substr($CFG->{INPUT}->{PLAIN},
                               $min, ($max-$min+1));
            $start = $max + 2;
        }

        #   skip file if requested by options
        if ($status->{z} > 0 and $out eq '') {
                printwarning("Empty output: skip generation of $outfile\n");
                main::error("Execution stopped\n") if $status->{z} > 2;
                next if $status->{z} == 2;
        }
        if ($status->{s} > 0 and ($out eq '' or $out !~ m/\S/)) {
                printwarning("Whitespace only: skip generation of $outfile\n");
                main::error("Execution stopped\n") if $status->{s} > 2;
                next if $status->{s} == 2;
        }

        #   open output file
        if ($outfile eq '-') {
            print $out;
        }
        else {
            open(OUT, ">$outfile")
                or main::error("Unable to write into $outfile: $!\n");
            print OUT $out
                or main::fileerror($outfile, "Unable to write into $outfile: $!\n");
            close(OUT)
                or main::fileerror($outfile, "Unable to close $outfile: $!\n");
        }

        #   additionally run chmod on the output file
        if ($outfile ne '-' and $chmod ne '' and -f $outfile) {
            system("chmod $chmod $outfile");
        }
    }
}

1;
