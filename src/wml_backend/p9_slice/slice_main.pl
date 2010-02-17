#!@PATH_PERL@
eval 'exec @PATH_PERL@ -S $0 ${1+"$@"}'
    if $running_under_some_shell;

use strict;
use warnings;

use vars qw($CFG);

BEGIN { $^W = 0; } # get rid of nasty warnings

##         _ _          
##     ___| (_) ___ ___ 
##    / __| | |/ __/ _ \
##    \__ \ | | (_|  __/
##    |___/_|_|\___\___|
##                    
##    Slice -- Extract out pre-defined slices of an ASCII file
##
##    The slice program reads an inputfile and divide its prepaired ASCII contents
##    into possibly overlapping slices. These slices are determined by enclosing
##    blocks which are defined by begin and end delimiters which have to be
##    already in the file.   The final output gets calculated by a slice term
##    consisting of slice names, set theory operators and optional round brackets.
##  
##    The latest release can be found on
##    http://www.engelschall.com/sw/slice/
##  
##    Copyright (c) 1997-2002 Ralf S. Engelschall.
##    Copyright (c) 1999-2002 Denis Barbier.
##  
##    This program is free software; it may be redistributed and/or modified only
##    under the terms of the GNU General Public License, which may be found in the
##    SLICE source distribution.  Look at the file COPYING.   This program is
##    distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
##    without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
##    PARTICULAR PURPOSE.  See either the GNU General Public License for more
##    details.
##  
##                                Ralf S. Engelschall
##                                rse@engelschall.com
##                                www.engelschall.com


use Getopt::Long 2.13;
use IO::Handle 1.15;
use IO::File 1.07;
use Bit::Vector 5.0;


##
##  slice_vers.pl -- Version Information for Slice (syntax: Perl)
##  [automatically generated and maintained by GNU shtool]
##

my $slice_version = {
    'v_hex'   => 0x103208,
    'v_short' => "1.3.8",
    'v_long'  => "1.3.8 (10-Feb-2002)",
    'v_tex'   => "This is Slice, Version 1.3.8 (10-Feb-2002)",
    'v_gnu'   => "Slice 1.3.8 (10-Feb-2002)",
    'v_web'   => "Slice/1.3.8",
    'v_sccs'  => "@(#)Slice 1.3.8 (10-Feb-2002)",
    'v_rcs'   => "\$Id: Slice 1.3.8 (10-Feb-2002) $/"
};

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

require SliceTerm;

sub usage {
    print STDERR "Usage: slice [options] [file]\n";
    print STDERR "\n";
    print STDERR "Options:\n";
    print STDERR "  -o, --outputfile=FILESPEC  create output file(s)\n";
    print STDERR "  -y, --output-policy=STRING set default output policy\n";
    print STDERR "  -v, --verbose              enable verbose mode\n";
    print STDERR "  -V, --version              display version string\n";
    print STDERR "  -h, --help                 display usage page\n";
    print STDERR "\n";
    print STDERR "FILESPEC format:\n";
    print STDERR "\n";
    print STDERR "  [SLICETERM:]PATH[\@CHMODOPT]\n";
    print STDERR "\n";
    print STDERR "  SLICETERM ..... a set-theory term describing the slices\n";
    print STDERR "  PATH .......... a filesystem path to the outputfile\n";
    print STDERR "  CHMODOPT ...... permission change options for 'chmod'\n";
    print STDERR "\n";
    exit(1);
}

sub hello {
    print STDERR "$slice_version->{v_tex}\n";
    print STDERR <<'EOT';
Copyright (c) 1997-2002 Ralf S. Engelschall. 
Copyright (c) 1999-2002 Denis Barbier.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
EOT
    exit(0);
}

