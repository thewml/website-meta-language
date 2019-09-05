##  htmlfix -- Fixup HTML markup code
##  Copyright (c) 1997-2000 Ralf S. Engelschall, All Rights Reserved.
##  Copyright (c) 2000 Denis Barbier

package TheWML::Backends::Fixup::Main;

use strict;
use warnings;
use 5.014;

use Getopt::Long ();

use Image::Size ();

use parent 'TheWML::CmdLine::Base';

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            argv
            opt_v
            bytes
            _buffer
            )
    },
);

sub usage
{
    print STDERR <<'EOF';
Usage: htmlfix [options] [file]

Options:
  -o, --outputfile=<file>  set output file instead of stdout
  -F, --fix=<fixes>        select which fix to apply
  -S, --skip=<fixes>       skip specified fixes
  -v, --verbose            verbose mode\n
Fixes are a comma separated list of (default is to process them all)
  imgalt : add ALT attributes to IMG tags
  imgsize: add WIDTH/HEIGHT attributes to IMG tags
  summary: add SUMMARY attribute to TABLE tags
  center : change proprietary CENTER tag to standard DIV tag
  space  : fix trailing spaces in tags
  quotes : add missing quotes for attributes and missing '#' character\n           to color attributes
  indent : indent paragraphs
  comment: out-comment tags
  tagcase: perform tag case-conversion
EOF
    die;
}

sub _name
{
    return 'HTMLfix';
}

#   processing loop
#
#   Definitions of fixups
#   Some attention has been paid for efficiency in regular expressions,
#   this is why they appear more complicated than needed.

#   FIXUP 1: add WIDTH/HEIGHT/ALT attributes to <img>-tags
sub _process_img_tag
{
    my ( $self, $attr ) = @_;
    my ( $Nwidth, $Nheight, $Pwidth, $Pheight );

    if (
        not(   $attr =~ m|SRC\s*=\s*"([^"]*)"|is
            or $attr =~ m|SRC\s*=\s*(\S+)|is )
        )
    {
        return $attr;
    }
    my $image = $1;

    my $size = -s $image;

    if ( !$size )
    {
        return $attr;
    }
    $self->bytes( $self->bytes() + $size );

    #   add WIDTH and HEIGHT to speed up display
    my $width  = -1;
    my $height = -1;
    my $scale  = 1;
    if (   $attr =~ m/WIDTH\s*=\s*([0-9%]+|\*)/is
        or $attr =~ m/WIDTH\s*=\s*"([0-9%]+|\*)"/is )
    {
        $width = $1;
    }
    if (   $attr =~ m/HEIGHT\s*=\s*([0-9%]+|\*)/is
        or $attr =~ m/HEIGHT\s*=\s*"([0-9%]+|\*)"/is )
    {
        $height = $1;
    }
    if (   $attr =~ s/SCALE\s*=\s*([0-9]+)%//is
        or $attr =~ s/SCALE\s*=\s*"([0-9]+)%"//is )
    {
        $scale = $1 / 100;
    }
    if (   $attr =~ s/SCALE\s*=\s*([0-9.]+)//is
        or $attr =~ s/SCALE\s*=\s*"([0-9.]+)"//is )
    {
        $scale = $1;
    }
    if (
        not(   $width eq '*'
            or $width == -1
            or $height eq '*'
            or $height == -1 )
        )
    {
        return $attr;
    }
    my $error;
    ( $Pwidth, $Pheight, $error ) = Image::Size::imgsize($image);
    if ( defined($Pwidth) and defined($Pheight) )
    {

        #    width given, height needs completed
        if (    ( not( $width eq '*' or $width == -1 ) )
            and ( $height eq '*' or $height == -1 ) )
        {
            $Nheight =
                ( $width == $Pwidth )
                ? $Pheight
                : int( ( $Pheight / $Pwidth ) * $width );
        }

        #   height given, width needs completed
        elsif ( ( not( $height eq '*' or $height == -1 ) )
            and ( $width eq '*' or $width == -1 ) )
        {
            $Nwidth =
                ( $height == $Pheight )
                ? $Pwidth
                : int( ( $Pwidth / $Pheight ) * $height );
        }

        #   both width and height needs completed
        elsif ( ( $height eq '*' or $height == -1 )
            and ( $width eq '*' or $width == -1 ) )
        {
            $Nwidth  = $Pwidth;
            $Nheight = $Pheight;
        }

        #   optionally scale the dimensions
        if ( $scale != 1 )
        {
            $Nwidth  = int( $Nwidth * $scale );
            $Nheight = int( $Nheight * $scale );
        }

        #   now set the new values
        if ( $width eq '*' )
        {
            $attr =~ s|(WIDTH\s*=\s*)\S+|$1$Nwidth|is;
            $self->verbose("substituting width for $image: ``width=$Nwidth''");
        }
        elsif ( $width == -1 )
        {
            $attr .= " width=$Nwidth";
            $self->verbose("adding width for $image: ``width=$Nwidth''");
        }
        if ( $height eq '*' )
        {
            $attr =~ s|(HEIGHT\s*=\s*)\S+|$1$Nheight|is;
            $self->verbose(
                "substituting height for $image: ``height=$Nheight''");
        }
        elsif ( $height == -1 )
        {
            $attr .= " height=$Nheight";
            $self->verbose("adding height for $image: ``height=$Nheight''");
        }
    }
    else
    {
        #   complain
        $self->verbose("cannot complete size of $image: $error");

        #   and make sure the =* placeholder constructs are removed
        $attr =~ s|WIDTH\s*=\s*\*||is;
        $attr =~ s|HEIGHT\s*=\s*\*||is;
    }

    return $attr;
}

sub _fixup_imgalt
{
    my ( $self, ) = @_;

    my $bufferN = '';

    if ( !defined( ${ $self->_buffer } ) ) { die "CLamm oo Buffer is undef." }

    while ( ${ $self->_buffer } =~ s|^(.*?)(<[iI][mM][gG]\s+)([^>]+?)(/?>)||s )
    {
        my ( $pre, $tag, $attr, $end ) = ( $1, $2, $3, $4 );
        if (    $attr !~ m|ALT\s*=\s*"[^"]*"|is
            and $attr !~ m|ALT\s*=\s*\S+|is )
        {
            $self->verbose("adding ALT for image");
            $attr .= ' alt=""';
        }
        $bufferN .= $pre . $tag . $attr . $end;
    }
    ${ $self->_buffer } = $bufferN . ${ $self->_buffer };

    return;
}

