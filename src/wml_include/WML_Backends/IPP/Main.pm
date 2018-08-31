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
    accessors => +{
        map { $_ => $_ }
            qw(
            INCLUDES
            opt_I
            opt_M
            opt_N
            opt_S
            opt_v
            _map
            )
    },
);

sub new
{
    my $class = shift;

    my $self = bless {}, $class;

    $self->_init(@_);

    return $self;
}

sub _init
{
    my ( $self, $args ) = @_;

    return;
}

use Getopt::Long 2.13;
use File::Temp qw/tempdir/;
use File::Spec;

use IO::All qw/ io /;

use Carp qw( cluck );
use WML_Backends ();
use WML_Backends::IPP::Args qw/ $IDENT_RE /;
use WML_Backends::IPP::Delimit ();
use WML_Backends::IPP::Map     ();
use WML_Frontends::Wml::Util qw/ canon_path /;

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

sub PatternProcess
{
    my ( $self, $mode, $_del, $dirname, $pattern, $ext, $level, $no_id, $arg )
        = @_;

    my $out = '';
    my $test =
        +( $ext eq '' )
        ? sub { my $dir = shift; return -f "$dir/$dirname/$_"; }
        : sub { my $dir = shift; return -d "$dir/$dirname"; };

    my $process_dirs = sub {
        my ($dirs) = @_;

    DIRS:
        foreach my $dir ( reverse @$dirs )
        {
            my @ls = grep { /^$pattern$/ && $test->($dir) }
                @{ io->dir("$dir/$dirname") };
            my $found = 0;
        LS:
            foreach (@ls)
            {
                next LS if ( m|/\.+$| or m|^\.+$| );
                $out .=
                    $self->ProcessFile( $mode, $_del, "$dirname/$_$ext", "",
                    $level, $no_id, $arg );
                $found = 1;
            }
            last DIRS if $found;
        }

        return;

    };

    if ( $_del->is_ang )
    {
        $process_dirs->( $self->opt_S );
    }
    if ( $_del->is_quote )
    {
        $process_dirs->( $self->opt_I );
    }
    if ( $_del->is_quote_all )
    {
        if ( -d $dirname )
        {
            my @ls =
                grep { /^$pattern$/ && $test->('.') } @{ io->dir("$dirname") };

            #   Sort list of files
            my $criterion = $arg->{'IPP_SORT'} || $arg->{'IPP_REVERSE'};
            if ( $criterion eq 'date' )
            {
                @ls = sort { -M $a <=> -M $b } @ls;
            }
            elsif ( $criterion eq 'numeric' )
            {
                @ls = sort { $a <=> $b } @ls;
            }
            elsif ($criterion)
            {
                @ls = sort @ls;
            }
            @ls = reverse @ls if ( $arg->{'IPP_REVERSE'} );

            #   and truncate it
            if ( $arg->{'IPP_MAX'} =~ m/^\d+$/ and $arg->{'IPP_MAX'} < @ls )
            {
                splice( @ls, $arg->{'IPP_MAX'} - scalar(@ls) );
            }
            push( @ls, "" );

            $arg->{'IPP_NEXT'} = '';
            $arg->{'IPP_THIS'} = '';
        LS:
            foreach (@ls)
            {
                next LS if ( m|/\.+$| or m|^\.+$| );

                #   set IPP_PREV, IPP_THIS, IPP_NEXT
                $arg->{'IPP_PREV'} = $arg->{'IPP_THIS'};
                $arg->{'IPP_THIS'} = $arg->{'IPP_NEXT'};
                $arg->{'IPP_NEXT'} = ( $_ eq '' ? '' : "$dirname/$_$ext" );
                next LS if $arg->{'IPP_THIS'} eq '';

                $out .=
                    $self->ProcessFile( $mode, $_del, $arg->{'IPP_THIS'}, "",
                    $level, $no_id, $arg );
            }
            delete $arg->{'IPP_NEXT'};
            delete $arg->{'IPP_THIS'};
            delete $arg->{'IPP_PREV'};
        }
    }
    return $out;
}

