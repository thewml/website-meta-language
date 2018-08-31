##
##  IPP -- Include Pre-Processor
##  Copyright (c) 1997,1998,1999 Ralf S. Engelschall, All Rights Reserved.
##  Copyright (c) 2000 Denis Barbier, All Rights Reserved.
##

package WML_Backends::IPP::Main;

use 5.014;

use strict;
use warnings;

use parent 'WML_Frontends::Wml::Base';

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            INCLUDES
            opt_I
            opt_M
            opt_N
            opt_S
            opt_v
            temp_fn
            _map
            _out_buf_ref
            )
    },
);

use Getopt::Long 2.13;
use File::Temp qw/tempdir/;
use File::Spec;

use IO::All qw/ io /;

use Carp qw( cluck );
use WML_Backends ();
use WML_Backends::IPP::Args qw/ $IDENT_RE /;
use WML_Backends::IPP::Delimit ();
use WML_Backends::IPP::File    ();
use WML_Backends::IPP::Map     ();

sub _delim
{
    return WML_Backends::IPP::Delimit->new( delimiter => shift );
}

sub _sq
{
    return _delim(q/'/);
}

# helper functions

sub verbose
{
    my ( $self, $level, $str ) = @_;
    if ( $self->opt_v )
    {
        print STDERR ' ' x ( $level * 2 ) . "$str\n";
    }
}

sub error
{
    my ($str) = @_;
    cluck("** IPP:Error: $str");
    exit(1);
}

sub warning
{
    my ($str) = @_;
    print STDERR "** IPP:Warning: $str\n";
}

# process command line
sub usage
{
    print STDERR <<'EOF';
Usage: ipp [options] file ...

Options:
  -D, --define=<name>=<value>  define a variable
  -S, --sysincludedir=<dir>    add system include directory
  -I, --includedir=<dir>       add user include directory
  -s, --sysincludefile=<file>  pre-include system include file
  -i, --includefile=<file>     pre-include user include file
  -M, --depend=<options>       dump dependencies as gcc does
  -P, --prolog=<path>          specify one or more prolog filters
  -m, --mapfile=<file>         use include file mapping table
  -N, --nosynclines            do not output sync lines
  -n, --inputfile=<file>       set input file name printed by sync lines
  -o, --outputfile=<file>      set output file instead of stdout
  -v, --verbose                verbosity
EOF
    exit(1);
}

sub ProcessFile
{
    my ( $self, $mode, $_del, $fn, $realname, $level, $no_id, $in_arg ) = @_;

    return WML_Backends::IPP::File->new( _main => $self )
        ->ProcessFile( $mode, $_del, $fn, $realname, $level, $no_id, $in_arg );
}

sub _write_includes
{
    my ( $self, $opt_s, $opt_i ) = @_;

    my $tmp = io()->file( $self->temp_fn )->open('>');
    foreach my $fn (@$opt_s)
    {
        if ( $fn =~ m#\A(\S+?)::(\S+).*\n\z# )
        {
            $fn = ( "$2.$1" =~ s|::|/|gr );
        }
        $tmp->print("#include <$fn>\n");
    }
    foreach my $fn (@$opt_i)
    {
        $tmp->print(
            ( $fn =~ m|\A\S+?::\S[^\n]*\z|ms )
            ? "#use $fn\n"
            : "#include \"$fn\"\n"
        );
    }
    return;
}

sub _create_initial_argument_vector
{
    my ( $self, $arg, $opt_D ) = @_;

    foreach my $str (@$opt_D)
    {
        $str =~ s=\A(['"])(.*)\1\z=$2=;
        if ( $str !~ s/\A($IDENT_RE)// )
        {
            error("Bad argument to option `D': $str");
        }
        my $id = $1;
        $arg->{$id} =
              +( $str =~ m|^="(.*)"$| )       ? $1
            : ( $str =~ m|^=(?:['"]['"])?$| ) ? ''
            : ( $str =~ m|^=(.+)$| )          ? $1
            : ( $str eq '' )                  ? 1
            :   error("Bad argument to option `D': $str");
    }
    return;
}

sub _del_temp
{
    my $self = shift;

    return unlink( $self->temp_fn );
}

sub _append
{
    my ( $self, $text ) = @_;

    ${ $self->_out_buf_ref } .= $text;

    return;
}

sub main
{
    my ( $self, ) = @_;
    $self->opt_v(0);
    $self->opt_M('-');
    $self->opt_I( [ () ] );
    $self->opt_S( [ () ] );
    $self->opt_N(0);
    $Getopt::Long::bundling      = 1;
    $Getopt::Long::getopt_compat = 0;

    my @opt_D;
    my @opt_i;
    my @opt_s;
    my @opt_P;
    my @opt_m;
    my $opt_o = '-';
    my $opt_n = '';
    if (
        not Getopt::Long::GetOptions(
            "D|define=s@"         => \@opt_D,
            "I|includedir=s@"     => $self->opt_I,
            "M|depend:s"          => $self->_gen_opt('opt_M'),
            "N|nosynclines"       => $self->_gen_opt('opt_N'),
            "v|verbose"           => $self->_gen_opt('opt_v'),
            "P|prolog=s@"         => \@opt_P,
            "S|sysincludedir=s@"  => $self->opt_S,
            "i|includefile=s@"    => \@opt_i,
            "m|mapfile=s@"        => \@opt_m,
            "n|inputfile=s"       => \$opt_n,
            "o|outputfile=s"      => \$opt_o,
            "s|sysincludefile=s@" => \@opt_s,
        )
        )
    {
        usage();
    }

    #   Adjust the -M flags
    if ( $self->opt_M !~ m%^(-|[MD]*)$% && ( !@ARGV ) )
    {
        push( @ARGV, $self->opt_M );
        $self->opt_M('');
    }
    usage() if ( !@ARGV );
    push( @{ $self->opt_I }, '.' );
    $self->_map( WML_Backends::IPP::Map->new( { filenames => \@opt_m } ) );

    # iterate over the input files
    $self->INCLUDES( { () } );
    $self->_out_buf_ref(
        do { my $s = ''; \$s; }
    );

    my %arg;
    $self->_create_initial_argument_vector( \%arg, \@opt_D );

    #   process the pre-loaded include files
    my $tmpdir = tempdir( 'ipp.XXXXXXXX', 'CLEANUP' => 1, )
        or die "Unable to create temporary directory: $!\n";
    $self->temp_fn( File::Spec->catfile( $tmpdir, "ipp.$$.tmp" ) );

    $self->_del_temp;
    $self->_write_includes( \@opt_s, \@opt_i );
    $self->_append(
        $self->ProcessFile( 'include', _sq(), $self->temp_fn, "", 0, 1, \%arg )
    );
    $self->_del_temp;

    $self->_process_real_files( \@opt_P, $opt_n, \%arg );
    $self->_do_output( $opt_o, );
}

sub _process_real_files
{
    my ( $self, $opt_P, $opt_n, $arg ) = @_;

    foreach my $fn (@ARGV)
    {
        #   create temporary working file
        io()->file( $self->temp_fn )->print( WML_Backends->input( [$fn] ) );

        #   apply prolog filters
        foreach my $p (@$opt_P)
        {
            my $rc = system(
"$p <$self->temp_fn >$self->temp_fn.f && mv $self->temp_fn.f $self->temp_fn 2>/dev/null"
            );
            error("Prolog Filter `$p' failed") if ( $rc != 0 );
        }

        #   process file via IPP filter
        $self->_append(
            $self->ProcessFile(
                'include', _sq(),
                $self->temp_fn, ( $opt_n eq '' ? $fn : $opt_n ),
                0, 1, $arg
            )
        );

        #   cleanup
        $self->_del_temp;
    }

    return;
}

sub _do_output
{
    my ( $self, $opt_o, ) = @_;

    if ( $self->opt_M ne '-' && $opt_o ne '-' )
    {
        my @deps = @ARGV;
        foreach my $inc ( keys( %{ $self->INCLUDES } ) )
        {
            if ( $self->INCLUDES->{$inc} != 1 or $self->opt_M !~ m|M| )
            {
                push( @deps, $inc );
            }
        }

        WML_Backends->out(
            (
                $self->opt_M =~ /D/
                ? ( ( $opt_o =~ s#\..*\z##mrs ) . '.d' )
                : '-'
            ),
            \&error,
            [
                $opt_o . ": \\\n",
                "\t" . join( " \\\n\t", @deps ) . "\n",

            ]
        );
    }
    else
    {
        # create output file
        WML_Backends->out( $opt_o, \&error, [ ${ $self->_out_buf_ref } ] );
    }

    return;
}

1;
