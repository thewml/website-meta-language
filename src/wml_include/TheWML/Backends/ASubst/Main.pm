##  asubst -- Area Substitution
##  Copyright (c) 1997,1998,1999 Ralf S. Engelschall, All Rights Reserved.

package TheWML::Backends::ASubst::Main;

use strict;
use warnings;
use 5.014;

use Getopt::Long ();

use parent 'TheWML::CmdLine::Base';

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            argv
            )
    },
);

sub usage
{
    print STDERR "Usage: asubst [options] [file]\n";
    print STDERR "\n";
    print STDERR "Options:\n";
    print STDERR
        "  -o, --outputfile=<file>  set output file instead of stdout\n";
    print STDERR "  -v, --verbose            verbose mode\n";
    die;
}

sub _name
{
    return 'ASubst';
}

sub cnvpre
{
    my ( $str, $level ) = @_;

    return '' if $str eq '';
    return $str;
}

sub cnvin
{
    my ( $str, $level ) = @_;

    return '' if $str eq '';

    my @SCMD;
    $str =~
        s|\[\[(s(.)[^\2]+?\2[^\2]*?\2[igosme]*?)\]\]|push(@SCMD, $1), ''|sge;
    $str =~
        s|\[\[(tr(.)[^\2]+?\2[^\2]+?\2[igosme]*?)\]\]|push(@SCMD, $1), ''|sge;
    foreach my $scmd (@SCMD)
    {
        ## no critic (ProhibitStringyEval)
        eval "\$str =~ $scmd;";
    }
    return $str;
}

sub cnvpost
{
    my ( $str, $level ) = @_;

    return '' if $str eq '';
    return $str;
}

my $_my_debug = 0;

#
#   processing loop
#

#  _expand_block -- expand a delimited and perhaps nested block structure
#
#  ($rc, $buffer) = _expand_block($buffer, \&cnvpre, $startdel, \&cnvin, $enddel, \&cnvpost, $level);
#
sub _expand_block
{
    my $self = shift;
    return $self->_expand_block_more(@_);
}

