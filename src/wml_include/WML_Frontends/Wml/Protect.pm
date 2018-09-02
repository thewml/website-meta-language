##  WML -- Website META Language
##
##  Copyright (c) 1996-2001 Ralf S. Engelschall.
##  Copyright (c) 1999-2001 Denis Barbier.
package WML_Frontends::Wml::Protect;

use 5.014;

use strict;
use warnings;

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            _protect_storage
            )
    }
);

use IO::All qw/ io /;

sub _unprotect
{
    my ( $self, $fn, $pass ) = @_;

    my $data = io->file($fn)->all;
    while ( my ( $prefix, $key, $new ) =
        $data =~ m|^(.*?)-=P\[([0-9]+)\]=-(.*)$|s )
    {
        $data = $new;
        if ( $pass < 9 and $pass < $self->_protect_storage->{$key}->{MAX} )
        {
            $prefix .=
                "<protect" . $self->_protect_storage->{$key}->{ARG} . ">";
            $data = "</protect>" . $data;
        }
        $data = $prefix . $self->_protect_storage->{$key}->{BODY} . $data;
    }

    #    Remove useless <protect> tags
    $data =~ s|</?protect.*?>||gs if $pass == 9;
    io->file($fn)->print($data);
    if ( $pass < 9 )
    {
        foreach my $key ( keys %{ $self->_protect_storage } )
        {
            $self->_protect_storage->{$key} = undef;
        }
    }

    return;
}

1;
