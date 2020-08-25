##  WML -- Website META Language
##
##  Copyright (c) 1996-2001 Ralf S. Engelschall.
##  Copyright (c) 1999-2001 Denis Barbier.
package TheWML::Frontends::Wml::Protect;

use 5.014;

use strict;
use warnings;

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            _PROTECT_COUNTER
            _firstpass
            _protect_storage
            )
    }
);

use List::Util qw/ max /;
use Path::Tiny qw/ path /;
use TheWML::Frontends::Wml::Util qw/ expandrange /;

sub _protect
{
    my ( $self, $fn, $pass ) = @_;

    my $data = path($fn)->slurp_raw;
    my $fp   = path($fn)->openw;

    #   First remove a shebang line
    if ( $self->_firstpass and $data =~ m/^#!wml/ )
    {
        while ( $data =~ s/^[^\n]*\\\n//s ) { 1; }
        $data =~ s/^[^\n]*\n//s;
    }

    #   Following passes will pass through previous test
    $self->_firstpass(0);

    #  This loop must take care of nestable <protect> tags
    while ( $data =~ s#\A(.*)<protect(.*?)>(.*?)</protect>##is )
    {
        my ( $prolog, $arg, $body ) = ( $1, $2, $3 );
        my $passes_str = '123456789';

        #    unquote the attribute
        $arg =~ s|(['"])(.*)\1\s*$|$2|;
        if ( $arg =~ m|pass=([0-9,-]*)|i )
        {
            $passes_str = $1;
            $passes_str =~ s|,||g;
            $passes_str = "1$passes_str" if $passes_str =~ m|^-|;
            $passes_str .= '9'           if $passes_str =~ m|-$|;
            $passes_str =~ s|([0-9])-([0-9])|expandrange($1, $2)|sge;
        }
        my $key = sprintf( "%06d", $self->_PROTECT_COUNTER );
        $self->_PROTECT_COUNTER( $self->_PROTECT_COUNTER + 1 );
        $self->_protect_storage->{$key} = {
            SPEC => $passes_str,
            MAX  => max( split( '', $passes_str ) ),
            ARG  => $arg,
            BODY => $body
        };
        $data = $prolog . "-=P[$key]=-" . $data;
    }

    #   And now unprotect passes
    while ( $data =~ s|^(.*?)-=P\[([0-9]+)\]=-||s )
    {
        my $key = $2;
        $fp->print($1);
        if ( $self->_protect_storage->{$key}->{SPEC} =~ m/$pass/ )
        {
            $fp->print("-=P[$key]=-");
        }
        else
        {
            $data =
                  "<protect"
                . $self->_protect_storage->{$key}->{ARG} . ">"
                . $self->_protect_storage->{$key}->{BODY}
                . "</protect>"
                . $data;
        }
    }
    $fp->print($data);
    $fp->close;
}

sub _unprotect
{
    my ( $self, $fn, $pass ) = @_;

    my $data = path($fn)->slurp_raw;
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
    path($fn)->spew_raw($data);
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
