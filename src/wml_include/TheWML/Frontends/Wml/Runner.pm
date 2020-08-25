##
##  Copyright (c) 1996-2001 Ralf S. Engelschall.
##  Copyright (c) 1999-2001 Denis Barbier.
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 of the License, or
##  (at your option) any later version.
##

package TheWML::Frontends::Wml::Runner;

use 5.014;

use strict;
use warnings;

use parent 'TheWML::CmdLine::Base';

use Class::XSAccessor (
    accessors => +{
        map { $_ => $_ }
            qw(
            _argv
            _last
            _opt_D_man
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
            _protector
            _src
            _src_filename
            _src_istmp
            _temp_dir
            _tmp
            _tmpdir
            bindir
            final
            pcnt
            )
    }
);

use Getopt::Long 2.13 ();
use File::Spec ();
use List::Util qw/ sum /;
use Path::Tiny qw/ path tempdir /;
use Term::ReadKey qw/ ReadMode ReadKey /;

use TheWML::Config                        ();
use TheWML::Frontends::Wml::OptD          ();
use TheWML::Frontends::Wml::PassesManager ();
use TheWML::Frontends::Wml::Protect       ();
use TheWML::Frontends::Wml::WmlRc         ();
use TheWML::Frontends::Wml::Util
    qw/ _my_cwd error expandrange quotearg split_argv usage /;

sub _opt_D
{
    return shift->_opt_D_man->_opt_D(@_);
}

sub new
{
    my $self = bless +{}, shift;

    $self->_pass_mgr( TheWML::Frontends::Wml::PassesManager->new );

    $self->_tmpdir( $ENV{TMPDIR} || '/tmp' );
    $self->_opt_D_man(
        TheWML::Frontends::Wml::OptD->new( _main => $self, _opt_D => [] ) );

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

    my $bindir = $self->bindir( TheWML::Config::bindir() );
    if ( index( $ENV{PATH}, $bindir ) < 0 )
    {
        $ENV{PATH} = "$bindir:$ENV{PATH}";
    }

    return $self;
}