#   This subvariant is used to split the input into
#   segments which only contain one block, but this
#   itself can be still nested.
#   input: ... < < > > ... < > < < > < > > ...
#   inputs for ExpandBlockOne: "... < < > > ...", "< >", "< < > < > > ...", #   ...
#
sub _expand_block_more
{
    my (
        $self,  $buffer,   $cnvpre,  $opendel,
        $cnvin, $closedel, $cnvpost, $level
    ) = @_;
    my ( $rc, $opened, $offset, @segment, $del, $openidx, $closeidx );
    my ( $bufferN, $s, $e, $data );

    #
    #   first, check for corresponding delimiters
    #   and determine (nested) block segment positions
    #
    $opened  = 0;
    $offset  = 0;
    @segment = (0);
    while (1)
    {
        $openidx  = index( $buffer, $opendel,  $offset );
        $closeidx = index( $buffer, $closedel, $offset );
        $self->_wml_back_end_asubst_debug( 1,
            "buffer=<>, off=$offset, o=$openidx, c=$closeidx\n" );
        if ( $openidx == -1 && $closeidx == -1 )
        {
            #   both not found, stop now
            push( @segment, length($buffer) );
            last;
        }
        if ( $openidx != -1 && $closeidx != -1 )
        {
            #   both found, take closer one
            ( $offset, $del, $opened ) = (
                $openidx < $closeidx
                ? ( $openidx, $opendel, $opened + 1 )
                : ( $closeidx, $closedel, $opened - 1 )
            );
        }
        else
        {
            #   one not found, take other one
            ( $offset, $del, $opened ) = (
                $openidx != -1
                ? ( $openidx, $opendel, $opened + 1 )
                : ( $closeidx, $closedel, $opened - 1 )
            );
        }
        $offset = $offset + length($del);

        #   still reached a complete segment
        if ( $opened == 0 )
        {
            push( @segment, $offset );
        }
    }
    if ( $opened != 0 )
    {
        return ( 1, "invalid number of opening and closing delimiters" );
    }

    #
    #   now process each segment
    #
    $bufferN = '';
    for ( my $i = 0 ; $i < $#segment ; )
    {
        $s    = $segment[$i];
        $e    = $segment[ ++$i ];
        $data = substr( $buffer, $s, ( $e - $s ) );
        my $rc;
        ( $rc, $data ) = $self->_expand_block_one(
            $opendel, $closedel, $data,    $cnvpre, $opendel,
            $cnvin,   $closedel, $cnvpost, $level
        );
        if ( $rc != 0 )
        {
            return ( $rc, $data );
        }
        $bufferN .= $data;
    }

    return ( 0, $bufferN );
}

#   This subvariant operates only on a buffer which
#   contains one block (which can be still nested).
#   input: "... < < > > ... "
#
sub _expand_block_one
{
    my (
        $self,     $opendel, $closedel, $buffer,  $cnvpre,
        $startdel, $cnvin,   $enddel,   $cnvpost, $level
    ) = @_;
    my ( $openidx, $closeidx, $prefix, $postfix, $inner, $rc, $data );

    $openidx  = index( $buffer, $opendel );
    $closeidx = rindex( $buffer, $closedel );

    #   either both exist or both not exist
    if ( $openidx == -1 && $closeidx == -1 )
    {
        if ( $level == 0 )
        {
            $data = $cnvpre->( $buffer, $level );    # could also be cnvpost..
        }
        else
        {
            $data = $buffer;
        }
        return ( 0, $data );
    }
    else
    {
        #   convert prefix
        $prefix = $cnvpre->( substr( $buffer, 0, $openidx ), $level );
        $self->_wml_back_end_asubst_debug( $level, "ExpandBlockOne::prefix",
            $prefix );

        #   recursive into the body
        $inner = substr(
            $buffer,
            $openidx + length($opendel),
            $closeidx - ( $openidx + length($opendel) )
        );
        $self->_wml_back_end_asubst_debug( $level, "ExpandBlockOne::inner",
            $inner );
        ( $rc, $inner ) = $self->_expand_block_more(
            $inner,    $cnvpre,  $opendel, $cnvin,
            $closedel, $cnvpost, $level + 1
        );
        $self->_wml_back_end_asubst_debug( $level, "ExpandBlockOne::inner",
            $inner );
        $inner = $cnvin->( $inner, $level + 1 );
        $self->_wml_back_end_asubst_debug( $level, "ExpandBlockOne::inner",
            $inner );

        #   convert postfix
        $postfix = $cnvpost->(
            substr(
                $buffer,
                $closeidx + length($closedel),
                length($buffer) - ( $closeidx + length($closedel) )
            ),
            $level
        );
        $self->_wml_back_end_asubst_debug( $level, "ExpandBlockOne::postfix",
            $postfix );

        return ( $rc, $prefix . $inner . $postfix );
    }
}

#   A debugging function
sub _wml_back_end_asubst_debug
{
    my ( $self, $level, $name, $str ) = @_;
    my (@o);

    # return if ($_my_debug == 0);
    return;

    push( @o, "    " x $level . "### $name =\n" );
    if ( $str eq '' )
    {
        push( @o, "    " x $level . "    ||\n" );
    }
    else
    {
        foreach my $l ( split( '\n', $str ) )
        {
            push( @o, "    " x $level . "    |$l|\n" );
        }
    }
    print STDERR @o;

    return;
}

sub main
{
    my ($self) = @_;
    my $opt_o = '-';
    $Getopt::Long::bundling      = 1;
    $Getopt::Long::getopt_compat = 0;
    if (
        not Getopt::Long::GetOptionsFromArray(
            $self->argv,
            "v|verbose"      => do { my $x; \$x; },
            "o|outputfile=s" => \$opt_o,
        )
        )
    {
        usage();
    }

    my $buffer = $self->_input;
    my $rc;
    if ( index( $buffer, '{:' ) != -1 )
    {
        ( $rc, $buffer ) =
            $self->_expand_block( $buffer, \&cnvpre, '{:', \&cnvin, ':}',
            \&cnvpost, 0 );
    }

    if ($rc)
    {
        die "aSubst:Error: $buffer\n";
    }

    $self->_out( $opt_o, [$buffer], );

    return;
}

1;
