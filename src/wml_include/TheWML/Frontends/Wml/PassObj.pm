##
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
##

package TheWML::Frontends::Wml::PassObj;

use strict;
use warnings;

use Carp::Always;

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{ map { $_ => $_ } qw( cb opt_pass time_ src_cb) }
);

use File::Basename qw/ basename /;
use File::Which qw/ which /;

#   remove escape backslashes
sub unquotearg
{
    my ($arg) = @_;
    $arg =~ s/\\([\$"`])/$1/g;
    return $arg;
}

sub dosource
{
    my ( $pass, $_pass_mgr, $prog, $args, $cb ) = @_;
    $_pass_mgr->verbose( 2, "source: $prog $args\n" );
    $_pass_mgr->verbose( 9, "loading: $prog\n" );
    if ( !defined( $pass->src_cb ) )
    {
        $pass->src_cb($cb);
    }

    $_pass_mgr->verbose( 9, "splitting from args: $args\n" );
    my @argv;
    while ($args)
    {
        redo
            if $args =~
            s|^\s*(-[a-zA-Z0-9]\S*)|push(@argv, unquotearg($1)), ''|egis;
        redo
            if $args =~
            s|^\s*"(.*?(?!\\).)"|push(@argv, unquotearg($1)), ''|egis;
        redo if $args =~ s|^\s*'([^']*)'|push(@argv, $1), ''|egis;
        redo if $args =~ s|^\s*(\S+)|push(@argv, unquotearg($1)), ''|egis;
        redo if $args =~ s|^\s+$|''|egis;
    }
    $_pass_mgr->verbose( 9, "splitting to argv: " . join( "|", @argv ) . "\n" );

    $_pass_mgr->verbose( 9, "running script\n" );
    my $rc = $pass->src_cb->(@argv);
    $rc //= '';
    $_pass_mgr->verbose( 9, "running script: rc=$rc\n" );
    $rc = 256 if not defined $rc;

    return $rc;
}

1;
