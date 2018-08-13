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
package WML_Frontends::Wml::PassesManager;

use strict;
use warnings;

use Class::XSAccessor (
    accessors => +{
        map { $_ => $_ }
            qw( _passes _process_argv_cb gen_hostname libdir out_istmp opt_v opt_o opt_s )
    }
);

use WML_Frontends::Wml::PassObj;

use WML_Frontends::Wml::Util qw/ _my_cwd error split_argv /;
use IO::All qw/ io /;

my $RESOLV_FN = '/etc/resolv.conf';

sub new
{
    my $self = bless +{}, shift;
    my $args = shift;
    $self->libdir( $args->{libdir} );
    my $__PASSES = [
        sub { return shift->pass1(@_); },
        sub { return shift->pass2(@_); },
        sub { return shift->pass3(@_); },
        sub { return shift->pass4(@_); },
        sub { return shift->pass5(@_); },
        sub { return shift->pass6(@_); },
        sub { return shift->pass7(@_); },
        sub { return shift->pass8(@_); },
        sub { return shift->pass9(@_); },
    ];
    $self->_passes(
        [
            '',
            map {
                WML_Frontends::Wml::PassObj->new(
                    idx => ( $_ + 1 ),
                    cb  => $__PASSES->[$_]
                    )
                }
                keys @$__PASSES

        ]
    );
    my $gen_hostname = `hostname`;
    $gen_hostname =~ s|\n$||;
    $gen_hostname ||= 'UNKNOWN-HOSTNAME';

    if ( $gen_hostname =~ /\A[a-zA-Z0-9_-]+\z/
        and -f $RESOLV_FN )
    {
        open( my $tmp_fh, '<', $RESOLV_FN )
            or error("Unable to load $RESOLV_FN: $!");
    RESOLV_LOOP:
        while ( my $l = <$tmp_fh> )
        {
            if ( my ($domain) = $l =~ m|\Adomain\s+\.?(\S+)| )
            {
                $gen_hostname .= ".$domain";
                last RESOLV_LOOP;
            }
        }
        close($tmp_fh)
            or error("Unable to close $RESOLV_FN: $!");
    }
    $self->gen_hostname($gen_hostname);
    return $self;
}

sub pass
{
    my ( $self, $i ) = @_;

    return $self->_passes->[$i];
}

*verbose = \&::verbose;

sub dosystem
{
    my ( $self, $cmd ) = @_;
    verbose( 2, "system: $cmd\n" );
    return scalar system($cmd);
}

sub _generic_do
{
    my ( $self, $pass_idx, $EXE, $opt, $from, $to ) = @_;
    my $prog = "@{[$self->libdir]}/exec/$EXE";
    my $args = "$opt -o $to $from";
    return scalar(
          $self->opt_s
        ? $self->dosystem("$prog $args")
        : $self->pass($pass_idx)->dosource( $prog, $args )
    );
}

sub pass1
{
    my ( $_pass_mgr, $opt, $from, $to, $tmp ) = @_;
    return $_pass_mgr->_generic_do( 1, 'wml_p1_ipp', $opt, $from, $to );
}

sub pass2
{
    my ( $_pass_mgr, $opt, $from, $to, $tmp ) = @_;
    my $cwd = _my_cwd;
    my $rc  = $_pass_mgr->dosystem(
        "@{[$_pass_mgr->libdir]}/exec/wml_p2_mp4h $opt -I '$cwd' $from >$tmp");

    #   remove asterisks which can be entered
    #   by the user to avoid tag interpolation
    my $buf = io()->file($tmp)->all;
    $buf =~ s|<\*?([a-zA-Z][a-zA-Z0-9-_]*)\*?([^a-zA-Z0-9-_])|<$1$2|sg;
    $buf =~ s|<\*?(/[a-zA-Z][a-zA-Z0-9-_]*)\*?>|<$1>|sg;
    io->file($to)->print($buf);

    return $rc;
}

