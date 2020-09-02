##
##  IPP -- Include Pre-Processor
##  Copyright (c) 1997,1998,1999 Ralf S. Engelschall, All Rights Reserved.
##  Copyright (c) 2000 Denis Barbier, All Rights Reserved.
##

package TheWML::Backends::IPP::File;

use 5.014;

use strict;
use warnings;

use parent 'TheWML::CmdLine::Base';

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            _del
            _main
            level
            mode
            no_id
            )
    },
);

use Path::Tiny qw/ path /;

use TheWML::Frontends::Wml::Util qw/ canon_path error /;
use TheWML::Backends::IPP::Line ();

sub _PatternProcess_helper
{
    my ( $self, $test, $out, $dirname, $pattern, $ext, $arg ) = @_;
    if ( not -d $dirname )
    {
        return;
    }
    my @ls =
        grep { /\A$pattern\z/ && $test->('.') } path($dirname)->children();

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
            $self->_main->_process_file( $self->mode, $self->_del,
            $arg->{'IPP_THIS'}, "", $self->level, $self->no_id, $arg );
    }
    delete @$arg{qw/IPP_NEXT IPP_THIS IPP_PREV/};
    return;
}

sub PatternProcess
{
    my ( $self, $dirname, $pattern, $ext, $arg ) = @_;

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
            my @ls = grep { /\A$pattern\z/ && $test->($dir) }
                path("$dir/$dirname")->children();
            my $found = 0;
        LS:
            foreach (@ls)
            {
                next LS if ( m|/\.+$| or m|^\.+$| );
                $out .=
                    $self->_main->_process_file( $self->mode, $self->_del,
                    "$dirname/$_$ext", "", $self->level, $self->no_id, $arg );
                $found = 1;
            }
            last DIRS if $found;
        }

        return;

    };

    if ( $self->_del->is_ang )
    {
        $process_dirs->( $self->_main->opt_S );
    }
    if ( $self->_del->is_quote )
    {
        $process_dirs->( $self->_main->opt_I );
    }
    if ( $self->_del->is_quote_all )
    {
        $self->_PatternProcess_helper( $test, \$out, $dirname, $pattern,
            $ext, $arg );
    }
    return $out;
}

sub _expand_pattern
{
    my ( $self, $dirname, $pattern, $ext, $arg ) = @_;
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
    return $self->PatternProcess( $dirname, $pattern, $ext, +{%$arg} );
}

sub _find_file
{
    my ( $self, $fn ) = @_;

    #    this is a regular file
    my $found = 0;

    my $process_dirs = sub {
    OPT:
        foreach my $dir ( reverse @{ shift @_ } )
        {
            if ( -f "$dir/$$fn" )
            {
                $$fn   = "$dir/$$fn";
                $found = 1;
                last OPT;
            }
        }
        return;
    };

    if ( $self->_del->is_ang )
    {
        $process_dirs->( $self->_main->opt_S );
    }
    if ( $self->_del->is_quote )
    {
        $process_dirs->( $self->_main->opt_I );
    }
    if ( $self->_del->is_quote_all )
    {
        if ( -f $$fn )
        {
            $found = 1;
        }
    }
    return $found;
}

sub _process_file
{
    my ( $self, $fn, $realname, $in_arg ) = @_;

    my $arg = +{%$in_arg};

    #   first check whether this is a filename pattern in which case
    #   we must expand it
    if ( my ( $dirname, $pattern, $ext ) =
        ( $fn =~ m/^(.*?)(?=[?*\]])([?*]|\[[^\]]*\])(.*)$/ ) )
    {
        return $self->_expand_pattern( $dirname, $pattern, $ext, $arg );
    }
    if ( not $self->_find_file( \$fn ) )
    {
        error("file not found: $fn");
    }

    #   stop if file was still included some time before
    if ( not $self->no_id )
    {
        my $id = canon_path($fn);
        if ( $self->mode eq 'use' )
        {
            return '' if ( exists $self->_main->INCLUDES->{$id} );
        }
        $self->_main->INCLUDES->{$id} = $self->_del->is_ang ? 1 : 2;
    }

    # Stop if just want to check dependency
    return '' if $self->mode eq 'depends';

    # Process the file
    $realname = $fn if $realname eq '';
    $self->_main->verbose( $self->level, "|" );
    $self->_main->verbose( $self->level, "+-- $fn" );
    my $in       = path($fn)->openr();
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

        my $op = TheWML::Backends::IPP::Line->new(
            _main    => $self->_main,
            arg      => $arg,
            l        => \$l,
            line_idx => $line_idx,
            out      => \$out,
            realname => $realname,
        )->_process_line( \$store, $self->level, $fn, ) // '';
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
