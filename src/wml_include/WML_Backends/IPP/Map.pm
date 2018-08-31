package WML_Backends::IPP::Map;

use strict;
use warnings;

use IO::All qw/ io /;

use Class::XSAccessor (
    accessors => +{
        map { $_ => $_ }
            qw(
            _map
            )
    },
);

sub new
{
    my $class = shift;

    my $self = bless {}, $class;

    $self->_init(@_);
    return $self;
}

sub _init
{
    my ( $self, $args ) = @_;

    $self->_map( +{} );

    return;
}

sub read_mapfile
{
    my ( $self, $mapfile ) = @_;

    my $MAP = $self->_map;
    my $fp  = io->file($mapfile);

LINES:
    while ( my $l = $fp->getline )
    {
        if ( $l =~ m|^\s*$| or m|^\s*#.*$| )
        {
            next LINES;
        }
        if ( my ( $given, $replace, $actiontype, $actiontext ) =
            $l =~ m|^(\S+)\s+(\S+)\s+\[\s*([SWE])\s*:\s*(.+?)\s*\].*$| )
        {
            foreach my $g ( split( /,/, $given ) )
            {
                $MAP->{$g} = {
                    REPLACE    => $replace,
                    ACTIONTYPE => $actiontype,
                    ACTIONTEXT => $actiontext,
                };
            }
        }
    }

    return;
}

sub mapfile
{
    my ( $self, $fn ) = @_;

    my $MAP = $self->_map;
    my $rec = $MAP->{$fn};
    if ( my $replace = $rec->{REPLACE} )
    {
        my $type = $rec->{ACTIONTYPE};
        my $text = $rec->{ACTIONTEXT};
        if ( $type eq 'S' )
        {
            $fn = $replace;
        }
        elsif ( $type eq 'W' )
        {
            warning("$fn: $text");
            $fn = $replace;
        }
        else
        {
            error("$fn: $text");
        }
    }
    return $fn;
}
1;
