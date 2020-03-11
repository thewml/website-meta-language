##  divert -- Diversion Filter
##  Copyright (c) 1997-2001 Ralf S. Engelschall, All Rights Reserved.
##  Copyright (c) 1999-2001 Denis Barbier, All Rights Reserved.

package TheWML::Backends::Divert::Main;

use strict;
use warnings;
use 5.014;

use Getopt::Long ();

use parent 'TheWML::CmdLine::Base';

use List::Util qw(first min);
use List::MoreUtils qw(any);

use Class::XSAccessor (
    constructor => '_cons',
    accessors   => +{
        map { $_ => $_ }
            qw(
            _BUFFER
            _OVRWRITE
            _expand_stack
            _filename
            _in_fh
            _line
            _loc_stack
            _location
            _opt_o
            _opt_q
            _opt_v
            argv
            )
    },
);

sub usage
{
    STDERR->print(<<'EOF');
Usage: divert [options] [file]

Options:
  -o, --outputfile=<file>  set output file instead of stdout
  -q, --quiet              quiet mode (no warnings)
  -v, --verbose            verbose mode
EOF

    die;
}

sub _name
{
    return "Divert";
}

sub warning
{
    my ( $self, $filename, $line, $str ) = @_;
    if ( not $self->_opt_q )
    {
        STDERR->print("** Divert:Warning: ${filename}:$line: $str\n");
    }

    return;
}

sub new
{
    my $self = shift->_cons(@_);

    $self->_init;

    return $self;
}

sub _init
{
    my ($self) = @_;

    {
        my $opt_v = 0;
        my $opt_q = 0;
        my $opt_o = q{-};
        local $Getopt::Long::bundling      = 1;
        local $Getopt::Long::getopt_compat = 0;
        if (
            not Getopt::Long::GetOptionsFromArray(
                $self->argv,
                'v|verbose'      => \$opt_v,
                'q|quiet'        => \$opt_q,
                'o|outputfile=s' => \$opt_o,
            )
            )
        {
            usage();
        }
        $self->_opt_v($opt_v);
        $self->_opt_q($opt_q);
        $self->_opt_o($opt_o);
    }

    #
    #   open input file and read into buffer
    #
    {
        my $in;

        if (   ( ( @{ $self->argv } == 1 ) && ( $self->argv->[0] eq q{-} ) )
            || ( !@{ $self->argv } ) )
        {
            $in = IO::Handle->new;
            $self->_filename('STDIN');
            $in->fdopen( fileno(STDIN), 'r' )
                || $self->error("cannot load STDIN: $!");
        }
        elsif ( @{ $self->argv } == 1 )
        {
            open $in, '<', $self->_filename( $self->argv->[0] )
                or $self->error("cannot load @{[$self->_filename]}: $!");
        }
        else
        {
            usage();
        }
        $self->_in_fh($in);
    }

    ##
    ##   Pass 1: Parse the input data into disjunct location buffers
    ##           Each location buffer contains plain text blocks and
    ##           location pointers.
    ##

    $self->_location('main');         # currently active location
    $self->_loc_stack( ['null'] );    # stack of remembered locations
    $self->_BUFFER( +{ null => [], main => [], } );    # the location buffers
    $self->_OVRWRITE( +{} );                           # the overwrite flags
    $self->_line(0);
    $self->_expand_stack( [] );

    return;
}

sub _init_buffer
{
    my ( $self, $key ) = @_;

    if ( not exists( $self->_BUFFER->{$key} ) )
    {
        $self->_BUFFER->{$key} = [];
    }

    return;
}

sub _handle_location
{
    my ( $self, $m1 ) = @_;

    ##
    ##  Tag: dump location
    ##

    #   initialize new location buffer
    $self->_init_buffer($m1);

    #   insert location pointer into current location
    if ( $self->_BUFFER->{ $self->_location } == $self->_BUFFER->{$m1} )
    {
        $self->warning( $self->_filename, $self->_line,
                  'self-reference of location ``'
                . $self->_location
                . q{'' - ignoring} );
    }
    else
    {
        push( @{ $self->_BUFFER->{ $self->_location } },
            $self->_BUFFER->{$m1} );
    }

    return;
}

sub _handle_enter_location
{
    my ( $self, $m1 ) = @_;

    ##
    ##  Tag: enter location
    ##

    #   remember old location on stack
    push( @{ $self->_loc_stack }, $self->_location );

    #   determine location and optional
    #   qualifies, then enter this location
    $self->_location($m1);
    my $rewind_now  = 0;
    my $rewind_next = 0;

    if ( my ($new_loc) = $self->_location =~ m|^\!(.*)$| )
    {

        #   location should be rewinded now
        $self->_location($new_loc);
        $rewind_now = 1;
    }

    if ( my ($new_loc) = $self->_location =~ m|^(.*)\!$| )
    {

        #   location should be rewinded next time
        $self->_location($new_loc);
        $rewind_next = 1;
    }

    #   initialize location buffer
    $self->_init_buffer( $self->_location );

    #   is a "rewind now" forced by a "rewind next" from last time?
    if ( $self->_OVRWRITE->{ $self->_location } )
    {
        $rewind_now = 1;
        $self->_OVRWRITE->{ $self->_location } = 0;
    }

    #   remember a "rewind next" for next time
    $self->_OVRWRITE->{ $self->_location } = 1 if ($rewind_next);

    #   execute a "rewind now" by clearing the location buffer
    if ( $rewind_now == 1 )
    {
        $#{ $self->_BUFFER->{ $self->_location } } = -1;
    }
    return;
}

