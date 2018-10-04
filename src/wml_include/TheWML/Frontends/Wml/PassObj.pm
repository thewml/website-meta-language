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
    accessors   => +{ map { $_ => $_ } qw( idx cb opt_pass time_ src_cb) }
);

use File::Basename qw/ basename /;
use File::Which qw/ which /;
use IO::All qw/ io /;

sub precompile
{
    my ( $name, $in ) = @_;

    my $error = '';
    local $SIG{__WARN__} = sub { $error .= $_[0]; };
    local $SIG{__DIE__};

    $in =~ s|exit(\s*\(0\))|return$1|sg;
    $in =~ s|exit(\s*\([^0].*?\))|die$1|sg;
    eval( "no strict; no warnings; package $name; \$main = sub { \@ARGV = \@_; "
            . $in
            . "; return 0; }; package main;" );
    $error = "$@" if ($@);
    my $func = eval("no strict; no warnings; \$${name}::main");

    $@ = $error || '';
    return ( $func, $@ );
}

#   remove escape backslashes
sub unquotearg
{
    my ($arg) = @_;
    $arg =~ s/\\([\$"`])/$1/g;
    return $arg;
}

sub dosource
{
    my ( $pass, $_pass_mgr, $prog, $args ) = @_;
    $_pass_mgr->verbose( 2, "source: $prog $args\n" );
    $_pass_mgr->verbose( 9, "loading: $prog\n" );
    if ( !defined( $pass->src_cb ) )
    {
        my $pkgname = basename($prog);
        if ( $prog !~ m|\A/| )
        {
            $prog = which($prog);
        }
        my $src = io->file($prog)->all;
        $_pass_mgr->verbose( 9,
            "loading: succeeded with $prog (" . length($src) . " bytes)\n" );

        $_pass_mgr->verbose( 9, "precompiling script: pkgname=$pkgname\n" );
        my ( $func, $error ) = precompile( $pkgname, $src );
        if ( $error ne '' )
        {
            die $error;
            $_pass_mgr->verbose( 9, "precompiling script: error: $error\n" );
        }
        else
        {
            $_pass_mgr->verbose( 9, "precompiling script: succeeded\n" );
        }
        $pass->src_cb($func);
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
