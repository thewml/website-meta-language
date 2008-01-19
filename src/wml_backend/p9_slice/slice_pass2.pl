##
##  slice_pass2.pl -- Pass 2
##  Copyright (c) 1997-2002 Ralf S. Engelschall. 
##  Copyright (c) 1999-2002 Denis Barbier.
##

package main;

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
        @I = ($asc);
        @I = split(',', $asc) if (index($asc, ',') > 0);

        #   iterate over each interval and
        #   set the corresponding elements in the set
        foreach $interval (@I) {
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
    for ($i = 1; $i <= $CFG->{SLICE}->{MAXLEVEL}; $i++) {
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
        $L = $CFG->{SLICE}->{MINLEVELS}->{$slice};
        for ($i = $L+1; $i <= $CFG->{SLICE}->{MAXLEVEL}; $i++) {
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

1;
##EOF##