sub _fixup_imgsize
{
    my ( $self, ) = @_;

    my $bufferN = '';
    while ( ${ $self->_buffer } =~ s|^(.*?)(<[iI][mM][gG]\s+)([^>]+?)(/?>)||s )
    {
        my ( $pre, $tag, $attr, $end ) = ( $1, $2, $3, $4 );
        $bufferN .= $pre . $tag . $self->_process_img_tag($attr) . $end;
    }
    ${ $self->_buffer } = $bufferN . ${ $self->_buffer };

    return;
}

#   FIXUP 2: add summary attribute to <table>-tags
sub _fixup_summary
{
    my ( $self, ) = @_;

    $self->verbose("adding summary attribute to <table>");

    my $last    = 0;
    my $bufferN = '';
    while ( ${ $self->_buffer } =~
        m|\G(.*?)(<[tT][aA][bB][lL][eE])([^>]*?)(/?>)|gs )
    {
        $last = pos( ${ $self->_buffer } );
        my ( $pre, $begin, $attr, $end ) = ( $1, $2, $3, $4 );

        #   add a SUMMARY="" tag to make HTML lints happy
        if ( $attr !~ m|SUMMARY\s*=|i )
        {
            $attr .= ' summary=""';
        }
        $bufferN .= $pre . $begin . $attr . $end;
    }
    ${ $self->_buffer } = $bufferN . substr( ${ $self->_buffer }, $last );

    return;
}

