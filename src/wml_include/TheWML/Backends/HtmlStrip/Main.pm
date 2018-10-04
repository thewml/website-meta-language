##
##  htmlstrip -- Strip HTML markup code
##  Copyright (c) 1997-2000 Ralf S. Engelschall, All Rights Reserved.
##  Copyright (c) 2000 Denis Barbier
##
package TheWML::Backends::HtmlStrip::Main;

use strict;
use warnings;

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            opt_O
            opt_b
            opt_o
            opt_v
            )
    },
);

#
#   process command line
#
sub usage
{
    print STDERR "Usage: htmlstrip [options] [file]\n";
    print STDERR "\n";
    print STDERR "Options:\n";
    print STDERR
        "  -o, --outputfile=<file>   set output file instead of stdout\n";
    print STDERR "  -O, --optimize=<level>    set optimization/crunch level\n";
    print STDERR "  -v, --verbose             verbose mode\n";
    die;
}

sub verbose
{
    my ( $self, $str ) = @_;
    if ( $self->opt_v )
    {
        print STDERR "** HTMLstrip:Verbose: $str\n";
    }
}

sub error
{
    my ($str) = @_;
    die "** HTMLstrip:Error: $str\n";
}

#
#   read input file
#
use TheWML::Backends;

#
#   stripping functions for particular areas
#

sub _nexttoken
{
    my ($buf) = @_;
    my ( $token, $bufN );

    if ( $buf =~ m|^([^<]+?)(<.+)$|s )
    {
        $token = $1;
        $bufN  = $2;
    }
    elsif ( $buf =~ m|^(<[^>]+>)(.*)$|s )
    {
        $token = $1;
        $bufN  = $2;
    }
    else
    {
        $token = $buf;
        $bufN  = '';
    }

    if ( length($token) > 80 )
    {
        my $x = substr( $token, 0, 80 );
        my $i = rindex( $x, ' ' );
        my $bufN = substr( $token, $i ) . $bufN;
        $token = substr( $token, 0, $i );
    }
    return ( $token, $bufN );
}

#   Strip Plain Text, i.e. outside of any
#   preformatted area and outside any HTML tag.
sub _strip_plain_text
{
    my ( $self, $buf ) = @_;

    #   Level 0
    #if ($self->opt_O >= 0) {
    #}
    #   Level 1
    if ( $self->opt_O >= 1 )
    {
        #   strip empty lines
        $buf =~ s|\n\s*\n|\n|sg;
    }

    #   Level 2
    if ( $self->opt_O >= 2 )
    {
        #   strip multiple whitespaces to single one
        $buf =~ s|(\S+)[ \t]{2,}|$1 |sg;

        #   strip trailing whitespaces
        $buf =~ s|\s+\n|\n|sg;
    }

    #   Level 3
    if ( $self->opt_O >= 3 )
    {
        #   strip leading whitespaces
        $buf =~ s|\n\s+|\n|sg;
    }

    #   Level 4
    if ( $self->opt_O >= 4 )
    {
        #   strip empty lines again
        $buf =~ s|^\s*$||mg;
        $buf =~ s|\n\n|\n|sg;
    }

    #   Level 5
    if ( $self->opt_O >= 5 )
    {
        #   concatenate all lines
        $buf =~ s|\n| |sg;
        #
        my $from = $buf;
        my $line = '';
        $buf = '';
        while ( length($from) > 0 )
        {
            my ( $token, $from ) = _nexttoken($from);
            if ( ( length($line) + length($token) ) < 80 )
            {
                $line .= $token;
            }
            else
            {
                $buf .= $line . "\n";
                $line = $token;
            }
        }
        $buf =~ s|^\s+||mg;
        $buf =~ s|\s+$||mg;
    }

    return $buf;
}

#   Strip HTML Tag, i.e. outside of any
#   preformatted area but inside a HTML tag.
sub _strip_html_tag
{
    my ( $self, $buf ) = @_;

    #   Level 0
    #if ($self->opt_O >= 0) {
    #}
    #   Level 1
    #if ($self->opt_O >= 1) {
    #}
    #   Level 2
    if ( $self->opt_O >= 2 )
    {
        #   strip multiple whitespaces to single one
        $buf =~ s|(\S+)[ \t]{2,}|$1 |mg;

        #   strip trailing whitespaces at end of line
        $buf =~ s|\s+\n|\n|sg;

        #   strip whitespaces between attribute name and value
        $buf =~ s|([ \t]+[a-zA-Z][a-zA-Z0-9_]*)\s*=\s*|$1=|sg;

        #   strip whitespaces before tag end
        $buf =~ s|[ \t]+>$|>|sg;
    }

    #   Level 3
    #if ($self->opt_O >= 3) {
    #}
    #   Level 4
    if ( $self->opt_O >= 4 )
    {
        #   strip HTML comments
        $buf =~ s|<!--.+?-->||sg;

        #   strip newlines before tag end
        $buf =~ s|\n>$|>|sg;
    }

    #   Level 5
    #if ($self->opt_O >= 5) {
    #}

    return $buf;
}

#   Strip Preformatted Areas, i.e.  inside
#   <pre>, <xmp> and <nostrip> container tags.
sub _strip_preformatted
{
    my ( $self, $buf ) = @_;

    #   Level 0
    #if ($self->opt_O >= 0) {
    #}
    #   Level 1
    #if ($self->opt_O >= 1) {
    #}
    #   Level 2
    if ( $self->opt_O >= 2 )
    {
        #   strip trailing whitespaces on non-empty lines
        $buf =~ s|\S\K[ \t]+\n|\n|sg;
    }

    #   Level 3
    #if ($self->opt_O >= 3) {
    #}
    #   Level 4
    #if ($self->opt_O >= 4) {
    #}
    #   Level 5
    #if ($self->opt_O >= 5) {
    #}

    return $buf;
}

sub _strip_non_preformatted
{
    my ( $self, $I ) = @_;

    my $O = '';
    while ( $I =~ s|^(.*?)(<.+?>)||s )
    {
        my ( $text, $tag ) = ( $1, $2 );
        $O .= $self->_strip_plain_text($text) . $self->_strip_html_tag($tag);
    }
    $O .= $self->_strip_plain_text($I);
    return $O;
}

1;

# __END__
# # Below is stub documentation for your module. You'd better edit it!
