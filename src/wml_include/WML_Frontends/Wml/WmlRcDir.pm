##  WML -- Website META Language
##
##  Copyright (c) 1996-2001 Ralf S. Engelschall.
##  Copyright (c) 1999-2001 Denis Barbier.
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with this program; if not, write to
##
##      Free Software Foundation, Inc.
##      59 Temple Place - Suite 330
##      Boston, MA  02111-1307, USA
##
##  Notice, that ``free software'' addresses the fact that this program
##  is __distributed__ under the term of the GNU General Public License
##  and because of this, it can be redistributed and modified under the
##  conditions of this license, but the software remains __copyrighted__
##  by the author. Don't intermix this with the general meaning of
##  Public Domain software or such a derivated distribution label.
##
##  The author reserves the right to distribute following releases of
##  this program under different conditions or license agreements.

package WML_Frontends::Wml::WmlRcDir;

use 5.014;

use strict;
use warnings;

use WML_Frontends::Wml::Util qw/ canonize_path split_argv /;

sub new
{
    my $self = bless +{}, shift;

    return $self;
}

sub _process_wmlrc_dir
{
    my ( $obj, $self, $dir ) = @_;

    my $_pass_mgr = $self->_pass_mgr;

    if ( not -f "$dir/.wmlrc" )
    {
        return;
    }
    $_pass_mgr->verbose( 2, "Reading RC file: $dir/.wmlrc\n" );
    open( my $wml_rc_fh, '<', "$dir/.wmlrc" )
        or error("Unable to load $dir/.wmlrc: $!");
    my @aa;
WMLRC_LINES:
    while ( my $l = <$wml_rc_fh> )
    {
        if ( $l =~ m|\A\s*\n?\z| or $l =~ m|\A\s*#[#\s]*.*\z| )
        {
            next WMLRC_LINES;
        }
        $l =~ s|\A\s+||;
        $l =~ s|\s+\z||;
        $l =~ s|\$([A-Za-z_][A-Za-z0-9_]*)|$ENV{$1}|ge;
        push( @aa, split_argv($l) );
    }
    close($wml_rc_fh) || error("Unable to close $dir/.wmlrc: $!");
    my @opt_I_OLD = @{ $self->_opt_I };
    $self->_opt_I( [] );
    my $dnew = $self->_process_options( \@aa, [] );
    my @opt_I_NEW = @opt_I_OLD;

    #   adjust -D options
    my $reldir = File::Spec->abs2rel( "$dir", $self->_src );
    $reldir = "." if $reldir eq '';
    foreach my $d (@$dnew)
    {
        if ( my ( $var, $path ) = $d =~ m#\A([A-Za-z0-9_]+)~(.+)\z# )
        {
            if ( $path !~ m#\A/# )
            {
                canonize_path( \$path, $reldir );
            }
            $path = '""' if ( $path eq '' );
            $d = "$var=$path";
        }
        elsif ( $d =~ m|^([A-Za-z0-9_]+)$| )
        {
            $d .= '=1';
        }
        push( @{ $self->_opt_D }, $d );
    }

    #   adjust -I options
    $reldir = File::Spec->abs2rel("$dir");
    $reldir = "." if $reldir eq '';
    foreach my $path ( @{ $self->_opt_I } )
    {
        if ( $path !~ m#\A/# )
        {
            canonize_path( \$path, $reldir );
            $path = '.' if ( $path eq '' );
        }
        push( @opt_I_NEW, $path );
    }
    $self->_opt_I( [@opt_I_NEW] );
    return;
}

1;

__END__

# vim: ft=perl
