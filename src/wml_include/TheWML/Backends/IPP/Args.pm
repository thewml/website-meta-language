package TheWML::Backends::IPP::Args;

use strict;
use warnings;

use parent 'Exporter';

our @EXPORT_OK = (qw($IDENT_RE));

use Class::XSAccessor (
    accessors => +{
        map { $_ => $_ }
            qw(
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

    return;
}

our $IDENT_RE = qr/[a-zA-Z][a-zA-Z0-9_]*/;

sub setargs
{
    my ( $self, $arg, $str ) = @_;

STR:
    while ($str)
    {
        $str =~ s|^\s+||;
        last STR if ( $str eq '' );
        if ( $str =~ s|^($IDENT_RE)="([^"]*)"|| )
        {
            $arg->{$1} = $2;
        }
        elsif ( $str =~ s|^($IDENT_RE)=(\S+)|| )
        {
            $arg->{$1} = $2;
        }
        elsif ( $str =~ s|^($IDENT_RE)=\s+|| )
        {
            $arg->{$1} = '';
        }
        elsif ( $str =~ s|^($IDENT_RE)|| )
        {
            $arg->{$1} = 1;
        }
        else
        {
            $str = substr( $str, 1 );    # make sure the loop terminates
        }
    }

    return;
}
1;
