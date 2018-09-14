package TheWML::Backends::IPP::Delimit;

use strict;
use warnings;

use parent 'Exporter';
our @EXPORT_OK = (qw(_delim));

use Class::XSAccessor (
    accessors => +{
        map { $_ => $_ }
            qw(
            delimiter
            )
    },
    constructor => 'new',
);

sub is
{
    my ( $self, $v ) = @_;

    return $self->delimiter eq $v;
}

sub is_ang
{
    my $self = shift;

    return $self->is('<');
}

sub is_quote
{
    my $self = shift;

    return $self->is_ang || $self->is(q/"/);
}

sub is_quote_all
{
    my $self = shift;

    return $self->is_quote || $self->is(q/'/);
}

sub _delim
{
    return TheWML::Backends::IPP::Delimit->new( delimiter => shift );
}

1;
