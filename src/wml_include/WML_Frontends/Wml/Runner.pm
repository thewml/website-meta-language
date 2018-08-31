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

package WML_Frontends::Wml::Runner;

use strict;
use warnings;

use parent 'WML_Frontends::Wml::Base';

use Class::XSAccessor (
    accessors => +{
        map { $_ => $_ }
            qw(
            _PROTECT_COUNTER
            _argv
            _firstpass
            _opt_D
            _opt_E
            _opt_I
            _opt_M
            _opt_O
            _opt_P
            _opt_V
            _opt_W
            _opt_c
            _opt_h
            _opt_i
            _opt_n
            _opt_o
            _opt_p
            _opt_q
            _opt_r
            _opt_t
            _out
            _out_filenames
            _pass_mgr
            _passes_idxs
            _protect_storage
            _src
            _src_filename
            _src_istmp
            _tmp
            _tmpdir
            bindir
            )
    }
);

use Getopt::Long 2.13;
use File::Spec ();
use Cwd        ();
use List::Util qw/ max /;
use File::Basename qw/ basename dirname /;

use IO::All qw/ io /;
use Term::ReadKey qw/ ReadMode ReadKey /;

use WmlConfig qw//;
use WML_Frontends::Wml::PassesManager ();
use WML_Frontends::Wml::Util
    qw/ _my_cwd canonize_path ctime error expandrange gmt_ctime gmt_isotime
    isotime quotearg split_argv usage /;

sub new
{
    my $self = bless +{}, shift;

    $self->_pass_mgr(
        WML_Frontends::Wml::PassesManager->new(
            {
                libdir => WmlConfig::libdir(),
            }
        )
    );

    $self->_tmpdir( $ENV{TMPDIR} || '/tmp' );

    # Clear out any existing CGI environments because some of our passes
    # (currently Pass 2 and 3) get totally confused by these variables.
    delete @ENV{
        qw(
            SERVER_SOFTWARE SERVER_NAME GATEWAY_INTERFACE SERVER_PROTOCOL
            SERVER_PORT REQUEST_METHOD PATH_INFO PATH_TRANSLATED SCRIPT_NAME
            QUERY_STRING REMOTE_HOST REMOTE_ADDR AUTH_TYPE REMOTE_USER REMOTE_IDENT
            CONTENT_TYPE CONTENT_LENGTH HTTP_ACCEPT HTTP_USER_AGENT
            )
    };

    my $bindir = $self->bindir( WmlConfig::bindir() );
    if ( index( $ENV{PATH}, $bindir ) < 0 )
    {
        $ENV{PATH} = "$bindir:$ENV{PATH}";
    }

    return $self;
}

sub _calc_epilogue_program
{
    my ( $self, $e ) = @_;

    my $_pass_mgr = $self->_pass_mgr;
    my $libdir    = $_pass_mgr->libdir;

    if ( $e =~ m|^htmlinfo(.*)| )
    {
        return "$libdir/exec/wml_aux_htmlinfo$1";
    }
    elsif ( $e =~ m|^linklint(.*)| )
    {
        $e = "$libdir/exec/wml_aux_linklint$1";
        $e .= " -nocache -one -summary" if ( $1 eq '' );
    }
    elsif ( $e =~ m|^weblint(.*)| )
    {
        return "$libdir/exec/wml_aux_weblint$1";
    }
    elsif ( $e =~ m|^tidy(.*)| )
    {
        $e = "tidy$1";
        $e .= ' -m' if ( $1 eq '' );
    }
    return $e;
}

