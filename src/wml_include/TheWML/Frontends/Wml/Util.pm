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

package TheWML::Frontends::Wml::Util;

use strict;
use warnings;
use Cwd ();

use parent 'Exporter';

our @EXPORT_OK = qw/ _my_cwd canon_path canonize_path ctime
    error expandrange gmt_ctime gmt_isotime
    isotime make_shell_safe quotearg split_argv time_record usage user_record /;

sub expandrange
{
    my ( $s, $e ) = @_;
    return join '', ( $s .. $e );
}

sub _my_cwd
{
    my $cwd = Cwd::cwd;
    $cwd =~ s#/\z##;
    return $cwd;
}

sub ctime
{
    my ($time) = @_;
    return scalar( localtime($time) );
}

sub isotime
{
    my ($time) = @_;

    my ( $sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst ) =
        localtime($time);
    return sprintf(
        "%04d-%02d-%02d %02d:%02d:%02d",
        $year + 1900,
        $mon + 1, $mday, $hour, $min, $sec
    );
}

sub gmt_ctime
{
    my ($time) = @_;
    return scalar( gmtime($time) );
}

sub gmt_isotime
{
    my ($time) = @_;

    my ( $sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst ) =
        gmtime($time);
    return sprintf(
        "%04d-%02d-%02d %02d:%02d:%02d",
        $year + 1900,
        $mon + 1, $mday, $hour, $min, $sec
    );
}

sub time_record
{
    my ($time) = @_;
    return +{
        ctime       => ctime($time),
        gmt_ctime   => gmt_ctime($time),
        gmt_isotime => gmt_isotime($time),
        isotime     => isotime($time),
        time        => $time,
    };
}

sub make_shell_safe
{
    my ($str) = @_;

    $$str =~ s|[\'\$\`\"]||g;    # make safe for shell interpolation

    return;
}

sub user_record
{
    my ($uid) = @_;

    my @pwinfo   = getpwuid($uid);
    my $username = $pwinfo[0] || 'UNKNOWN-USERNAME';
    make_shell_safe( \$username );
    my $realname = $pwinfo[6] || 'UNKNOWN-REALNAME';
    $realname =~ s|^([^\,]+)\,.*$|$1|;
    make_shell_safe( \$realname );

    return { username => $username, realname => $realname };
}

sub usage
{
    my ($progname) = @_;

    print STDERR <<"EOF";
Usage: $progname [options] [inputfile]

Input Options:
  -I, --include=PATH      adds an include directory
  -i, --includefile=PATH  pre-include a particular include file
  -D, --define=NAME[=STR] define a variable
  -D, --define=NAME~PATH  define an auto-adjusted path variable
  -n, --noshebang         no shebang-line parsing (usually used by WMk)
  -r, --norcfile          no .wmlrc files are read
  -c, --nocd              read .wmlrc files without changing to input file directory

Output Options:
  -O, --optimize=NUM      specify the output optimization level
  -o, --outputfile=PATH   specify the output file(s)
  -P, --prolog=PATH       specify one or more prolog filters
  -E, --epilog=PATH       specify one or more epilog filters
  -t, --settime           sets mtime of outputfile(s) to mtime+1 of inputfile

Processing Options:
  -M, --depend[=OPTIONS]  dump dependencies as gcc does
  -p, --pass=STR          specify which passed should be run
  -W, --passoption[=PASS,OPTIONS]
                          set options for a specific pass
  -s, --safe              don't use precompile/inline hacks to speedup processing
  -v, --verbose[=NUM]     verbose mode
  -q, --quiet             quiet mode

Giving Feedback:
  -V, --version[=NUM]     display version and build information
  -h, --help              display this usage summary

EOF
    exit(1);
}

sub error
{
    my ($str) = @_;
    print STDERR "** WML:Error: $str\n";
    exit(1);
}

#   escape options if not quoted but
#   when shell metachars exists
sub quotearg
{
    my ($arg) = @_;
    if ( not( $arg =~ /\A'/ and $arg =~ /'\z/ ) )
    {
        $arg =~ s#([\$"`])#\\$1#gs;
    }
    return $arg;
}

#   helper function to split argument line
#   the same way Bourne-Shell does:
#   #1: foo=bar quux   => "foo=bar", "quux"
#   #2: "foo=bar quux" => "foo=bar quux"
#   #3: foo="bar quux" => "foo=bar quux"     <-- !!
sub split_argv
{
    my ($str) = @_;
    my @argv;
    my $r    = '';
    my $prev = '';

SPLIT_ARGV: while (1)
    {
        $prev = $str;
        if (   $str =~ s#\A"([^"\\]*(?:\\.[^"\\]*)*)"#$r .= $1;''#e
            or $str =~ s#\A'([^'\\]*(?:\\.[^'\\]*)*)'#$r .= $1;''#e
            or $str =~ s#\A([^\s"']+)#$r .= $1;''#e )
        {
            next SPLIT_ARGV;
        }
        if ( $str =~ /\A[\s\n]/ or $str eq '' )
        {
            if ( $r ne '' )
            {
                push( @argv, $r );
                $r = '';
            }
            $str =~ s#\A[\s\n]+##;
            last if ( $str eq '' );
        }
        if ( $str eq $prev )
        {
            #    breaks an infinite loop
            print STDERR "** WML:Error: options can not be correctly parsed\n";
            exit(1);
        }
    }
    return @argv;
}

sub canon_path
{
    my ($path) = @_;

    my $pathL = '';
    while ( $path ne $pathL )
    {
        $pathL = $path;
        $path =~ s|//|/|g;
        $path =~ s|/\./|/|g;
        $path =~ s|/\.$|/|g;
        $path =~ s|^\./(.)|$1|g;
        $path =~ s|([^/.][^/.]*)/\.\.||;
    }
    return $path;
}

sub canonize_path
{
    my ( $path, $reldir ) = @_;
    $$path = canon_path( ( $$path eq '.' ) ? "$reldir" : "$reldir/$$path" );
    return;
}

1;