#   FIXUP 3: change <center>..</center> to <div align=center>..</div>
sub _fixup_center
{
    my ( $self, ) = @_;

    $self->verbose(
        "replacing <center>..</center> by <div align=center>..</div>");

    ${ $self->_buffer } =~
        s|<[cC][eE][nN][tT][eE][rR]((?:\s[^>ck]*)?)>|<div align="center"$1>|g;
    ${ $self->_buffer } =~ s|</[cC][eE][nN][tT][eE][rR]>|</div>|g;

    return;
}

#   FIXUP 4: fix trailing space in tags
sub _fixup_space
{
    my ( $self, ) = @_;

    $self->verbose("trailing space in tags");

    #   Only space characters are removed, neither tabs nor newlines
    ${ $self->_buffer } =~ s| +>|>|g;
    ${ $self->_buffer } =~ s|([^\s])/>|$1 />|g;

    return;
}

#   FIXUP 5: add quotations to attribute values and
#            add missing '#' char to color attributes
sub _fixup_quotes
{
    my ( $self, ) = @_;

    $self->verbose("add quotes to attributes");

    my $last    = 0;
    my $bufferN = '';
    while ( ${ $self->_buffer } =~ m|\G(.*?)(<[a-zA-Z_][^>]*>)|sg )
    {
        $last = pos( ${ $self->_buffer } );
        my ( $prolog, $tag ) = ( $1, $2 );
        $tag =~ s@([A-Za-z_-]+=)([^\s\"\'><\[]+)(\s|/?>)@$1"$2"$3@sg;
        $tag =~ s|([A-Za-z_-]+=")([0-9A-Fa-f]{6}"[\s/>])|$1#$2|sg;
        $bufferN .= $prolog . $tag;
    }
    ${ $self->_buffer } = $bufferN . substr( ${ $self->_buffer }, $last );

    return;
}

#   FIXUP 6: paragraph indentation
sub _process_indent_container
{
    my ( $attr, $data ) = @_;

    #   determine amount of padding
    my $num  = 0;
    my $size = 4;
    $attr =~ s/num\s*=\s*"?(\d+)"?/$num = $1, ''/ige;
    $attr =~ s/size\s*=\s*"?(\d+)"?/$size = $1, ''/ige;

    #   pad the data
    if ( $num > 0 )
    {
        my $pad = ' ' x ( $num * $size );
        $data =~ s/^/$pad/mg;
    }
    elsif ( $num == 0 )
    {
        ( my $prefix ) = ( $data =~ m|^\n*([ \t]+)|s );
        if ( length($prefix) > 0 )
        {
            $data =~ s/^$prefix//mg;
        }
    }
    return $data;
}

sub _fixup_indent
{
    my ( $self, ) = @_;

    $self->verbose("paragraph indentation");

    if ( ${ $self->_buffer } =~ m|<[iI][nN][dD][eE][nN][tT][\s>]| )
    {
        my $bufferN = '';
        while (
            ${ $self->_buffer } =~ s|^(.*?)<indent([^>]*)>(.*?)</indent>||is )
        {
            my ( $pre, $attr, $data ) = ( $1, $2, $3 );
            $bufferN .= $pre . _process_indent_container( $attr, $data );
        }
        ${ $self->_buffer } = $bufferN . ${ $self->_buffer };
    }

    return;
}

#   FIXUP 7: out-commenting tags
sub _fixup_comment
{
    my ( $self, ) = @_;

    $self->verbose("remove commenting tags");
    ${ $self->_buffer } =~ s|<[a-zA-Z_][a-zA-Z0-9-]*#.*?>||sg;
    ${ $self->_buffer } =~ s|</[a-zA-Z_][a-zA-Z0-9-]*#>||sg;

    return;
}

#   FIXUP 8: tag case translation
sub _doit_upper
{
    my ( $prefix, $body ) = @_;
    $prefix =~ s/^(.+)$/\U$1\E/;
    $body =~
s/(\s+[a-zA-Z][a-zA-Z0-9_-]*)(\s*=\s*[^"\s]+|\s*=\s*"[^"]*"|\/?>|\s)/\U$1\E$2/sg;
    return $prefix . $body;
}

sub _doit_lower
{
    my ( $prefix, $body ) = @_;
    $prefix =~ s/^(.+)$/\L$1\E/;
    $body =~
s/(\s+[a-zA-Z][a-zA-Z0-9_-]*)(\s*=\s*[^"\s]+|\s*=\s*"[^"]*"|\/?>|\s)/\L$1\E$2/sg;
    return $prefix . $body;
}

sub _process_tag_conv
{
    my ( $attr, $data ) = @_;

    #   determine case translation type
    my $case = 'upper';
    $attr =~ s/case\s*=\s*"?(upper|lower)"?/$case = lc($1), ''/ige;

    #   and then translate the data
    if ( $case eq 'upper' )
    {
        $data =~ s|(<[a-zA-Z][a-zA-Z0-9_-]*\s*/?>)|\U$1\E|sg;
        $data =~ s|(<[a-zA-Z][a-zA-Z0-9_-]*)(\s+.*?/?>)|_doit_upper($1,$2)|sge;
        $data =~ s|(<\/[a-zA-Z][a-zA-Z0-9_-]*\s*/?>)|\U$1\E|sg;
    }
    else
    {
        $data =~ s|(<[a-zA-Z][a-zA-Z0-9_-]*\s*/?>)|\L$1\E|sg;
        $data =~ s|(<[a-zA-Z][a-zA-Z0-9_-]*)(\s+.*?>)|_doit_lower($1,$2)|sge;
        $data =~ s|(<\/[a-zA-Z][a-zA-Z0-9_-]*\s*/?>)|\L$1\E|sg;
    }
    return $data;
}

sub _fixup_tagcase
{
    my ( $self, ) = @_;

    $self->verbose("tag case translation");

    if ( ${ $self->_buffer } =~ m|<[tT][aA][gG][cC][oO][nN][vV][\s>]| )
    {
        my $bufferN = '';
        while (
            ${ $self->_buffer } =~ s|^(.*?)<tagconv([^>]*)>(.*?)</tagconv>||is )
        {
            my ( $pre, $attr, $data ) = ( $1, $2, $3 );
            $bufferN .= $pre . _process_tag_conv( $attr, $data );
        }
        ${ $self->_buffer } = $bufferN . ${ $self->_buffer };
    }
}

sub run
{
    my ( $self, $opt_S, $opt_F ) = @_;

    # process all required fixups
    my %skips = (
        map {
            my $s = $_;
            map { $_ => 1 } split( ',', $s )
        } @$opt_S
    );
FIXUP:
    foreach my $m ( map { my $s = $_; split( ',', $s ) } @$opt_F )
    {
        if ( exists $skips{$m} )
        {
            next FIXUP;
        }
        if ( $m =~ /[^a-z]/ )
        {
            die "invalid chars in identifier '$m'!";
        }
        my $fixup = '_fixup_' . $m;
        if ( my $ref = $self->can($fixup) )
        {
            $ref->($self);
        }
    }

    # statistic
    $self->verbose("Total amount of images: @{[$self->bytes]} bytes");

    return;
}

sub main
{
    my ( $self, $param ) = @_;

    my $opt_q = 0;
    my $opt_v = 0;
    my $opt_o = '-';
    my @opt_F;
    my @opt_S;
    $Getopt::Long::bundling      = 1;
    $Getopt::Long::getopt_compat = 0;

    if (
        not Getopt::Long::GetOptionsFromArray(
            $self->argv,
            "v|verbose"      => \$opt_v,
            "q"              => \$opt_q,
            "F|fix=s"        => \@opt_F,
            "S|skip=s"       => \@opt_S,
            "o|outputfile=s" => \$opt_o
        )
        )
    {
        usage();
    }
    if ( !@opt_F )
    {
        push @opt_F,
            'imgalt,imgsize,summary,center,space,quotes,indent,comment,tagcase';
    }

    my $buffer = $self->_input;

    if ( !defined($buffer) ) { die "Egloo Buffer is undef." }

    $self->opt_v($opt_v);
    $self->bytes(0);
    $self->_buffer( \$buffer );
    $self->run( \@opt_S, \@opt_F );

    $self->_out( $opt_o, [$buffer] );
    return;
}

1;
