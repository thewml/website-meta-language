##
##  IPP -- Include Pre-Processor
##  Copyright (c) 1997,1998,1999 Ralf S. Engelschall, All Rights Reserved.
##  Copyright (c) 2000 Denis Barbier, All Rights Reserved.
##

package WML_Backends::IPP::File;

use 5.014;

use strict;
use warnings;

use parent 'WML_Frontends::Wml::Base';

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            _main
            )
    },
);

use IO::All qw/ io /;
use WML_Frontends::Wml::Util qw/ canon_path /;
use WML_Backends::IPP::Line ();

sub ProcessFile
{
    my ( $self, $mode, $_del, $fn, $realname, $level, $no_id, $in_arg ) = @_;

    my $arg = +{%$in_arg};

    #   first check whether this is a filename pattern in which case
    #   we must expand it
    if ( my ( $dirname, $pattern, $ext ) =
        ( $fn =~ m/^(.*?)(?=[?*\]])([?*]|\[[^\]]*\])(.*)$/ ) )
    {
        return $self->_main->_expand_pattern( $dirname, $pattern, $ext, $mode,
            $_del, $level, $no_id, $arg );
    }
    if ( not $self->_main->_find_file( $_del, \$fn ) )
    {
        error("file not found: $fn");
    }

    #   stop if file was still included some time before
    if ( not $no_id )
    {
        my $id = canon_path($fn);
        if ( $mode eq 'use' )
        {
            return '' if ( exists $self->_main->INCLUDES->{$id} );
        }
        $self->_main->INCLUDES->{$id} = $_del->is_ang ? 1 : 2;
    }

    # Stop if just want to check dependency
    return '' if $mode eq 'depends';

    # Process the file
    $realname = $fn if $realname eq '';
    $self->_main->verbose( $level, "|" );
    $self->_main->verbose( $level, "+-- $fn" );
    my $in       = io()->file($fn);
    my $line_idx = 0;
    my $out      = '';
    if ( not $self->_main->opt_N and not $arg->{'IPP_NOSYNCLINES'} )
    {
        $out .=
              "<__file__ $realname /><__line__ 0 />"
            . "<protect pass=2><:# line $line_idx \"$realname\":></protect>\n";
    }
    my $store = '';

LINES:
    while ( my $l = $in->getline )
    {
        ++$line_idx;

        my $op = WML_Backends::IPP::Line->new(
            _main    => $self->_main,
            arg      => $arg,
            l        => \$l,
            line_idx => $line_idx,
            out      => \$out,
            realname => $realname,
        )->_process_line( \$store, $level, $fn, ) // '';
        if ( $op eq 'last' )
        {
            last LINES;
        }
        elsif ( $op eq 'redo' )
        {
            redo LINES;
        }
    }
    $out .= $store;

    return $out;
}

1;
