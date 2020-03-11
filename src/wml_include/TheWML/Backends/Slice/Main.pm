package TheWML::Backends::Slice::Main;

use strict;
use warnings;

use SliceTermParser     ();
use TheWML::CmdLine::IO ();

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            _vars
            _CFG
            argv
            )
    },
);

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

use Getopt::Long 2.13 ();
use Bit::Vector 5.0   ();
use List::Util qw/ first max /;

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

sub verbose
{
    my ( $self, $str ) = @_;

    if ( $self->{OPT}->{X} )
    {
        $str =~ s|^|** Slice:Verbose: |mg;
        print STDERR $str;
    }

    return;
}

sub printerror
{
    my ($str) = @_;

    $str =~ s|^|** Slice:Error: |mg;
    print STDERR $str;

    return;
}

sub error
{
    my ($str) = @_;

    printerror($str);
    exit(1);
}

sub printwarning
{
    my ($str) = @_;

    $str =~ s|^|** Slice:Warning: |mg;
    print STDERR $str;

    return;
}

sub usage
{
    print STDERR <<'EOF';
Usage: slice [options] [file]

Options:
  -o, --outputfile=FILESPEC  create output file(s)
  -y, --output-policy=STRING set default output policy
  -v, --verbose              enable verbose mode
  -V, --version              display version string
  -h, --help                 display usage page

FILESPEC format:

  [SLICETERM:]PATH[@CHMODOPT]

  SLICETERM ..... a set-theory term describing the slices
  PATH .......... a filesystem path to the outputfile
  CHMODOPT ...... permission change options for 'chmod'
EOF
    exit(1);
}