sub pass3
{
    my ( $_pass_mgr, $opt, $from, $to, $tmp ) = @_;

    return
        scalar $_pass_mgr->dosystem(
"@{[$_pass_mgr->libdir]}/exec/wml_p3_eperl $opt -P -k -B '<:' -E ':>' $from >$to"
        );
}

sub pass4
{
    my ( $_pass_mgr, $opt, $from, $to, $tmp ) = @_;

    return scalar $_pass_mgr->dosystem("m4 $opt --prefix-builtins <$from >$to");
}

sub pass5
{
    my ( $_pass_mgr, $opt, $from, $to, $tmp ) = @_;
    return $_pass_mgr->_generic_do( 5, 'wml_p5_divert', $opt, $from, $to );
}

sub pass6
{
    my ( $_pass_mgr, $opt, $from, $to, $tmp ) = @_;
    return $_pass_mgr->_generic_do( 6, 'wml_p6_asubst', $opt, $from, $to );
}

sub pass7
{
    my ( $_pass_mgr, $opt, $from, $to, $tmp ) = @_;
    return $_pass_mgr->_generic_do( 7, 'wml_p7_htmlfix', $opt, $from, $to );
}

sub pass8
{
    my ( $_pass_mgr, $opt, $from, $to, $tmp ) = @_;
    return $_pass_mgr->_generic_do( 8, 'wml_p8_htmlstrip', $opt, $from, $to );
}

sub pass9
{
    my ( $_pass_mgr, $opt, $from, $to, $tmp ) = @_;

    #   First check whether a shebang line is found and no
    #   output files were assigned on command line.
    #   This is needed to unprotect output files.
    if ( !@{ $_pass_mgr->opt_o } )
    {
        local @ARGV = @{ $_pass_mgr->_read_slices($from) };
        if (@ARGV)
        {
            $_pass_mgr->out_istmp(0);
            $_pass_mgr->_process_argv_cb->($_pass_mgr);
            $opt = $_pass_mgr->pass(9)->opt_pass;
        }
    }

    #   slice contains "package" commands and
    #   other stuff, so we cannot source it.
    return
        scalar $_pass_mgr->dosystem(
        "@{[$_pass_mgr->libdir]}/exec/wml_p9_slice $opt $from");
}

sub _read_slices
{
    my ( $self, $from ) = @_;

    my @ret;
    open( my $slice_fh, '<', $from )
        or error("Unable to load $from: $!");
    while ( my $l = <$slice_fh> )
    {
        if ( $l =~ m|%!slice\s+(.*)$| )
        {
            push( @ret, split_argv($1) );
        }
    }
    close($slice_fh)
        or error("Unable to close $from: $!");
    return \@ret;
}

sub _display_times
{
    my ($_pass_mgr) = @_;

    my ( $u, $s, $cu, $cs ) = times();
    my $at      = $u + $s + $cu + $cs;
    my $pt      = 0;
    my $timestr = '';

    foreach my $i ( 1 .. 9 )
    {
        my $t = $_pass_mgr->pass($i)->time_() // '';
        if ( length($t) )
        {
            $pt += $t;
        }
        my $cond = ( $i == 2 or $i == 3 );
        $timestr .= (
              ( $t ne '' ) ? sprintf( $cond ? '%5.2f' : '%4.2f', $t )
            : $cond        ? '   -- '
            :                '  -- '
        );
    }

    $timestr = sprintf( '%4.2f | ', $at - $pt ) . $timestr;
    $timestr .= sprintf( '| %6.2f', $at );
    verbose( 1, "Processing time (seconds):\n" );
    verbose( 1,
        "main |  ipp  mp4h   epl  gm4  div asub hfix hstr slic |  TOTAL\n" );
    verbose( 1,
        "---- | ---- ----- ----- ---- ---- ---- ---- ---- ---- | ------\n" );
    verbose( 1, "$timestr\n" );
}

1;