sub _calc_epilogue_program
{
    my ( $self, $e ) = @_;
    my $libdir = TheWML::Config::libdir();

    if ( $e =~ m|^htmlinfo(.*)| )
    {
        return "$libdir/exec/wml_aux_htmlinfo$1";
    }
    elsif ( $e =~ m|^linklint(.*)| )
    {
        $e = "linklint$1";
        $e .= " -nocache -one -summary" if ( $1 eq '' );
    }
    elsif ( $e =~ m|^weblint(.*)| )
    {
        return "weblint$1";
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
    my $libdir    = TheWML::Config::libdir();

    #   Unprotect output files and run epilog filters
    if ( !@{ $self->_out_filenames } )
    {
        return;
    }

    #   unprotect all outputfiles
    foreach my $o ( @{ $self->_out_filenames } )
    {
        $self->_protector->_unprotect( $o, 9 );
    }

    #   optionally set mtime of outputfiles
    #   to mtime of inputfile if inputfile was not STDIN
    if ( not $self->_src_istmp and $self->_opt_t )
    {
        my $atime = time();
        my $mtime = path( $self->_src )->stat->mtime;
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
    my $o        = '"' . join( ' ', @{ $self->_out_filenames } ) . '"';
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
    if ( defined $rc )
    {
        if ( $rc != 0 )
        {
            $self->_unlink_tmp;
            die +( $rc % 256 != 0 )
                ? sprintf(
                "** WML:Break: Error in Pass %d (status=%d, rc=%d).\n",
                1, $rc % 256, $rc / 256 )
                : sprintf( "** WML:Break: Error in Pass %d (rc=%d).\n",
                1, $rc / 256 );
        }
    }
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
    my $_SORT        = ( $pass_str =~ s/!$// );
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

sub _optionally_view_current_result
{
    my ( $self, $pass_idx, $to ) = @_;
    return;
    my $_pass_mgr = $self->_pass_mgr;
    if ( not( $_pass_mgr->_opt_v() >= 3 && $pass_idx < 9 ) )
    {
        return;
    }
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
        $self->_unlink_tmp;
        die "** WML:Break: Manual Stop.\n";
    }
    return;
}

my @_PROP = ( "-", "\\", "|", "/" );

sub _times_sum
{
    return sum( ( times() )[ 0 .. 3 ] );
}

sub _run_pass
{
    my ( $self, $pass_idx, $cnt, $from, $to ) = @_;
    my $_pass_mgr = $self->_pass_mgr;

    $_pass_mgr->verbose( 2, "PASS $pass_idx:\n" );
    if ( not $self->_opt_q )
    {
        print STDERR $_PROP[ $self->pcnt % 4 ] . "\b";
    }
    $self->pcnt( $self->pcnt + 1 );

    #   run pass
    my $stime = _times_sum;
    $self->_protector->_protect( $$from, $pass_idx );
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
        $$from,     $$to, $self->_tmp->[2]
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
    $self->_protector->_unprotect( $$to, $pass_idx ) if ( $pass_idx < 9 );
    my $dtime = _times_sum() - $stime;
    $dtime = 0.01 if ( $dtime < 0 );
    $_pass->time_($dtime);
    $self->_optionally_view_current_result( $pass_idx, $$to );

    #   step further
    $self->_last($$to);
    $self->final(1) if $pass_idx == 9;
    my $bit = ( $$cnt & 1 );
    $$from = $self->_tmp->[$bit];
    $$to   = $self->_tmp->[ $bit ^ 1 ];
    unlink($$to);
    ++$$cnt;

    return;
}

# MAIN PROCESSING LOOP
sub _passes_loop
{
    my ($self) = @_;

    $self->final(0);
    $self->_last('');
    my ( $from, $to, $cnt ) = (
          ( not $self->_src_istmp )
        ? ( $self->_tmp->[0], $self->_tmp->[1], 1, )
        : ( $self->_src, $self->_tmp->[0], 0, )
    );

    $self->pcnt(0);
PASS_IDX: foreach my $pass_idx ( @{ $self->_passes_idxs } )
    {
        $self->_run_pass( $pass_idx, \$cnt, \$from, \$to );
        if ( $self->final )
        {
            last PASS_IDX;
        }
    }
    return;
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
    my ($self) = @_;

    my $_pass_mgr = $self->_pass_mgr;

    if ( $self->_last ne '' and $self->final and $_pass_mgr->out_istmp )
    {
        $self->_protector->_unprotect( $self->_tmp->[3], 9 );
    }
    elsif ( $self->_last ne '' and not $self->final )
    {
        my @fh = ();
        $self->_protector->_unprotect( $self->_last, 9 );
        if ( @{ $self->_out_filenames } )
        {
            foreach my $o ( @{ $self->_out_filenames } )
            {
                push @fh, path($o)->openw;
            }
        }
        else
        {
            my $o = $self->_tmp->[3];
            push @fh, path($o)->openw;
        }
        my $buf = path( $self->_last )->slurp_raw;
        foreach my $fp (@fh)
        {
            $fp->print($buf);
            $fp->close;
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
This is WML Version @{[TheWML::Config::_VERSION]}
Copyright (c) 1996-2001 Ralf S. Engelschall.
Copyright (c) 1999-2001 Denis Barbier.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
EOF
        if ( $self->_opt_V > 1 )
        {
            print STDERR TheWML::Config::build_info();
        }
        if ( $self->_opt_V > 2 )
        {
            print STDERR "\nUsed Perl System:\n",
                `@{[TheWML::Config::perlprog()]} -V`;
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
    my $tmp_fh  = path( $self->_src )->openr;
    my $shebang = '';
SHEBANG:
    while ( my $l = $tmp_fh->getline )
    {
        $shebang .= $l;
        if ( $shebang !~ s/\\\s*\z//s )
        {
            last SHEBANG;
        }
    }
    $tmp_fh->close;
    if ( $shebang =~ m|^#!wml\s+(.+\S)\s*$|is )
    {
        #   split opts into arguments and process them
        $self->_process_options( [ split_argv($1) ], $self->_opt_D );
    }
    return;
}

sub _calc_passes_options
{
    my ($self)    = @_;
    my $_pass_mgr = $self->_pass_mgr;
    my $libdir    = TheWML::Config::libdir();
    my $datadir   = TheWML::Config::datadir();
    my ( $defipp, $defmp4h, $defeperl, $defgm4 ) = $self->_calc_default_opts();

    #   determine preloads
    my $preload = join '', map {
        ( -f "$datadir/include/sys/bootp$_.wml" and /\A[34]\z/ )
            ? " -s 'sys/bootp$_.wml'"
            : ''
    } @{ $self->_passes_idxs };
    $preload .= join '',
        map { m|^<(.+)>$| ? " -s '$1'" : " -i '$_'" } @{ $self->_opt_i };
    my $verbose = ( ( $_pass_mgr->_opt_v() >= 3 ) ? '-v' : '' );

    my $arr = [
        (
                  "$defipp $verbose -S $datadir/include -n @{[$self->_src]} "
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
    return;
}

sub _calc_reldir
{
    my ($self) = @_;

    if ( $self->_src_istmp )
    {
        return '.';
    }
    my $reldir = $self->_src;
    $reldir =~ s#(:?/|^)[^/]+$##;
    my $cwd = _my_cwd;
    $reldir = File::Spec->abs2rel( $cwd, "$cwd/$reldir" );
    $reldir = "." if $reldir eq '';
    return $reldir;
}

sub _set_src
{
    my ( $self, ) = @_;

    # set the input file
    $self->_src( $self->_argv->[0] // '' );

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
        $self->_src( $self->_temp_dir->child("wml.input.tmp") );
        unlink( $self->_src );
        path( $self->_src )->spew_raw(
            do { local $/; <STDIN> }
        );
    }

    if ( $self->_src_istmp and not -f $self->_src )
    {
        die "** WML:Error: input file `@{[$self->_src]}' not found\n";
    }
    return;
}

sub _output_and_cleanup
{
    my ($self) = @_;
    my $_pass_mgr = $self->_pass_mgr;

    $self->_do_output( $self->_passes_loop() );

    $self->_handle_output();

    #   ... and eventually send to stdout
    if ( $_pass_mgr->out_istmp )
    {
        print path( $self->_tmp->[3] )->slurp_raw;
    }

    $self->_unlink_tmp;

    if ( $_pass_mgr->_opt_v() >= 1 )
    {
        $_pass_mgr->_display_times;
    }
    $self->_temp_dir('');

    return 0;
}

sub _handle_out_tmp
{
    my ($self) = @_;

    if ( not $self->_src_istmp )
    {
        path( $self->_tmp->[0] )->spew_raw( path( $self->_src )->slurp_raw );
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
        "D|define=s@"      => $_opt_D,
        "E|epilog=s@"      => $self->_opt_E,
        "I|include=s@"     => $self->_opt_I,
        "P|prolog=s@"      => $self->_opt_P,
        "W|passoption=s@"  => $self->_opt_W,
        "i|includefile=s@" => $self->_opt_i,
        "p|pass=s@"        => $self->_opt_p,
    );
    if (
        not Getopt::Long::GetOptionsFromArray(
            $my_argv, %list_options,
            "M|depend:s"      => $self->_gen_opt('_opt_M'),
            "O|optimize=i"    => $self->_gen_opt('_opt_O'),
            "V|version:i"     => $self->_gen_opt('_opt_V'),
            "c|nocd"          => $self->_gen_opt('_opt_c'),
            "h|help"          => $self->_gen_opt('_opt_h'),
            "n|noshebang"     => $self->_gen_opt('_opt_n'),
            "o|outputfile=s@" => $self->_opt_o,
            "q|quiet"         => $self->_gen_opt('_opt_q'),
            "r|norcfile"      => $self->_gen_opt('_opt_r'),
            "s|safe"          => $_pass_mgr->_gen_opt('_opt_s'),
            "t|settime"       => $self->_gen_opt('_opt_t'),
            "v|verbose:i"     => $_pass_mgr->_gen_opt('_opt_v'),

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
    my $self = shift;

    $self->_pass_mgr->_opt_s(0);
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
    $self->_temp_dir( tempdir() );

    my @temp_argv = @{ $self->_argv };
    $self->_process_options( \@temp_argv, [] );
    $self->_src( $temp_argv[0] // '-' );

    #   reset with defaults (except $self->_opt_r and $_pass_mgr->_opt_v())
    $self->_reset_opts;
    $self->_process_ENV_WMLOPTS;

    # .wmlrc File Parsing
    TheWML::Frontends::Wml::WmlRc->new( _main => $self )->_process_wmlrc;

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
    $self->_opt_D_man->_adjust_opt_D($dnew);

    #   5. process the options from the pseudo-shebang line
    $self->_process_shebang;

    #   6. expand %DIR and %BASE in the -o flags
    $self->_opt_o( [ map { $self->_map_opt_o($_) } @{ $self->_opt_o } ] );
    $self->_opt_D_man->_process_opt_D;
    $_pass_mgr->_fix_verbose_level;
    $self->_protector(
        TheWML::Frontends::Wml::Protect->new(
            _PROTECT_COUNTER => 0,
            _protect_storage => $self->_protect_storage( +{} ),
        )
    );
    $_pass_mgr->_opt_o( $self->_opt_o );

    $self->_protector->_firstpass(1);

    #   Flag set if some output goes to stdout
    $_pass_mgr->out_istmp(0);

    $self->_opt_D_man->_populate_opt_D;

    #   Create temporary file names
    $self->_tmp(
        [
            map { $self->_temp_dir->child( sprintf( "wml.tmp%d", $_ + 1 ) ) }
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