sub hello
{
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

use vars qw( $opt_h $opt_y $opt_V $opt_v @opt_o );

sub setup
{
    my ($self) = @_;
    $self->_CFG( +{} );

    #   parse command line options
    $opt_h = 0;
    $opt_V = 0;
    $opt_v = 0;
    $opt_y = '';
    @opt_o = ();
    my @options_list = (
        "v|verbose", "V|version",
        "h|help",    "o|outputfile=s@",
        "y|output-policy=s",
    );
    $SIG{'__WARN__'} = sub {
        print STDERR "Slice:Error: $_[0]";
    };

    $Getopt::Long::bundling      = 1;
    $Getopt::Long::getopt_compat = 0;
    if ( not Getopt::Long::GetOptionsFromArray( $self->argv, @options_list ) )
    {
        print STDERR "Try `$0 --help' for more information.\n";
        exit(0);
    }
    $SIG{'__WARN__'} = undef;
    usage() if ($opt_h);
    hello() if ($opt_V);

    #   process command line arguments and
    #   read input file
    my $INPUT = TheWML::CmdLine::IO->input( $self->argv, \&usage );

    #   add additional options
    $INPUT =~
        s|^%!slice\s+(.*?)\n|push(@{$self->argv}, split(' ', $1)), ''|egim;
    if ( not Getopt::Long::GetOptionsFromArray( $self->argv, @options_list ) )
    {
        usage;
    }
    if ( !@opt_o )
    {
        @opt_o = ("ALL:-");    # default is all on stdout
    }

    #   setup the $CFG hash
    $self->_CFG->{INPUT}          = {};
    $self->_CFG->{INPUT}->{SRC}   = $INPUT;    # original source
    $self->_CFG->{INPUT}->{PLAIN} = '';        # source without slice delimiters
    $self->_CFG->{OPT}            = {
        X => $opt_v,
        O => [@opt_o],
        Y => {
            'u' => 0,
            'w' => 0,
            'z' => 0,
            's' => 0,
        }
    };
    my $modifier = $opt_y;

    foreach my $opt (qw(u w z s))
    {
        if ( $modifier =~ m/\Q$opt\E([0-9]+)/ )
        {
            $self->_CFG->{OPT}->{Y}->{$opt} = $1;
        }
    }
    $self->_CFG->{SLICE} = {
        SET => {
            ASC => {},    # slice set, represented in ASCII
            OBJ => {},    # slice set, represented as Bit::Vector object
        },
        MINLEVELS => {},    # slice min levels
        MAXLEVEL  => 0,     # maximum slice level
    };
}

##  Pass 1: Determine delimiters
sub pass1
{
    my ($self) = @_;
    $self->verbose("\nPass 1: Determine delimiters\n\n");

    my @CURRENT_SLICE_NAMES;
    my %CURRENT_LEVEL_BRAIN;
    my $CURRENT_LEVEL_SET = Bit::Vector->new(512);

    #   allocate the next free level starting from 1
    my $alloclevel = sub {
        my $max = $CURRENT_LEVEL_SET->Max();
        my $i =
            ( ( first { !$CURRENT_LEVEL_SET->bit_test($_) } ( 0 .. $max ) )
            // max( 0, $max + 1 ) );
        $CURRENT_LEVEL_SET->Bit_On($i);
        return $i + 1;
    };

    #   clear the given level
    my $clearlevel = sub {
        my ($i) = @_;

        $CURRENT_LEVEL_SET->Bit_Off( $i - 1 );
    };

    my $INPUT = $self->_CFG->{INPUT}->{SRC};
    my $open  = 0;
    my $pos   = 0;
    my $prev  = 0;
    my $ID_RE = qr/[A-Z][_A-Z0-9]*/;
    while (
        $INPUT =~ m/
             (?=[\[:])                 #  Consider only sequences beginning
                                       #  with `[' or `:'
             (?:\[($ID_RE):   #  Begin delimiter
                      |
             :($ID_RE)?\])    #  End delimiter
                      /gx
        )
    {

        if ( defined($1) )
        {
            #
            #   begin delimiter found
            #
            my $name     = $1;
            my $inputpos = pos($INPUT);
            my $prolog =
                substr( $INPUT, $prev, $inputpos - $prev - length($name) - 2 );

            #   add prolog
            $self->_CFG->{INPUT}->{PLAIN} .= $prolog;

            #   and store position of next character in input datas
            $pos += length($prolog);
            $prev = $inputpos;

            my $L = $alloclevel->();    # allocate next free level

            push( @CURRENT_SLICE_NAMES, $name )
                ;                       # remember name  for end delimiter
            $CURRENT_LEVEL_BRAIN{"$name"} .=
                ":$L";                  # remember level for end delimiter
            $self->_CFG->{SLICE}->{MINLEVELS}->{"$name"} //= '';
            if (   $self->_CFG->{SLICE}->{MINLEVELS}->{"$name"} eq ''
                or $self->_CFG->{SLICE}->{MINLEVELS}->{"$name"} > $L )
            {
                $self->_CFG->{SLICE}->{MINLEVELS}->{"$name"} = $L;
            }

            #  now begin entry with LEVEL:START
            $self->_CFG->{SLICE}->{SET}->{ASC}->{"$name:$L"} .=
                ( $self->_CFG->{SLICE}->{SET}->{ASC}->{"$name:$L"} ? ',' : '' )
                . "$L:$pos";

            #  adjust notice about highest level
            $self->_CFG->{SLICE}->{MAXLEVEL} = (
                  $self->_CFG->{SLICE}->{MAXLEVEL} < $L
                ? $L
                : $self->_CFG->{SLICE}->{MAXLEVEL}
            );

            $self->verbose("    slice `$name': begin at $pos, level $L\n");

            ++$open;
        }
        elsif ( $open > 0 )
        {
            #
            #   end delimiter found
            #
            my $name     = ( $2 // '' );
            my $inputpos = pos($INPUT);
            my $prolog =
                substr( $INPUT, $prev, $inputpos - $prev - length($name) - 2 );

            #   add prolog
            $self->_CFG->{INPUT}->{PLAIN} .= $prolog;

            #   and store position of next character in input datas
            $pos += length($prolog) - 1;
            $prev = $inputpos;

            my $namex = pop(@CURRENT_SLICE_NAMES);    # take remembered name
            $name = $namex
                if ( $name eq '' );    # fill name because of shortcut syntax
            $CURRENT_LEVEL_BRAIN{"$name"} =~
                s|:([0-9]+)\z||;       # take remembered level
            my $L = $1;

            $clearlevel->($L);         # de-allocate level

            # now end entry with :END
            $self->_CFG->{SLICE}->{SET}->{ASC}->{"$name:$L"} .= ":$pos";

            $self->verbose("    slice `$name': end at $pos, level $L\n");

            ++$pos;
            --$open;
        }
    }

    # add all remaining input
    $self->_CFG->{INPUT}->{PLAIN} .= substr( $INPUT, $prev );

    #   check: were all opened slices really closed?
    if ( $CURRENT_LEVEL_SET->Norm > 0 )
    {
        my $i;
        my $err = '';
        for my $i ( 0 .. $CURRENT_LEVEL_SET->Max() )
        {
            if ( $CURRENT_LEVEL_SET->bit_test($i) )
            {
                foreach my $name ( keys(%CURRENT_LEVEL_BRAIN) )
                {
                    if ( $CURRENT_LEVEL_BRAIN{$name} == ( $i + 1 ) )
                    {
                        $err .= " `$name'";
                    }
                }
            }
        }
        error("Some slices were not closed:$err\n");
    }
}

sub _asc2set
{
    my ( $asc, $set, $onlylevel, $notcleared ) = @_;
    $onlylevel  //= '';
    $notcleared //= 0;

    $set->Empty() if ( not $notcleared );
    return $set
        if ( $asc =~ m|\A[0-9]+:0:-1\z| );    # string represents the empty set

    #   iterate over each interval and
    #   set the corresponding elements in the set
    foreach my $interval (
        ( index( $asc, ',' ) > 0 ) ? split( ',', $asc ) : ($asc) )
    {
        my ( $level, $from, $to ) =
            ( $interval =~ m|\A([0-9]+):([0-9]+):([0-9]+)\z| );
        next if ( ( $onlylevel ne '' ) and ( $level != $onlylevel ) );
        next if ( $from > $to );
        $set->Interval_Fill( $from, $to );
    }
}

sub _get_var
{
    my ( $self, $name ) = @_;

    return $self->_vars->{$name};
}

sub _set_var
{
    my ( $self, $name, $val ) = @_;

    return $self->_vars->{$name} = $val;
}

sub _complement_var
{
    my ( $self, $v1 ) = @_;

    $self->_get_var($v1)->Complement( $self->_get_var($v1) );

    return;
}

sub _mutate_var
{
    my ( $self, $v1, $op, $v3 ) = @_;

    $self->_get_var($v1)->$op( $self->_get_var($v1), $self->_get_var($v3) );

    return;
}

##  Pass 2: Calculation of slice sets
sub pass2
{
    my ($self) = @_;
    $self->verbose("\nPass 2: Calculation of slice sets\n\n");

    my $n       = length( $self->_CFG->{INPUT}->{PLAIN} ) + 1;
    my $set     = Bit::Vector->new($n);                          # working set
    my $setA    = Bit::Vector->new($n);                          # "all" set
    my $ASC_SET = $self->_CFG->{SLICE}->{SET}->{ASC};
    my $OBJ_SET = $self->_CFG->{SLICE}->{SET}->{OBJ};

    #   restore slice names
    foreach my $slice ( keys( %{$ASC_SET} ) )
    {
        my $asc = delete $ASC_SET->{$slice};
        $slice =~ s%:[0-9]+\z%%g;
        $ASC_SET->{$slice} .=
            ( $ASC_SET->{"$slice"} ? ',' : '' ) . $asc;
    }

    #   convert ASCII representation to real internal set objects
    foreach my $slice ( keys( %{$ASC_SET} ) )
    {
        $set->Empty();
        _asc2set( $ASC_SET->{$slice}, $set );
        $OBJ_SET->{$slice} = $set->Clone();
    }

    #   define the various (un)defined slice areas
    $set->Fill();
    $OBJ_SET->{'UNDEF0'} = $set->Clone();
    $set->Empty();
    $OBJ_SET->{'DEF0'} = $set->Clone();
    $setA->Empty();
    for my $i ( 1 .. $self->_CFG->{SLICE}->{MAXLEVEL} )
    {
        $set->Empty();
        foreach my $slice ( keys( %{$ASC_SET} ) )
        {
            _asc2set( $ASC_SET->{$slice}, $set, $i, 1 )
                ;    # load $set with entries of level $i
            $setA->Union( $setA, $set );    # add to $setA these entries
        }
        $OBJ_SET->{"DEF$i"} = $set->Clone();
        $set->Complement($set);
        $OBJ_SET->{"UNDEF$i"} = $set->Clone();
    }
    $OBJ_SET->{'DEF'} = $setA->Clone();
    $setA->Complement($setA);
    $OBJ_SET->{'UNDEF'} = $setA->Clone();
    $OBJ_SET->{'ALL'}   = $OBJ_SET->{'UNDEF0'};

    #   define the various slice areas which are not overwritten
    foreach my $slice ( keys( %{$ASC_SET} ) )
    {
        $set->Empty();
        _asc2set( $ASC_SET->{$slice}, $set );
        my $L = $self->_CFG->{SLICE}->{MINLEVELS}->{$slice};
        for my $i ( ( $L + 1 ) .. $self->_CFG->{SLICE}->{MAXLEVEL} )
        {
            $set->Difference( $set, $OBJ_SET->{"DEF$i"} );
        }
        $OBJ_SET->{"NOV_$slice"} = $set->Clone();
    }

    if ( $self->_CFG->{OPT}->{X} )
    {
        foreach my $slice ( sort( keys( %{$OBJ_SET} ) ) )
        {
            $set = $OBJ_SET->{$slice};
            if ( $set->Norm > 0 )
            {
                $self->verbose(
                    "    slice `$slice': " . $set->to_ASCII() . "\n" );
            }
            else
            {
                $self->verbose("    slice `$slice': -Empty-\n");
            }
        }
    }
}

##  Pass 3: Output generation
sub _calc_entry_output_params
{
    my ($entry) = @_;
    if ( $entry =~ m|^([_A-Z0-9~!+u%n\-\\^x*{}()@]+):(.+)@(.+)$| )
    {
        # full syntax
        return ( $1, $2, $3 );
    }
    elsif ( $entry =~ m|^([_A-Z0-9~!+u%n\-\\^x*{}()@]+):(.+)$| )
    {
        # only slice and file
        return ( $1, $2, '' );
    }
    elsif ( $entry =~ m|^([^@]+)@(.+)$| )
    {
        # only file and chmod
        return ( 'ALL', $1, $2 );
    }
    else
    {
        # only file
        return ( 'ALL', $entry, '' );
    }
}

sub pass3
{
    my ($self) = @_;

    $self->verbose("\nPass 3: Output generation\n\n");

    foreach my $entry ( @{ $self->_CFG->{OPT}->{O} } )
    {

        #   determine skip options:
        #     u: a set is undefined
        #     w: a wildcard set does not match
        #     z: result is empty
        #     s: result is only composed of whitespaces
        my $status = $self->_CFG->{OPT}->{Y};
        if ( $entry =~ s|\#([suwz0-9]+)\z|| )
        {
            my $modifier = $1;
            foreach (qw(u w z s))
            {
                ( $modifier =~ m/$_([0-9]+)/ ) and $status->{$_} = $1;
            }
        }
        my ( $slice, $outfile, $chmod ) = _calc_entry_output_params($entry);
        $self->verbose(
            "    file `$outfile': sliceterm='$slice', chmodopts='$chmod'\n");

        #   parse the sliceterm and create corresponding
        #   Perl 5 statement containing Bit::Vector calls
        my ( $cmds, $var ) = SliceTerm::Parse( $self->_CFG, $slice, $status );
        $var //= '';

        #   skip file if requested by options
        if ( $status->{u} > 0 and !defined($cmds) )
        {
            printwarning("Undefined set: skip generation of $outfile\n");
            next if $status->{u} > 1;
        }

        #   just debugging...
        if ( $self->_CFG->{OPT}->{X} )
        {
            $self->verbose("        calculated Perl 5 set term:\n");
            $self->verbose("        ----\n");
            my $x = $cmds;
            $self->verbose("        $x\n");
            $self->verbose("        ----\n");
        }

        my $CFG = $self->_CFG;
        $self->_vars( {} );

        #   now evaluate the Bit::Vector statements
        #   and move result to $set
        foreach my $cmd (@$cmds)
        {
            $cmd->( $self, $CFG );
        }
        my $set = $self->_get_var($var);

        my $start = 0;
        my $out   = '';
        if ( defined($set) )
        {
            #   now scan the set and write out characters
            #   which have a corresponding bit set.
            while (( $start < $set->Size() )
                && ( my ( $min, $max ) = $set->Interval_Scan_inc($start) ) )
            {
                $out .= substr( $self->_CFG->{INPUT}->{PLAIN},
                    $min, ( $max - $min + 1 ) );
                $start = $max + 2;
            }
        }

        #   skip file if requested by options
        if ( $status->{z} > 0 and $out eq '' )
        {
            printwarning("Empty output: skip generation of $outfile\n");
            main::error("Execution stopped\n") if $status->{z} > 2;
            next                               if $status->{z} == 2;
        }
        if ( $status->{'s'} > 0 and ( $out eq '' or $out !~ m/\S/ ) )
        {
            printwarning("Whitespace only: skip generation of $outfile\n");
            main::error("Execution stopped\n") if $status->{'s'} > 2;
            next                               if $status->{'s'} == 2;
        }

        TheWML::CmdLine::IO->out( $outfile, [$out] );

        #   additionally run chmod on the output file
        if ( $outfile ne '-' and $chmod ne '' and -f $outfile )
        {
            system("chmod $chmod $outfile");
        }
    }
}

sub main
{
    my $self = shift;

    $self->setup;
    $self->pass1;
    $self->pass2;
    $self->pass3;

    return 0;
}

1;

##EOF##