sub _handle_leave_location
{
    my ( $self, $m1 ) = @_;

    ##
    ##  Tag: leave location
    ##

    if ( !@{ $self->_loc_stack } )
    {
        $self->warning( $self->_filename, $self->_line,
            q{already in ``null'' location -- ignoring leave} );
    }
    else
    {
        my $loc = ( $1 // '' );
        if ( $loc eq 'null' )
        {
            $self->warning( $self->_filename, $self->_line,
                      q{cannot leave ``null'' location }
                    . q{-- ignoring named leave} );
        }
        elsif ( $loc ne '' and $loc ne $self->_location )
        {

            #   leave the named location and all locations
            #   on the stack above it.
            my $n = -1;
            for ( my $i = $#{ $self->_loc_stack } ; $i >= 0 ; --$i )
            {
                if ( $self->_loc_stack->[$i] eq $loc )
                {
                    $n = $i;
                    last;
                }
            }
            if ( $n == -1 )
            {
                $self->warning( $self->_filename, $self->_line,
qq{no such currently entered location ``$loc'' -- ignoring named leave}
                );
            }
            else
            {
                splice( @{ $self->_loc_stack }, $n );
                $self->_location( pop( @{ $self->_loc_stack } ) );
            }
        }
        else
        {
            #   leave just the current location
            $self->_location( pop( @{ $self->_loc_stack } ) );
        }
    }

    return;
}

sub _handle_plain_text
{
    my ( $self, $remain_ref ) = @_;

    ##
    ##  Plain text
    ##

    #   calculate the minimum amount of plain characters we can skip
    my $l  = length( ${$remain_ref} );
    my $i1 = index( ${$remain_ref}, '<<' );
    $i1 = $l if $i1 == -1;

    #   Skip ../ which is often used in URLs
    my $i2 = -1;
    do
    {
        $i2 = index( ${$remain_ref}, '..', $i2 + 1 );
        } while ( $i2 > -1
        and $i2 + 2 < $l
        and substr( ${$remain_ref}, $i2 + 2, 1 ) eq '/' );
    $i2 = $l if $i2 == -1;

    my $i3 = index( ${$remain_ref}, '{#' );
    $i3 = $l if $i3 == -1;    #}
    my $i4 = index( ${$remain_ref}, ':#' );
    $i4 = $l if $i4 == -1;

    my $i = min( $i1, $i2, $i3, $i4 );

    #   skip at least 2 characters if we are sitting
    #   on just a "<<", "..", "{#" or ":#"
    $i = 1 if ( $i == 0 );

    #   append plain text to remembered data and adjust ${$remain_ref}
    #   variable
    if ( $i == $l )
    {
        push( @{ $self->_BUFFER->{ $self->_location } }, ${$remain_ref} );
        ${$remain_ref} = '';
    }
    else
    {
        #   substr with 4 arguments was introduced in perl 5.005
        push(
            @{ $self->_BUFFER->{ $self->_location } },
            substr( ${$remain_ref}, 0, $i )
        );
        substr( ${$remain_ref}, 0, $i ) = '';
    }

    return;
}

sub _run
{
    my ($self) = @_;

    while ( defined( my $remain = $self->_in_fh->getline ) )
    {
        $self->_line( $self->_line + 1 );
        while ( length $remain > 0 )
        {

            if ( $remain =~ s|^<<([a-zA-Z][a-zA-Z0-9_]*)>>|| )
            {
                $self->_handle_location($1);
            }
            elsif ( $remain =~ s|^{#([a-zA-Z][a-zA-Z0-9_]*)#}|| )
            {
                $self->_handle_location($1);
            }
            elsif ( $remain =~ s|^\.\.(\!?[a-zA-Z][a-zA-Z0-9_]*\!?)>>|| )
            {
                $self->_handle_enter_location($1);
            }
            elsif ( $remain =~ s|^{#(\!?[a-zA-Z][a-zA-Z0-9_]*\!?)#:|| )
            {
                $self->_handle_enter_location($1);
            }
            elsif ($remain =~ s|^<<([a-zA-Z][a-zA-Z0-9_]*)?\.\.||
                or $remain =~ s|^:#([a-zA-Z][a-zA-Z0-9_]*)?#}|| )
            {
                $self->_handle_leave_location($1);
            }
            else
            {
                $self->_handle_plain_text( \$remain );
            }
        }
    }
    $self->_in_fh->close();

    return;
}

sub _expand_diversion
{
    my ( $self, $loc ) = @_;

    #   check for recursion by making sure
    #   the current location has not already been seen.
    if ( any { $_ == $loc } @{ $self->_expand_stack } )
    {

        #   find name of location via location pointer
        #   for human readable warning message
        my $name = (
            (
                first { $self->_BUFFER->{$_} == $loc }
                keys( %{ $self->_BUFFER } )
            ) // 'unknown'
        );
        $self->warning( $self->_filename, $self->_line,
            "recursion through location ``$name'' - break" );
        return '';
    }

    #   ok, location still not seen,
    #   but remember it for recursive calls.
    push( @{ $self->_expand_stack }, $loc );

    #   recursively expand the location
    #   by stepping through its list elements
    my $data = '';
    foreach my $el ( @{$loc} )
    {
        $data .= ref($el) ? $self->_expand_diversion($el) : $el;
    }

    #   we can remove the location from
    #   the stack because we are back from recursive calls.
    pop( @{ $self->_expand_stack } );

    #   return expanded buffer
    return $data;
}

sub calc_result
{
    my ($self) = @_;

    $self->_run;

    ##
    ##   Pass 2: Recursively expand the location structure
    ##           by starting from the main location buffer
    ##
    return $self->_expand_diversion( $self->_BUFFER->{'main'} );
}

sub main
{
    my ($self) = @_;
    $self->_out( $self->_opt_o(), [ $self->calc_result ] );
    return;
}

1;
