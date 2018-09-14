##  WML -- Website META Language
##
##  Copyright (c) 1996-2001 Ralf S. Engelschall.
##  Copyright (c) 1999-2001 Denis Barbier.
package TheWML::Frontends::Wml::WmlRc;

use 5.014;

use strict;
use warnings;

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            _main
            )
    }
);

use Cwd ();
use File::Basename qw/ basename dirname /;
use TheWML::Frontends::Wml::WmlRcDir ();
use TheWML::Frontends::Wml::Util qw/ _my_cwd /;

sub _process_wmlrc_dir
{
    my ( $self, $dir ) = @_;

    return TheWML::Frontends::Wml::WmlRcDir->new(
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