sub _handle_output
{
    my ( $self, ) = @_;
    my $_pass_mgr = $self->_pass_mgr;
    my $libdir    = $_pass_mgr->libdir;

    #   Unprotect output files and run epilog filters
    if ( !@{ $self->_out_filenames } )
    {
        return;
    }

    #   unprotect all outputfiles
    foreach my $o ( @{ $self->_out_filenames } )
    {
        $self->_unprotect( $o, 9 );
    }

    #   optionally set mtime of outputfiles
    #   to mtime of inputfile if inputfile was not STDIN
    if ( not $self->_src_istmp and $self->_opt_t )
    {
        my (
            $dev,  $ino,   $mode,  $nlink, $uid,     $gid, $rdev,
            $size, $atime, $mtime, $ctime, $blksize, $blocks
        ) = stat( $self->_src );
        $atime = time();
        foreach my $o ( @{ $self->_out_filenames } )
        {
            utime( $atime, $mtime + 1, $o );
        }
    }

    #   run epilog filters
    foreach my $o ( @{ $self->_out_filenames } )
    {
        foreach my $e ( @{ $self->_opt_E } )
        {
            my $e_prog = $self->_calc_epilogue_program($e);
            $_pass_mgr->verbose( 2, "EPILOG: $e_prog $o\n" );
            my $rc = system("$e_prog $o");

            #   Tidy returns 1 on warnings and 2 on errors :(
            $rc = 0
                if ( $rc == 256
                and index( $e_prog, "$libdir/exec/wml_aux_tidy" ) >= 0 );
            error("epilog failed: $e_prog $o") if $rc != 0;
        }
    }
    return;
}

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

