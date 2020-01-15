package TheWML::Backends::IPP::Map;

use strict;
use warnings;

use Path::Tiny qw/ path /;

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
    $self->read_multi_map_files( $args->{filenames} );

    return;
}

sub read_multi_map_files
{
    my ( $self, $map_fns ) = @_;

    foreach my $fn (@$map_fns)
    {
        $self->read_mapfile($fn);
    }

    return;
}

sub read_mapfile
{
    my ( $self, $mapfile ) = @_;

    my $MAP = $self->_map;
    my $fp  = path($mapfile)->openr;

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