sub setup {
    my ($CFG) = @_;

    #   parse command line options
    my $opt_h = 0;
    my $opt_V = 0;
    my $opt_v = 0;
    my $opt_y = '';
    my @opt_o = ();
    my $INPUT;
    my (@options_list) = (
        "v|verbose",
        "V|version",
        "h|help",
        "o|outputfile=s@",
        "y|output-policy=s",
    );
    $SIG{'__WARN__'} = sub { 
        print STDERR "Slice:Error: $_[0]";
    };

    $Getopt::Long::bundling = 1;
    $Getopt::Long::getopt_compat = 0;
    if (not Getopt::Long::GetOptions(@options_list)) {
        print STDERR "Try `$0 --help' for more information.\n";
        exit(0);
    }
    $SIG{'__WARN__'} = undef;
    usage($0) if ($opt_h);
    hello() if ($opt_V);

    #   process command line arguments and
    #   read input file
    if (($#ARGV == 0 and $ARGV[0] eq '-') or $#ARGV == -1) {
        my $fp = new IO::Handle;
        $fp->fdopen(fileno(STDIN), "r")
            || error("Unable to load STDIN: $!\n");
        local ($/) = undef;
        $INPUT = <$fp>;
        $fp->close()
            || error("Unable to close STDIN: $!\n");
    }
    elsif ($#ARGV == 0) {
        my $fp = new IO::File;
        $fp->open($ARGV[0])
            || error("Unable to load $ARGV[0]: $!\n");
        local ($/) = undef;
        $INPUT = <$fp>;
        $fp->close()
            || error("Unable to close $ARGV[0]: $!\n");
    }
    else {
        usage();
    }

    #   add additional options
    $INPUT =~ s|^%!slice\s+(.*?)\n|push(@ARGV, split(' ', $1)), ''|egim;
    if (not Getopt::Long::GetOptions(@options_list)) {
        usage();
    }
    if ($#opt_o == -1) {
        @opt_o = ( "ALL:-" ); # default is all on stdout
    }

    #   setup the $CFG hash
    $CFG->{INPUT} = {};
    $CFG->{INPUT}->{SRC}   = $INPUT;  # original source
    $CFG->{INPUT}->{PLAIN} = '';      # source without slice delimiters
    $CFG->{OPT} = {};    
    $CFG->{OPT}->{X} = $opt_v;        # option -v
    $CFG->{OPT}->{O} = [ @opt_o ];    # options -o
    $CFG->{OPT}->{Y} = {
        'u' => 0, 'w' => 0,
        'z' => 0, 's' => 0,
    };
    my $modifier = $opt_y;
    foreach (qw(u w z s)) {
        ($modifier =~ m/$_(\d+)/) and $CFG->{OPT}->{Y}->{$_} = $1;
    }
    $CFG->{SLICE} = {};
    $CFG->{SLICE}->{SET} = {};       
    $CFG->{SLICE}->{SET}->{ASC} = {}; # slice set, represented in ASCII
    $CFG->{SLICE}->{SET}->{OBJ} = {}; # slice set, represented as Bit::Vector object
    $CFG->{SLICE}->{MINLEVELS}  = {}; # slice min levels
    $CFG->{SLICE}->{MAXLEVEL}   = 0;  # maximum slice level
}

##
##
##  Pass 1: Determine delimiters
##
##

sub pass1 {
    my ($CFG) = @_;

    my (@CURRENT_SLICE_NAMES, %CURRENT_LEVEL_BRAIN, $CURRENT_LEVEL_SET);
    my ($INPUT, $prolog, $pos, $inputpos, $prev, $name, $namex, $L, $open);

    verbose("\nPass 1: Determine delimiters\n\n");

    @CURRENT_SLICE_NAMES = ();
    %CURRENT_LEVEL_BRAIN = ();
    $CURRENT_LEVEL_SET   = new Bit::Vector(512);

    #   allocate the next free level starting from 1
    my $alloclevel = sub {
        my ($i);

        for ($i = 0; $i <= $CURRENT_LEVEL_SET->Max(); $i++) {
            last if (not $CURRENT_LEVEL_SET->bit_test($i));
        }
        $CURRENT_LEVEL_SET->Bit_On($i);
        return $i + 1;
    };

    #   clear the given level
    my $clearlevel = sub {
        my ($i) = @_;

        $CURRENT_LEVEL_SET->Bit_Off($i - 1);
    };

    $INPUT = $CFG->{INPUT}->{SRC};
    $open  = 0;
    $pos   = 0;
    $prev  = 0;
    while ($INPUT =~ m/
             (?=[\[:])                 #  Consider only sequences beginning
                                       #  with `[' or `:'
             (?:\[([A-Z][_A-Z0-9]*):   #  Begin delimiter
                      |
             :([A-Z][_A-Z0-9]*)?\])    #  End delimiter
                      /gx) {
        if (defined ($1)) {
            #
            #   begin delimiter found
            #
            $name     = $1;
            $inputpos = pos($INPUT);
            $prolog   = substr ($INPUT, $prev, $inputpos - $prev - length ($name) - 2);

            #   add prolog
            $CFG->{INPUT}->{PLAIN} .= $prolog;

            #   and store position of next character in input datas
            $pos  += length($prolog);
            $prev  = $inputpos;

            $L = $alloclevel->();                 # allocate next free level

            push(@CURRENT_SLICE_NAMES, $name);  # remember name  for end delimiter
            $CURRENT_LEVEL_BRAIN{"$name"} .= ":$L";# remember level for end delimiter
            if ($CFG->{SLICE}->{MINLEVELS}->{"$name"} eq '' or
                $CFG->{SLICE}->{MINLEVELS}->{"$name"} > $L) {
                $CFG->{SLICE}->{MINLEVELS}->{"$name"} = $L;
            }

            #  now begin entry with LEVEL:START
            $CFG->{SLICE}->{SET}->{ASC}->{"$name:$L"} .=
                 ($CFG->{SLICE}->{SET}->{ASC}->{"$name:$L"} ? ',' : '') . "$L:$pos";

            #  adjust notice about highest level
            $CFG->{SLICE}->{MAXLEVEL} = ($CFG->{SLICE}->{MAXLEVEL} < $L ?
                                         $L : $CFG->{SLICE}->{MAXLEVEL});

            verbose("    slice `$name': begin at $pos, level $L\n");

            $open++;
        }
        elsif ($open > 0) {
            #
            #   end delimiter found
            #
            $name     = $2;
            $inputpos = pos($INPUT);
            $prolog   = substr ($INPUT, $prev, $inputpos - $prev - length ($name) - 2);

            #   add prolog
            $CFG->{INPUT}->{PLAIN} .= $prolog;

            #   and store position of next character in input datas
            $pos  += length($prolog) - 1;
            $prev  = $inputpos;

            $namex = pop(@CURRENT_SLICE_NAMES);      # take remembered name
            $name  = $namex if ($name eq '');        # fill name because of shortcut syntax
            $CURRENT_LEVEL_BRAIN{"$name"} =~ s|:(\d+)$||; # take remembered level
            $L = $1;

            $clearlevel->($L);                         # de-allocate level

            # now end entry with :END
            $CFG->{SLICE}->{SET}->{ASC}->{"$name:$L"} .= ":$pos";

            verbose("    slice `$name': end at $pos, level $L\n");

            $pos++;
            $open--;
        }
    }
    # add all remaining input
    $CFG->{INPUT}->{PLAIN} .= substr ($INPUT, $prev);

    #   check: were all opened slices really closed?
    if ($CURRENT_LEVEL_SET->Norm > 0) {
        my $i;
        my $err = '';
        for ($i = 0; $i <= $CURRENT_LEVEL_SET->Max(); $i++) {
            if ($CURRENT_LEVEL_SET->bit_test($i)) {
                my $name;
                foreach $name (keys(%CURRENT_LEVEL_BRAIN)) {
                    if ($CURRENT_LEVEL_BRAIN{$name} == ($i+1)) {
                        $err .= " `$name'";
                    }
                }
            }
        }
        error("Some slices were not closed:$err\n");
    }
}

##
##
##  Pass 2: Calculation of slice sets
##
##

sub pass2 {
    my ($CFG) = @_;

    my ($n, $asc, $slice, $set, $setA);

    verbose("\nPass 2: Calculation of slice sets\n\n");

    #  convert ASCII set representation string into internal set object
    sub asc2set {
        my ($asc, $set, $onlylevel, $notcleared) = @_;
        my ($i, $I, $internal, $from, $to, $level);

        $set->Empty() if (($notcleared eq '') or (not $notcleared));
        return $set if ($asc =~ m|^\d+:0:-1$|); # string represents the empty set

        #   split out the interval substrings 
        my @I = ($asc);
        @I = split(',', $asc) if (index($asc, ',') > 0);

        #   iterate over each interval and
        #   set the corresponding elements in the set
        foreach my $interval (@I) {
            ($level, $from, $to) = ($interval =~ m|^(\d+):(\d+):(\d+)$|);
            next if (($onlylevel ne '') and ($level != $onlylevel)); 
            next if ($from > $to);
            $set->Interval_Fill($from, $to);
        }
    }

    $n = length($CFG->{INPUT}->{PLAIN})+1;
    $set  = new Bit::Vector($n); # working set
    $setA = new Bit::Vector($n); # "all" set

    #   restore slice names
    foreach $slice (keys(%{$CFG->{SLICE}->{SET}->{ASC}})) {
        $asc = $CFG->{SLICE}->{SET}->{ASC}->{$slice};
        delete $CFG->{SLICE}->{SET}->{ASC}->{$slice};
        $slice =~ s|:\d+$||g;
        $CFG->{SLICE}->{SET}->{ASC}->{$slice} .=
                 ($CFG->{SLICE}->{SET}->{ASC}->{"$slice"} ? ',' : '') . $asc;
    }

    #   convert ASCII representation to real internal set objects
    foreach $slice (keys(%{$CFG->{SLICE}->{SET}->{ASC}})) {
        $asc = $CFG->{SLICE}->{SET}->{ASC}->{$slice};
        $set->Empty();
        asc2set($asc, $set);
        $CFG->{SLICE}->{SET}->{OBJ}->{$slice} = $set->Clone();
    }

    #   define the various (un)defined slice areas
    $set->Fill();
    $CFG->{SLICE}->{SET}->{OBJ}->{'UNDEF0'} = $set->Clone();
    $set->Empty();
    $CFG->{SLICE}->{SET}->{OBJ}->{'DEF0'} = $set->Clone();
    $setA->Empty();
    for (my $i = 1; $i <= $CFG->{SLICE}->{MAXLEVEL}; $i++) {
        $set->Empty();
        foreach $slice (keys(%{$CFG->{SLICE}->{SET}->{ASC}})) {
            $asc = $CFG->{SLICE}->{SET}->{ASC}->{$slice};
            asc2set($asc, $set, $i, 1); # load $set with entries of level $i
            $setA->Union($setA, $set);   # add to $setA these entries
        }
        $CFG->{SLICE}->{SET}->{OBJ}->{"DEF$i"} = $set->Clone();
        $set->Complement($set);
        $CFG->{SLICE}->{SET}->{OBJ}->{"UNDEF$i"} = $set->Clone();
    }
    $CFG->{SLICE}->{SET}->{OBJ}->{'DEF'} = $setA->Clone();
    $setA->Complement($setA);
    $CFG->{SLICE}->{SET}->{OBJ}->{'UNDEF'} = $setA->Clone();
    $CFG->{SLICE}->{SET}->{OBJ}->{'ALL'} = $CFG->{SLICE}->{SET}->{OBJ}->{'UNDEF0'};

    #   define the various slice areas which are not overwritten
    foreach $slice (keys(%{$CFG->{SLICE}->{SET}->{ASC}})) {
        $asc = $CFG->{SLICE}->{SET}->{ASC}->{$slice};
        $set->Empty();
        asc2set($asc, $set);
        my $L = $CFG->{SLICE}->{MINLEVELS}->{$slice};
        for (my $i = $L+1; $i <= $CFG->{SLICE}->{MAXLEVEL}; $i++) {
            $set->Difference($set, $CFG->{SLICE}->{SET}->{OBJ}->{"DEF$i"});
        }
        $CFG->{SLICE}->{SET}->{OBJ}->{"NOV_$slice"} = $set->Clone();
    }

    if ($CFG->{OPT}->{X}) {
        foreach $slice (sort(keys(%{$CFG->{SLICE}->{SET}->{OBJ}}))) {
            $set = $CFG->{SLICE}->{SET}->{OBJ}->{$slice};
            if ($set->Norm > 0) {
                verbose("    slice `$slice': " . $set->to_ASCII() . "\n");
            }
            else {
                verbose("    slice `$slice': -Empty-\n");
            }
        }
    }
}


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

$CFG = {};
setup($CFG);
pass1($CFG);
pass2($CFG);
pass3($CFG);

exit(0);


##EOF##