sub _expand_pattern
{
    my ( $self, $dirname, $pattern, $ext, $mode, $_del, $level, $no_id, $arg )
        = @_;
    if ( $dirname =~ m|^(.*)/(.*?)$| )
    {
        $dirname = $1;
        $pattern = $2 . $pattern;
    }
    else
    {
        $pattern = $dirname . $pattern;
        $dirname = '.';
    }
    if ( $ext =~ m|^(.*?)(/.*)$| )
    {
        $pattern .= $1;
        $ext = $2;
    }
    else
    {
        $pattern .= $ext;
        $ext = '';
    }

    #   replace filename patterns by regular expressions
    $pattern =~ s/\./\\./g;
    $pattern =~ s/\*/.*/g;
    $pattern =~ s/\?/./g;
    return $self->PatternProcess( $mode, $_del, $dirname, $pattern,
        $ext, $level, $no_id, +{%$arg} );
}

sub _process_line
{
    my ( $self, $l, $line_idx, $arg, $store, $level, $out, $fn, $realname ) =
        @_;

    #   EOL-comments
    return if $$l =~ m/^\s*#(?!use|include|depends)/;

    #   Line-Continuation Support
    $$l =~ s|^\s+|| if $$store ne '';
    return if $$l =~ m|^\\\s*\n$|;
    if ( $$l =~ m|^(.*[^\\])\\\s*\n$| )
    {
        $$store .= $1;
        return;
    }
    if ( $$l =~ m|^(.*\\)\\(\s*\n)$| )
    {
        $$l = $1 . $2;
    }
    $$l     = $$store . $$l;
    $$store = '';

    # Variable Interpolation

    #       Substitutions are performed from left to right and from
    #       inner to outer, all operators have same precedence.
    if ( $$l =~ m/((?!\\).|^)\$\(([a-zA-Z0-9_]+)((=|:[-=?+*])([^()]*))?\)/ )
    {
        my ( $name, $op, $str ) = ( $2, $4, $5 );
        if ( not defined($op) )
        {
            #   Normal Value
            $$l =~
s/((?!\\).|^)\$\($name\)/exists $arg->{$name} ? $1.$arg->{$name} : $1/e;
            return 'redo';
        }

        #   Escape special characters
        $op =~ s/([?+*])/\\$1/;
        my $subst = '((?!\\\\).|^)\\$\\(' . $name . $op . '(?:[^()]*)\\)';

        if ( $op eq '=' )
        {
            #   Assign
            $$l =~ s/$subst/$1/;
            if ( $str eq '' )
            {
                delete $arg->{$name} if exists $arg->{$name};
            }
            else
            {
                $arg->{$name} = $str;
            }
        }
        elsif ( $op eq ':\?' )
        {
            #   Indicate Error if Unset
            $$l =~
s/$subst/exists $arg->{$name} ? $1.$arg->{$name} : $1.error($str)/e;
        }
        elsif ( $op eq ':-' )
        {
            #   Use Default Values
            $$l =~ s/$subst/exists $arg->{$name} ? $1.$arg->{$name} : $1.$str/e;
        }
        elsif ( $op eq ':=' )
        {
            #   Use Default Values And Assign
            $$l =~ s/$subst/exists $arg->{$name} ? $1.$arg->{$name} : $1.$str/e;
            if ( $str eq '' )
            {
                delete $arg->{$name} if exists $arg->{$name};
            }
            else
            {
                $arg->{$name} = $str;
            }
        }
        elsif ( $op eq ':\+' )
        {
            #   Use Alternative Value
            $$l =~ s/$subst/exists $arg->{$name} ? $1.$str : $1/e;
        }
        elsif ( $op eq ':\*' )
        {
            #   Use Negative Alternative Value
            $$l =~ s/$subst/exists $arg->{$name} ? $1 : $1.$str/e;
        }
        else
        {
            #   There is an error in these statements
            die "Internal error when expanding variables";
        }
        return 'redo';
    }

    #   EOL-comments again
    return if $$l =~ m/^\s*#(?!use|include|depends)/;

    #   Implicit Variables
    $$l =~ s|__LINE__|$line_idx|g;
    if ( $level == 0 and $arg->{'IPP_SRC_REALNAME'} ne '' )
    {
        $arg->{'IPP_SRC_REALNAME'} = './' . $arg->{'IPP_SRC_REALNAME'}
            if $arg->{'IPP_SRC_REALNAME'} !~ m|/|;
        $$l =~ s|__FILE__|$arg->{'IPP_SRC_REALNAME'}|g;
    }
    else
    {
        $$l =~ s|__FILE__|$fn|g;
    }

    #   remove one preceding backslash
    $$l =~ s/\\(\$\([a-zA-Z0-9_]+(:[-=?+*][^()]*)?\))/$1/g;

    #   ``#include'', ``#use'' and ``#depends'' directives
    if ( my ( $cmd, $incfile, $args ) =
        ( $$l =~ m/^#(use|include|depends)\s+(\S+)(.*)$/ ) )
    {
        #   set arguments
        my %argO = %$arg;
        WML_Backends::IPP::Args->new->setargs( $arg, $args );

        #   do possible argument mapping
        $incfile = $self->_map->mapfile($incfile);

        my $type;

        #   determine raw filename and type
        if ( $incfile =~ m|^(\S+?)::(\S+)$| )
        {
            $type    = '<';
            $incfile = "$2.$1";
            $incfile =~ s|::|/|g;
        }
        elsif ( $incfile =~ m|^(['"<])([^'">]+)['">]$| )
        {
            $type    = $1;
            $incfile = $2;
        }
        else
        {
            error("Unknown file-argument syntax: ``$incfile''");
        }

        #   now recurse down
        $$out .=
            $self->ProcessFile( $cmd, _delim($type),
            $incfile, "", $level + 1, 0, $arg );
        if ( not $self->opt_N and not $arg->{'IPP_NOSYNCLINES'} )
        {
            $$out .=
                  "<__file__ $realname /><__line__ $line_idx />"
                . "<protect pass=2><:# line $line_idx \"$realname\":></protect>\n";
        }

        #   reset arguments
        %$arg = %argO;
    }

    #   ``__END__'' feature
    elsif ( $$l =~ m|^\s*__END__\s*\n?$| )
    {
        return 'last';
    }

    #   plain text
    else
    {
        $$out .= $$l;
    }

    return;
}

sub ProcessFile
{
    my ( $self, $mode, $_del, $fn, $realname, $level, $no_id, $in_arg ) = @_;

    my $arg = +{%$in_arg};

    #   first check whether this is a filename pattern in which case
    #   we must expand it
    if ( my ( $dirname, $pattern, $ext ) =
        ( $fn =~ m/^(.*?)(?=[?*\]])([?*]|\[[^\]]*\])(.*)$/ ) )
    {
        return $self->_expand_pattern( $dirname, $pattern, $ext, $mode,
            $_del, $level, $no_id, $arg );
    }

    #    this is a regular file
    my $found = 0;

    my $process_dirs = sub {
    OPT:
        foreach my $dir ( reverse @{ shift @_ } )
        {
            if ( -f "$dir/$fn" )
            {
                $fn    = "$dir/$fn";
                $found = 1;
                last OPT;
            }
        }
        return;
    };

    if ( $_del->is_ang )
    {
        $process_dirs->( $self->opt_S );
    }
    if ( $_del->is_quote )
    {
        $process_dirs->( $self->opt_I );
    }
    if ( $_del->is_quote_all )
    {
        if ( -f $fn )
        {
            $found = 1;
        }
    }
    error("file not found: $fn") if not $found;

    #   stop if file was still included some time before
    if ( not $no_id )
    {
        my $id = canon_path($fn);
        if ( $mode eq 'use' )
        {
            return '' if ( exists $self->INCLUDES->{$id} );
        }
        $self->INCLUDES->{$id} = $_del->is_ang ? 1 : 2;
    }

    # Stop if just want to check dependency
    return '' if $mode eq 'depends';

    # Process the file
    $realname = $fn if $realname eq '';
    $self->verbose( $level, "|" );
    $self->verbose( $level, "+-- $fn" );
    my $in       = io()->file($fn);
    my $line_idx = 0;
    my $out      = '';
    if ( not $self->opt_N and not $arg->{'IPP_NOSYNCLINES'} )
    {
        $out .=
              "<__file__ $realname /><__line__ 0 />"
            . "<protect pass=2><:# line $line_idx \"$realname\":></protect>\n";
    }
    my $store = '';

LINES:
    while ( my $l = $in->getline )
    {
        ++$line_idx;

        my $op =
            $self->_process_line( \$l, $line_idx, $arg, \$store,
            $level, \$out, $fn, $realname, ) // '';
        if ( $op eq 'last' )
        {
            last LINES;
        }
        elsif ( $op eq 'redo' )
        {
            redo LINES;
        }
    }
    $out .= $store;

    return $out;
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

    # read mapfiles
    $self->_map( WML_Backends::IPP::Map->new );
    $self->_map->read_multi_map_files( \@opt_m );

    # iterate over the input files
    $self->INCLUDES( { () } );
    my $outbuf = '';

    #   create initial argument vector
    my %arg = ();
    foreach my $str (@opt_D)
    {
        $str =~ s=\A(['"])(.*)\1\z=$2=;
        if ( $str =~ m|^($IDENT_RE)="(.*)"$| )
        {
            $arg{$1} = $2;
        }
        elsif ( $str =~ m|^($IDENT_RE)=(['"]['"])?$| )
        {
            $arg{$1} = '';
        }
        elsif ( $str =~ m|^($IDENT_RE)=(.+)$| )
        {
            $arg{$1} = $2;
        }
        elsif ( $str =~ m|^($IDENT_RE)$| )
        {
            $arg{$1} = 1;
        }
        else
        {
            error("Bad argument to option `D': $str");
        }
    }

    #   process the pre-loaded include files
    my $tmpdir = tempdir( 'ipp.XXXXXXXX', 'CLEANUP' => 1, )
        or die "Unable to create temporary directory: $!\n";
    my $tmpfile = File::Spec->catfile( $tmpdir, "ipp.$$.tmp" );

    unlink($tmpfile);
    {
        my $tmp = io()->file($tmpfile)->open('>');
        foreach my $fn (@opt_s)
        {
            if ( $fn =~ m#\A(\S+?)::(\S+).*\n\z# )
            {
                $fn = ( "$2.$1" =~ s|::|/|gr );
            }
            $tmp->print("#include <$fn>\n");
        }
        foreach my $fn (@opt_i)
        {
            $tmp->print(
                ( $fn =~ m|\A\S+?::\S[^\n]*\z|ms )
                ? "#use $fn\n"
                : "#include \"$fn\"\n"
            );
        }
    }
    $outbuf .=
        $self->ProcessFile( 'include', _sq(), $tmpfile, "", 0, 1, \%arg );
    unlink($tmpfile);

    #   process real files
    foreach my $fn (@ARGV)
    {
        #   create temporary working file
        io()->file($tmpfile)->print( WML_Backends->input( [$fn] ) );

        #   apply prolog filters
        foreach my $p (@opt_P)
        {
            my $rc = system(
                "$p <$tmpfile >$tmpfile.f && mv $tmpfile.f $tmpfile 2>/dev/null"
            );
            error("Prolog Filter `$p' failed") if ( $rc != 0 );
        }

        #   process file via IPP filter
        $outbuf .=
            $self->ProcessFile( 'include', _sq(),
            $tmpfile, ( $opt_n eq '' ? $fn : $opt_n ),
            0, 1, \%arg );

        #   cleanup
        unlink($tmpfile);
    }

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
        WML_Backends->out( $opt_o, \&error, [$outbuf] );
    }
}

1;
