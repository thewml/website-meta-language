##
##  slice_pass1.pl -- Pass 1
##  Copyright (c) 1997-2002 Ralf S. Engelschall.
##  Copyright (c) 1999-2002 Denis Barbier.
##

package main;

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
    sub alloclevel {
        my ($i);

        for ($i = 0; $i <= $CURRENT_LEVEL_SET->Max(); $i++) {
            last if (not $CURRENT_LEVEL_SET->bit_test($i));
        }
        $CURRENT_LEVEL_SET->Bit_On($i);
        return $i + 1;
    }

    #   clear the given level
    sub clearlevel {
        my ($i) = @_;

        $CURRENT_LEVEL_SET->Bit_Off($i - 1);
    }

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

            $L = alloclevel();                 # allocate next free level

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

            clearlevel($L);                         # de-allocate level

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

1;
##EOF##
