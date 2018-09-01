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

package WML_Frontends::Wml::WmlRc;

use 5.014;

use strict;
use warnings;

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            _main
            dir
            )
    }
);

use Cwd ();
use File::Basename qw/ basename dirname /;
use WML_Frontends::Wml::WmlRcDir ();
use WML_Frontends::Wml::Util qw/ _my_cwd /;

sub _process_wmlrc_dir
{
    my ( $self, $dir ) = @_;

    return WML_Frontends::Wml::WmlRcDir->new(
        _main => $self->_main,
        dir   => $dir
    )->_process_wmlrc_dir;
}

sub _process_wmlrc
{
    my ($self) = @_;

    if ( $self->_main->_opt_r )
    {
        return;
    }
    my $savedir = '';

    #   First save current directory and go to input file directory
    if ( not $self->_main->_opt_c and $self->_main->_src =~ m|/| )
    {
        $self->_main->_src( dirname( $self->_main->_src ) );
        if ( -d $self->_main->_src )
        {
            $savedir = Cwd::cwd;
            chdir( $self->_main->_src );
        }
    }
    $self->_main->_src('') if not $savedir;

    #   2. add all parent dirs .wmlrc files for options
    my $cwd = _my_cwd;
    my @DIR;
    while ($cwd)
    {
        push( @DIR, $cwd );
        $cwd =~ s#/[^/]+\z##;
    }

    #   Restore directory
    chdir($savedir) if $savedir;

    #   3. add ~/.wmlrc file for options
    my @pwinfo = getpwuid($<);
    my $home   = $pwinfo[7];
    $home =~ s#/\z##;
    if ( -f "$home/.wmlrc" )
    {
        push( @DIR, $home );
    }

    #   now parse these RC files
    foreach my $dir ( reverse(@DIR) )
    {
        $self->_process_wmlrc_dir($dir);
    }
    return;
}

1;