sub _protect
{
    my ( $self, $fn, $pass ) = @_;

    my $data = io->file($fn)->all;
    open my $fp, '>', $fn
        or error("Unable to write into $fn for protection: $!");

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
            $passes_str .= '9' if $passes_str =~ m|-$|;
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
        $fp->print($1)
            || error("Unable to write into $fn for protection: $!");
        if ( $self->_protect_storage->{$key}->{SPEC} =~ m/$pass/ )
        {
            $fp->print("-=P[$key]=-")
                || error("Unable to write into $fn for protection: $!");
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
    $fp->print($data)
        || error("Unable to write into $fn for protection: $!");
    $fp->close() || error("Unable to close ${fn}: $!");
}

sub _calc_out_fn_helper
{
    my ( $self, $o ) = @_;

    return (
          ( $o =~ m#\A(?:[_A-Z0-9~!+u%n\-\\^x*{}()@]+):(.+)\@(?:.+)\z# ) ? $1
        : ( $o =~ m#\A(?:[_A-Z0-9~!+u%n\-\\^x*{}()@]+):(.+)\z# )         ? $1
        : ( $o =~ m#\A(.+)\@.+\z# )                                      ? $1
        :                                                                  $o
    );
}

sub _calc_out_fn
{
    my ( $self, $o ) = @_;

    my $ret = $self->_calc_out_fn_helper($o);

    return ( $ret ne '-' ? $ret : $self->_tmp->[3] );
}

sub _ProcessOutfiles
{
    my ($self) = @_;
    my $_pass_mgr = $self->_pass_mgr;
    $self->_out('');
    $self->_out_filenames( [] );
    foreach my $o ( @{ $self->_opt_o } )
    {
        next if ( $o =~ m|\*[^:]*$| );

        my $append = sub {
            $self->_out( $self->_out . shift );
        };

        #   create option
        if ( $o eq '-' )
        {
            $append->( " -o '" . quotearg( $self->_tmp->[3] ) . "'" );
            $_pass_mgr->out_istmp(1);
        }
        elsif ( $o =~ /(.*):-\z/ )
        {
            $append->(
                " -o '" . quotearg( $1 . ':' . $self->_tmp->[3] ) . "'" );
            $_pass_mgr->out_istmp(1);
        }
        else
        {
            $append->( " -o '" . quotearg($o) . "'" );
        }

        #   unquote the filename
        $o =~ s|^(['"])(.*)\1$|$2|;

        #   create output file list for epilog filters
        push @{ $self->_out_filenames }, $self->_calc_out_fn($o);
    }
    return;
}

sub _handle_opt_M_stdin
{
    my ( $self, ) = @_;

    my $_pass_mgr = $self->_pass_mgr;

    if ( not @{ $self->_out_filenames } )
    {
        return;
    }
    my $o = '"' . join( ' ', @{ $self->_out_filenames } ) . '"';
    my $opt_pass = '';
    foreach my $aa ( @{ $self->_opt_W } )
    {
        if ( $aa =~ m|^([0-9]),(.*)$| )
        {
            $opt_pass .= " $2 " if $1 == 1;
        }
    }
    my $rc;
    eval {
        $rc = $_pass_mgr->pass1( $_pass_mgr->pass(1)->opt_pass() . $opt_pass,
            $self->_src, $o, $self->_tmp->[2] );
    };
    if ( $rc != 0 )
    {
        if ( $rc % 256 != 0 )
        {
            printf( STDERR
                    "** WML:Break: Error in Pass %d (status=%d, rc=%d).\n",
                1, $rc % 256, $rc / 256 );
        }
        else
        {
            printf( STDERR "** WML:Break: Error in Pass %d (rc=%d).\n",
                1, $rc / 256 );
        }
        $self->_unlink_tmp;
        die;
    }
}

sub _populate_opt_D
{
    my ($self) = @_;

    my $_pass_mgr = $self->_pass_mgr;
    my $libdir    = $_pass_mgr->libdir;

    my @pwinfo       = getpwuid($<);
    my $gen_username = $pwinfo[0];
    $gen_username =~ s|[\'\$\`\"]||g;    # make safe for shell interpolation
    $gen_username ||= 'UNKNOWN-USERNAME';

    my $gen_realname = $pwinfo[6];
    $gen_realname =~ s|^([^\,]+)\,.*$|$1|;
    $gen_realname =~ s|[\'\$\`\"]||g;    # make safe for shell interpolation
    $gen_realname ||= 'UNKNOWN-REALNAME';

    my $gen_time        = time();
    my $gen_ctime       = ctime($gen_time);
    my $gen_isotime     = isotime($gen_time);
    my $gen_gmt_ctime   = gmt_ctime($gen_time);
    my $gen_gmt_isotime = gmt_isotime($gen_time);

    my (
        $src_dirname,     $src_basename, $src_time,
        $src_ctime,       $src_isotime,  $src_gmt_ctime,
        $src_gmt_isotime, $src_username, $src_realname,
    );
    my $cwd = _my_cwd;

    if ( $self->_src_istmp )
    {
        $src_dirname = $cwd;
        $self->_src_filename('STDIN');
        $src_basename    = 'STDIN';
        $src_time        = $gen_time;
        $src_ctime       = $gen_ctime;
        $src_isotime     = $gen_isotime;
        $src_gmt_ctime   = $gen_gmt_ctime;
        $src_gmt_isotime = $gen_gmt_isotime;
        $src_username    = $gen_username;
        $src_realname    = $gen_realname;
    }
    else
    {
        $src_dirname = (
            ( $self->_src =~ m#/# )
            ? Cwd::abs_path( dirname( $self->_src ) )
            : $cwd
        );
        $src_basename = $self->_src_filename( basename( $self->_src ) );
        $src_basename =~ s#(\.[a-zA-Z0-9]+)\z##;
        my (
            $dev,  $ino,   $mode,  $nlink, $uid,     $gid, $rdev,
            $size, $atime, $mtime, $ctime, $blksize, $blocks
        ) = stat( $self->_src );
        $src_time        = $mtime;
        $src_ctime       = ctime($mtime);
        $src_isotime     = isotime($mtime);
        $src_gmt_ctime   = gmt_ctime($mtime);
        $src_gmt_isotime = gmt_isotime($mtime);
        my @pwinfo = getpwuid($uid);
        $src_username = $pwinfo[0] || 'UNKNOWN-USERNAME';
        $src_username =~ s|[\'\$\`\"]||g;    # make safe for shell interpolation
        $src_realname = $pwinfo[6] || 'UNKNOWN-REALNAME';
        $src_realname =~ s|^([^\,]+)\,.*$|$1|;
        $src_realname =~ s|[\'\$\`\"]||g;    # make safe for shell interpolation
    }

    unshift(
        @{ $self->_opt_D },
        "WML_SRC_DIRNAME=$src_dirname",
        "WML_SRC_FILENAME=" . $self->_src_filename,
        "WML_SRC_BASENAME=$src_basename",
        "WML_SRC_TIME=$src_time",
        "WML_SRC_CTIME=$src_ctime",
        "WML_SRC_ISOTIME=$src_isotime",
        "WML_SRC_GMT_CTIME=$src_gmt_ctime",
        "WML_SRC_GMT_ISOTIME=$src_gmt_isotime",
        "WML_SRC_USERNAME=$src_username",
        "WML_SRC_REALNAME=$src_realname",
        "WML_GEN_TIME=$gen_time",
        "WML_GEN_CTIME=$gen_ctime",
        "WML_GEN_ISOTIME=$gen_isotime",
        "WML_GEN_GMT_CTIME=$gen_gmt_ctime",
        "WML_GEN_GMT_ISOTIME=$gen_gmt_isotime",
        "WML_GEN_USERNAME=$gen_username",
        "WML_GEN_REALNAME=$gen_realname",
        "WML_GEN_HOSTNAME=@{[$_pass_mgr->gen_hostname]}",
        'WML_LOC_PREFIX=' . WmlConfig::prefix(),
        "WML_LOC_BINDIR=" . $self->bindir,
        "WML_LOC_LIBDIR=$libdir",
        'WML_LOC_MANDIR=' . WmlConfig::mandir(),
        "WML_VERSION=@{[$self->_VERSION]}",
        "WML_TMPDIR=" . $self->_tmpdir
    );

    return;
}

sub _calc_passes_idxs
{
    my ( $self, ) = @_;

    #   canonicalize -p option(s)
    if ( !@{ $self->_opt_p } )
    {
        #   no option means all passes
        $self->_opt_p( [ ('1-9') ] );
    }
    if ( not -s $self->_src )
    {
        #   on empty input optimize to just use pass 9
        $self->_opt_p( [ ('9') ] );
    }
    my $pass_str = join( '', @{ $self->_opt_p } );
    $pass_str =~ s|,||g;
    $pass_str =~ s|([0-9])-([0-9])|expandrange($1, $2)|sge;
    my $_SORT = ( $pass_str =~ s/!$// );
    my @_passes_idxs = split( '', $pass_str );
    if ( !$_SORT )
    {
        @_passes_idxs = sort { $a <=> $b } @_passes_idxs;
    }

    #   only pre-processing if -M option specified
    @_passes_idxs = ('1') if $self->_opt_M ne '-';

    $self->_passes_idxs( \@_passes_idxs );

    return;
}

# MAIN PROCESSING LOOP
sub _passes_loop
{
    my ( $self,, ) = @_;

    my $final     = 0;
    my $last      = '';
    my $_pass_mgr = $self->_pass_mgr;
    my @prop      = ( "-", "\\", "|", "/" );
    my ( $from, $to, $cnt ) = (
          ( not $self->_src_istmp )
        ? ( $self->_tmp->[0], $self->_tmp->[1], 1, )
        : ( $self->_src, $self->_tmp->[0], 0, )
    );

    my $pcnt = 0;
PASS_IDX: foreach my $pass_idx ( @{ $self->_passes_idxs } )
    {
        $_pass_mgr->verbose( 2, "PASS $pass_idx:\n" );
        print STDERR $prop[ $pcnt++ % 4 ] . "\b" if ( not $self->_opt_q );

        #   run pass
        my ( $u, $s, $cu, $cs ) = times();
        my $stime = $u + $s + $cu + $cs;
        $self->_protect( $from, $pass_idx );
        my $opt_pass = '';
        foreach my $aa ( @{ $self->_opt_W } )
        {
            if ( my ( $pp, $s ) = $aa =~ m|\A([0-9]),(.*)\z| )
            {
                $opt_pass .= " $s " if $pp == $pass_idx;
            }
        }
        my $_pass = $_pass_mgr->pass($pass_idx);
        my $rc    = $_pass->cb()->(
            $_pass_mgr, $_pass->opt_pass() . $opt_pass,
            $from, $to, $self->_tmp->[2]
        );
        if ( !length($rc) )
        {
            $rc = 0;
        }
        if ( $rc != 0 )
        {
            if ( $rc % 256 != 0 )
            {
                printf( STDERR
                        "** WML:Break: Error in Pass %d (status=%d, rc=%d).\n",
                    $pass_idx, $rc % 256, $rc / 256 );
            }
            else
            {
                printf( STDERR "** WML:Break: Error in Pass %d (rc=%d).\n",
                    $pass_idx, $rc / 256 );
            }
            $self->_unlink_tmp;
            die;
        }

        # pass 9 is a special case
        $self->_unprotect( $to, $pass_idx ) if ( $pass_idx < 9 );
        ( $u, $s, $cu, $cs ) = times();
        my $etime = $u + $s + $cu + $cs;
        my $dtime = $etime - $stime;
        $dtime = 0.01 if ( $dtime < 0 );
        $_pass->time_($dtime);

        #   optionally view current result
        if (0)
        {
            if ( $_pass_mgr->_opt_v() >= 3 && $pass_idx < 9 )
            {
                print STDERR "Want to see result after Pass$pass_idx [yNq]: ";
                ReadMode 4;
                my $key = ReadKey(0);
                ReadMode 0;
                print STDERR "\n";
                if ( $key =~ m|[Yy]| )
                {
                    my $pager = ( $ENV{PAGER} || 'more' );
                    system("$pager $to");
                }
                elsif ( $key =~ m|[qQ]| )
                {
                    printf( STDERR "** WML:Break: Manual Stop.\n" );
                    $self->_unlink_tmp;
                    die;
                }
            }
        }

        #   step further
        $last = $to;
        $final = 1 if $pass_idx == 9;
        my $bit = ( $cnt & 1 );
        $from = $self->_tmp->[$bit];
        $to   = $self->_tmp->[ $bit ^ 1 ];
        unlink($to);
        ++$cnt;

        if ($final)
        {
            last PASS_IDX;
        }
    }
    return ( $final, $last );
}

sub _unlink_tmp
{
    my ($self) = @_;
    unlink( @{ $self->_tmp }[ 0 .. 3 ] );
    unlink( $self->_src ) if ( $self->_src_istmp );
    return;
}

sub _do_output
{
    my ( $self, $final, $last ) = @_;

    my $_pass_mgr = $self->_pass_mgr;

    if ( $last ne '' and $final and $_pass_mgr->out_istmp )
    {
        $self->_unprotect( $self->_tmp->[3], 9 );
    }
    elsif ( $last ne '' and not $final )
    {
        my @fh = ();
        $self->_unprotect( $last, 9 );
        if ( @{ $self->_out_filenames } )
        {
            foreach my $o ( @{ $self->_out_filenames } )
            {
                open my $fp, '>', $o or error("Unable to write into $o");
                push @fh, $fp;
            }
        }
        else
        {
            my $o = $self->_tmp->[3];
            open $fh[0], '>', $o or error("Unable to write into $o");
        }
        my $buf = io()->file($last)->all;
        foreach my $fp (@fh)
        {
            $fp->print($buf)
                or error("Unable to write into output file: $!");
            $fp->close() or error("Unable to close output file: $!");
        }
    }

    return;
}

sub _map_opt_o
{
    my ( $self, $opts ) = @_;
    my ( $dir, $base );

    if ( $self->_src =~ m#\A(.+)/([^/]+)\z# )
    {
        ( $dir, $base ) = ( $1, $2 );
    }
    else
    {
        ( $dir, $base ) = ( '.', $self->_src );
    }
    $base =~ s#\.[a-zA-Z0-9]+\z##;
    $opts =~ s|%DIR|$dir|sg;
    $opts =~ s|%BASE|$base|sg;
    return $opts;
}

sub _print_version
{
    my ( $self, ) = @_;

    #   fix the version level
    if ( $self->_opt_V == 0 )
    {
        # Getopt::Long sets 0 if -V only
        $self->_opt_V(1);
    }
    if ( $self->_opt_V == -1 )
    {
        # we operate with 0 for not set
        $self->_opt_V(0);
    }
    if ( $self->_opt_V )
    {
        print STDERR <<"EOF";
This is WML Version @{[$self->_VERSION]}
Copyright (c) 1996-2001 Ralf S. Engelschall.
Copyright (c) 1999-2001 Denis Barbier.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
EOF
        if ( $self->_opt_V > 1 )
        {
            print STDERR WmlConfig::build_info();
        }
        if ( $self->_opt_V > 2 )
        {
            print STDERR "\nUsed Perl System:\n",
                `@{[WmlConfig::perlprog()]} -V`;
        }
        exit(0);
    }

    return;
}

sub _process_ENV_WMLOPTS
{
    my ($self) = @_;

    my $_pass_mgr = $self->_pass_mgr;
    my $opts      = $ENV{WMLOPTS};
    return if not $opts;
    $_pass_mgr->verbose( 2, "Reading WMLOPTS variable" );
    $opts =~ s|^\s+||;
    $opts =~ s|\s+$||;

    #   arguments are not quoted because shell metacharacters
    #   have already been expanded, but dollar sign must be
    #   escaped
    $opts =~ s|\$|\\\$|g;
    $self->_process_options( [ split_argv($opts) ], $self->_opt_D );
    return;
}

sub _process_wmlrc
{
    my ($self) = @_;

    my $_pass_mgr = $self->_pass_mgr;

    if ( $self->_opt_r )
    {
        return;
    }
    my $savedir = '';
    my @DIR     = ();

    #   First save current directory and go to input file directory
    if ( not $self->_opt_c and $self->_src =~ m|/| )
    {
        $self->_src( dirname( $self->_src ) );
        if ( -d $self->_src )
        {
            $savedir = Cwd::cwd;
            chdir( $self->_src );
        }
    }
    $self->_src('') if not $savedir;

    #   2. add all parent dirs .wmlrc files for options
    my $cwd = _my_cwd;
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
        if ( -f "$dir/.wmlrc" )
        {
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
        }
    }
    return;
}

sub _calc_default_opts
{
    my ( $self, ) = @_;

    my $defipp   = '';
    my $defgm4   = '';
    my $defmp4h  = '';
    my $defeperl = '';
    while ( my ( $cnt, $o ) = each @{ $self->_opt_o } )
    {
        $defmp4h .= " -D SLICE_OUTPUT_FILENAME[$cnt]=\"$o\"" if $o =~ m|\*|;
    }
    foreach my $d ( @{ $self->_opt_D } )
    {
        if ( my ( $var, undef, $val ) = ( $d =~ m#\A(.+?)=("?)(.*)\2\n*\z# ) )
        {
            $defgm4   .= " \"-Dm4_$var=$val\"";
            $defmp4h  .= " -D $var=\"$val\"";
            $defeperl .= " \"-d$var=$val\"";
            $defipp   .= " \"-D$var=$val\"";
        }
    }
    $defipp .= " -M" . $self->_opt_M if $self->_opt_M ne '-';
    $defipp .= " -DIPP_SRC_REALNAME=" . $self->_src_filename
        if not $self->_src_istmp;

    return ( $defipp, $defmp4h, $defeperl, $defgm4 );
}

sub _process_shebang
{
    my ($self) = @_;
    if ( $self->_opt_n )
    {
        return;
    }
    open( my $tmp_fh, '<', $self->_src )
        or error("Unable to load @{[$self->_src]}: $!");
    my $shebang = '';
SHEBANG:
    while ( my $l = <$tmp_fh> )
    {
        $shebang .= $l;
        if ( $shebang !~ s/\\\s*\z//s )
        {
            last SHEBANG;
        }
    }
    close($tmp_fh)
        or error("Unable to close @{[$self->_src]}:: $!");
    if ( $shebang =~ m|^#!wml\s+(.+\S)\s*$|is )
    {
        #   split opts into arguments and process them
        $self->_process_options( [ split_argv($1) ], $self->_opt_D );
    }
}

sub _calc_passes_options
{
    my ( $self,, ) = @_;
    my $_pass_mgr  = $self->_pass_mgr;
    my $libdir     = $_pass_mgr->libdir;
    my ( $defipp, $defmp4h, $defeperl, $defgm4 ) = $self->_calc_default_opts();

    #   determine preloads
    my $preload = join '', map {
        ( -f "$libdir/include/sys/bootp$_.wml" and /\A[34]\z/ )
            ? " -s 'sys/bootp$_.wml'"
            : ''
    } @{ $self->_passes_idxs };
    $preload .= join '',
        map { m|^<(.+)>$| ? " -s '$1'" : " -i '$_'" } @{ $self->_opt_i };
    my $verbose = ( ( $_pass_mgr->_opt_v() >= 3 ) ? '-v' : '' );

    my $arr = [
        (
                  "$defipp $verbose -S $libdir/include -n @{[$self->_src]} "
                . ( join '', map { " -I $_" } @{ $self->_opt_I } )
                . " $preload "
                . (
                join '', map { ' -P "' . quotearg($_) . '"' } @{ $self->_opt_P }
                )
        ),
        $defmp4h,
        $defeperl,
        $defgm4, $verbose, $verbose, $verbose,
        (
            "$verbose "
                . ( ( $self->_opt_O ne '' ) ? "-O" . $self->_opt_O : '' )
        ),
        "$verbose " . $self->_out,
    ];
    while ( my ( $i, $str ) = each @$arr )
    {
        $_pass_mgr->pass( $i + 1 )->opt_pass($str);
    }
}

sub _calc_reldir
{
    my ($self) = @_;

    if ( $self->_src_istmp )
    {
        return '.';
    }
    my $reldir = $self->_src;
    $reldir =~ s,(:?/|^)[^/]+$,,;
    my $cwd = _my_cwd;
    $reldir = File::Spec->abs2rel( $cwd, "$cwd/$reldir" );
    $reldir = "." if $reldir eq '';
    return $reldir;
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

    my $reldir = $self->_calc_reldir;
    foreach my $d ( map { quotearg $_} @$dnew )
    {
        if ( my ( $var, $path ) = $d =~ m|^([A-Za-z0-9_]+)~(.+)$| )
        {
            if ( $path !~ m|^/| )
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
    return;
}

sub _set_src
{
    my ( $self, ) = @_;

    # set the input file
    $self->_src( $self->_argv->[0] );

    # if no inputfile is given, WML reads from stdin and forces quiet mode
    if ( $self->_src eq '' )
    {
        $self->_src('-');
        $self->_opt_q(1);
    }

    # if input is stdin we create a temporary file
    $self->_src_istmp(0);
    if ( $self->_src eq '-' )
    {
        $self->_src_istmp(1);
        $self->_src( $self->_tmpdir . "/wml.input.$$.tmp" );
        unlink( $self->_src );
        io->stdin() > io->file( $self->_src );
    }

    if ( $self->_src_istmp and not -f $self->_src )
    {
        print STDERR "** WML:Error: input file `@{[$self->_src]}' not found\n";
        die;
    }
    return;
}

sub _output_and_cleanup
{
    my ( $self,, ) = @_;
    my $_pass_mgr = $self->_pass_mgr;

    $self->_do_output( $self->_passes_loop() );

    $self->_handle_output();

    #   ... and eventually send to stdout
    if ( $_pass_mgr->out_istmp )
    {
        io->file( $self->_tmp->[3] ) > io('-');
    }

    $self->_unlink_tmp;

    if ( $_pass_mgr->_opt_v() >= 1 )
    {
        $_pass_mgr->_display_times;
    }

    return 0;
}

sub _VERSION
{
    return WmlConfig::_VERSION;
}

sub _handle_out_tmp
{
    my ($self) = @_;

    if ( not $self->_src_istmp )
    {
        io->file( $self->_src ) > io->file( $self->_tmp->[0] );
    }

    if ( $self->_out eq '' )
    {
        $self->_out( " -o" . $self->_tmp->[3] );
        $self->_pass_mgr->out_istmp(1);
    }
    return;
}

sub _process_options
{
    my ( $self, $my_argv, $_opt_D ) = @_;
    my $_pass_mgr = $self->_pass_mgr;
    local $SIG{__WARN__} = sub {
        print STDERR "WML:Error: $_[0]";
    };
    $Getopt::Long::bundling      = 1;
    $Getopt::Long::getopt_compat = 0;
    my %list_options = (
        "I|include=s@"     => $self->_opt_I,
        "i|includefile=s@" => $self->_opt_i,
        "D|define=s@"      => $_opt_D,
        "P|prolog=s@"      => $self->_opt_P,
        "E|epilog=s@"      => $self->_opt_E,
        "p|pass=s@"        => $self->_opt_p,
        "W|passoption=s@"  => $self->_opt_W,
    );
    my %scalar_options = (
        "o|outputfile=s@" => $self->_opt_o,
        "r|norcfile"      => $self->_gen_opt('_opt_r'),
        "n|noshebang"     => $self->_gen_opt('_opt_n'),
        "c|nocd"          => $self->_gen_opt('_opt_c'),
        "O|optimize=i"    => $self->_gen_opt('_opt_O'),
        "M|depend:s"      => $self->_gen_opt('_opt_M'),
        "q|quiet"         => $self->_gen_opt('_opt_q'),
        "V|version:i"     => $self->_gen_opt('_opt_V'),
        "h|help"          => $self->_gen_opt('_opt_h'),
        "t|settime"       => $self->_gen_opt('_opt_t'),
        "s|safe"          => $_pass_mgr->_gen_opt('_opt_s'),
        "v|verbose:i"     => $_pass_mgr->_gen_opt('_opt_v'),
    );
    if (
        not Getopt::Long::GetOptionsFromArray(
            $my_argv, %scalar_options, %list_options
        )
        )
    {
        warn "Try `$0 --help' for more information.\n";
        exit(0);
    }
    usage($0) if ( $self->_opt_h );
    while ( my ( $opt, $var ) = each(%list_options) )
    {
        if ( @$var and $var->[0] =~ m|^=| )
        {
            my $arg = substr( $opt, 0, 1 );
            warn "An equal sign has been detected after the `-$arg' option\n";
            warn "Try `$0 --help' for more information.\n\n";
            exit(0);
        }
    }
    return ($_opt_D);
}

sub _reset_opts
{
    my $self      = shift;
    my $_pass_mgr = $self->_pass_mgr;

    $_pass_mgr->_opt_s(0);
    $self->_opt_D( [] );
    $self->_opt_M('-');
    $self->_opt_O('');
    $self->_opt_P( [] );
    $self->_opt_V(-1);
    $self->_opt_W( [] );
    $self->_opt_i( [] );
    $self->_opt_n(0);
    $self->_opt_o( [] );
    $self->_opt_p( [] );
    $self->_opt_q(0);
    $self->_opt_t(0);
    return;
}

# A god method: https://en.wikipedia.org/wiki/God_object
sub run_with_ARGV
{
    my ( $self, $args ) = @_;
    my $_pass_mgr = $self->_pass_mgr;
    $self->_argv( [ @{ $args->{ARGV} } ] );
    $_pass_mgr->_opt_v(-1);
    $self->_opt_E( [] );
    $self->_opt_I( [] );
    $self->_opt_h(0);
    $self->_reset_opts;

    my @temp_argv = @{ $self->_argv };
    $self->_process_options( \@temp_argv, [] );
    $self->_src( $temp_argv[0] );

    #   reset with defaults (except $self->_opt_r and $_pass_mgr->_opt_v())
    $self->_reset_opts;
    $self->_process_ENV_WMLOPTS;

    # .wmlrc File Parsing
    $self->_process_wmlrc;

    #   4. process the command line options
    my ($dnew) = $self->_process_options( $self->_argv, [] );
    $self->_print_version();

    #   If the -M was the last option and the user forgot
    #   to put `--' to end options, we adjust it.
    if ( $self->_opt_M !~ m%^(-|[MD]*)$% and ( !@{ $self->_argv } ) )
    {
        push( @{ $self->_argv }, $self->_opt_M );
        $self->_opt_M('');
    }

    $self->_set_src();

    # now adjust -D options from command line relative to path to source file
    $self->_adjust_opt_D($dnew);

    #   5. process the options from the pseudo-shebang line
    $self->_process_shebang;

    #   6. expand %DIR and %BASE in the -o flags
    $self->_opt_o( [ map { $self->_map_opt_o($_) } @{ $self->_opt_o } ] );
    $self->_process_opt_D;
    $_pass_mgr->_fix_verbose_level;
    $self->_PROTECT_COUNTER(0);
    $self->_protect_storage( +{} );
    $_pass_mgr->_opt_o( $self->_opt_o );

    $self->_firstpass(1);

    #   Flag set if some output goes to stdout
    $_pass_mgr->out_istmp(0);

    my $cwd = _my_cwd;
    $self->_populate_opt_D;

    #   Create temporary file names
    $self->_tmp(
        [
            map { sprintf( "%s/wml.%s.tmp%d", $self->_tmpdir, $$, $_ + 1 ) }
                0 .. 3
        ]
    );

    $self->_calc_passes_idxs();

    #   determine prologs
    $self->_out_filenames( [] );
    $self->_ProcessOutfiles;
    $_pass_mgr->_process_argv_cb(
        sub {
            $self->_process_options( $self->_argv, $self->_opt_D );
            $self->_ProcessOutfiles;
            return;
        }
    );

    $self->_handle_out_tmp;
    $self->_calc_passes_options;

    if ( $self->_opt_M ne '-' )
    {
        $self->_handle_opt_M_stdin();
        $self->_unlink_tmp;
        return 0;
    }
    return $self->_output_and_cleanup();
}

1;

__END__

# vim: ft=perl
