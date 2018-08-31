##
##  IPP -- Include Pre-Processor
##  Copyright (c) 1997,1998,1999 Ralf S. Engelschall, All Rights Reserved.
##  Copyright (c) 2000 Denis Barbier, All Rights Reserved.
##

package WML_Backends::IPP::File;

use 5.014;

use strict;
use warnings;

use parent 'WML_Frontends::Wml::Base';

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            _main
            )
    },
);

use IO::All qw/ io /;
use WML_Frontends::Wml::Util qw/ canon_path /;
use WML_Backends::IPP::Line ();

sub _PatternProcess_helper
{
    my (
        $self,    $test, $out,   $mode,  $_del, $dirname,
        $pattern, $ext,  $level, $no_id, $arg
    ) = @_;
    if ( not -d $dirname )
    {
        return;
    }
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

        $$out .=
            $self->_main->ProcessFile( $mode, $_del, $arg->{'IPP_THIS'}, "",
            $level, $no_id, $arg );
    }
    delete @$arg{qw/IPP_NEXT IPP_THIS IPP_PREV/};
    return;
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
                    $self->_main->ProcessFile( $mode, $_del, "$dirname/$_$ext",
                    "", $level, $no_id, $arg );
                $found = 1;
            }
            last DIRS if $found;
        }

        return;

    };

    if ( $_del->is_ang )
    {
        $process_dirs->( $self->_main->opt_S );
    }
    if ( $_del->is_quote )
    {
        $process_dirs->( $self->_main->opt_I );
    }
    if ( $_del->is_quote_all )
    {
        $self->_PatternProcess_helper(
            $test,    \$out, $mode,  $_del,  $dirname,
            $pattern, $ext,  $level, $no_id, $arg
        );
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
    if ( not $self->_main->_find_file( $_del, \$fn ) )
    {
        error("file not found: $fn");
    }

    #   stop if file was still included some time before
    if ( not $no_id )
    {
        my $id = canon_path($fn);
        if ( $mode eq 'use' )
        {
            return '' if ( exists $self->_main->INCLUDES->{$id} );
        }
        $self->_main->INCLUDES->{$id} = $_del->is_ang ? 1 : 2;
    }

    # Stop if just want to check dependency
    return '' if $mode eq 'depends';

    # Process the file
    $realname = $fn if $realname eq '';
    $self->_main->verbose( $level, "|" );
    $self->_main->verbose( $level, "+-- $fn" );
    my $in       = io()->file($fn);
    my $line_idx = 0;
    my $out      = '';
    if ( not $self->_main->opt_N and not $arg->{'IPP_NOSYNCLINES'} )
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

        my $op = WML_Backends::IPP::Line->new(
            _main    => $self->_main,
            arg      => $arg,
            l        => \$l,
            line_idx => $line_idx,
            out      => \$out,
            realname => $realname,
        )->_process_line( \$store, $level, $fn, ) // '';
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

1;
