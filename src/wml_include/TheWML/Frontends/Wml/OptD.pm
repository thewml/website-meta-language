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
package TheWML::Frontends::Wml::OptD;

use 5.014;

use strict;
use warnings;

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            _main
            _opt_D
            )
    }
);

use Path::Tiny qw/ path /;

use TheWML::Config qw//;
use TheWML::Frontends::Wml::Util
    qw/ _my_cwd canonize_path quotearg time_record usage user_record /;

sub _populate_opt_D
{
    my ($self) = @_;

    my $_pass_mgr = $self->_main->_pass_mgr;

    my $gen_user     = user_record($<);
    my $gen_time_rec = time_record( time() );

    my ( $src_dirname, $src_basename, $src_time_rec, $src_user );
    my $cwd = _my_cwd;

    if ( $self->_main->_src_istmp )
    {
        $src_dirname  = $cwd;
        $src_basename = $self->_main->_src_filename('STDIN');
        $src_time_rec = $gen_time_rec;
        $src_user     = $gen_user;
    }
    else
    {
        $src_dirname = (
            ( $self->_main->_src =~ m#/# )
            ? path( $self->_main->_src )->parent->absolute->stringify
            : $cwd
        );
        $src_basename =
            $self->_main->_src_filename( path( $self->_main->_src )->basename )
            =~ s#(\.[a-zA-Z0-9]+)\z##r;
        my $stat = path( $self->_main->_src )->stat;
        $src_time_rec = time_record( $stat->mtime );
        $src_user     = user_record( $stat->uid );
    }

    unshift(
        @{ $self->_opt_D },
        "WML_SRC_DIRNAME=$src_dirname",
        "WML_SRC_FILENAME=" . $self->_main->_src_filename,
        "WML_SRC_BASENAME=$src_basename",
        "WML_SRC_TIME=$src_time_rec->{time}",
        "WML_SRC_CTIME=$src_time_rec->{ctime}",
        "WML_SRC_ISOTIME=$src_time_rec->{isotime}",
        "WML_SRC_GMT_CTIME=$src_time_rec->{gmt_ctime}",
        "WML_SRC_GMT_ISOTIME=$src_time_rec->{gmt_isotime}",
        "WML_SRC_USERNAME=$src_user->{username}",
        "WML_SRC_REALNAME=$src_user->{realname}",
        "WML_GEN_TIME=$gen_time_rec->{time}",
        "WML_GEN_CTIME=$gen_time_rec->{ctime}",
        "WML_GEN_ISOTIME=$gen_time_rec->{isotime}",
        "WML_GEN_GMT_CTIME=$gen_time_rec->{gmt_ctime}",
        "WML_GEN_GMT_ISOTIME=$gen_time_rec->{gmt_isotime}",
        "WML_GEN_USERNAME=$gen_user->{username}",
        "WML_GEN_REALNAME=$gen_user->{realname}",
        "WML_GEN_HOSTNAME=@{[$_pass_mgr->gen_hostname]}",
        'WML_LOC_PREFIX=' . TheWML::Config::prefix(),
        "WML_LOC_BINDIR=" . $self->_main->bindir,
        "WML_LOC_DATADIR=" . TheWML::Config::datadir(),
        "WML_LOC_LIBDIR=" . TheWML::Config::libdir(),
        'WML_LOC_MANDIR=' . TheWML::Config::mandir(),
        "WML_VERSION=@{[TheWML::Config::_VERSION]}",
        "WML_TMPDIR=" . $self->_main->_tmpdir
    );

    return;
}

sub _process_opt_D
{
    my ($self) = @_;

    #   7. Undefine variables when requested
    my %new_opt_D;
    foreach my $d ( @{ $self->_opt_D } )
    {
        if ( my ( $var, $val ) = ( $d =~ m|^(.+?)=(.*)$| ) )
        {
            if ( $val eq 'UNDEF' )
            {
                delete $new_opt_D{$var};
            }
            else
            {
                $new_opt_D{$var} = $val;
            }
        }
    }
    @{ $self->_opt_D } = map { $_ . "=" . $new_opt_D{$_} } keys %new_opt_D;
    return;
}

sub _adjust_opt_D
{
    my ( $self, $dnew ) = @_;

    my $reldir = $self->_main->_calc_reldir;
    foreach my $d ( map { quotearg $_} @$dnew )
    {
        if ( my ( $var, $path ) = $d =~ m|^([A-Za-z0-9_]+)~(.+)$| )
        {
            if ( $path !~ m|^/| )
            {
                canonize_path( \$path, $reldir );
            }
            $path = '""' if ( $path eq '' );
            $d    = "$var=$path";
        }
        elsif ( $d =~ m|^([A-Za-z0-9_]+)$| )
        {
            $d .= '=1';
        }
        push( @{ $self->_opt_D }, $d );
    }
    return;
}

1;

__END__

# vim: ft=perl
