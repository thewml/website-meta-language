##
##  IPP -- Include Pre-Processor
##  Copyright (c) 1997,1998,1999 Ralf S. Engelschall, All Rights Reserved.
##  Copyright (c) 2000 Denis Barbier, All Rights Reserved.
##

package TheWML::Backends::IPP::Line;

use 5.014;

use strict;
use warnings;

use parent 'TheWML::CmdLine::Base';

use Class::XSAccessor (
    constructor => 'new',
    accessors   => +{
        map { $_ => $_ }
            qw(
            _main
            arg
            l
            line_idx
            out
            realname
            )
    },
);

use TheWML::Backends::IPP::Delimit qw/ _delim /;

sub _line_Continuation_Support
{
    my ( $self, $store ) = @_;
    my $l = $self->l;

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
    return 1;
}

sub _line_perform_Substitutions
{
    my ( $self, ) = @_;
    my $l   = $self->l;
    my $arg = $self->arg;

    #       Substitutions are performed from left to right and from
    #       inner to outer, all operators have same precedence.
    if ( $$l !~ m/(?:(?!\\).|^)\$\(([a-zA-Z0-9_]+)(?:(=|:[-=?+*])([^()]*))?\)/ )
    {
        return;
    }
    my ( $name, $op, $str ) = ( $1, $2, $3 );
    if ( not defined($op) )
    {
        #   Normal Value
        $$l =~
s/((?!\\).|^)\$\($name\)/exists $arg->{$name} ? $1.$arg->{$name} : $1/e;
        return 'redo';
    }

    my $subst = qr#(?:(?!\\).|^)\K\$\(\Q$name$op\E(?:[^()]*)\)#;

    my $del = sub {
        if ( $str eq '' )
        {
            delete $arg->{$name} if exists $arg->{$name};
        }
        else
        {
            $arg->{$name} = $str;
        }
    };

    if ( $op eq '=' )
    {
        #   Assign
        $$l =~ s/$subst//;
        $del->();
    }
    elsif ( $op eq ':?' )
    {
        #   Indicate Error if Unset
        $$l =~ s/$subst/exists $arg->{$name} ? $arg->{$name} : error($str)/e;
    }
    elsif ( $op eq ':-' )
    {
        #   Use Default Values
        $$l =~ s/$subst/exists $arg->{$name} ? $arg->{$name} : $str/e;
    }
    elsif ( $op eq ':=' )
    {
        #   Use Default Values And Assign
        $$l =~ s/$subst/exists $arg->{$name} ? $arg->{$name} : $str/e;
        $del->();
    }
    elsif ( $op eq ':+' )
    {
        #   Use Alternative Value
        $$l =~ s/$subst/exists $arg->{$name} ? $str : ''/e;
    }
    elsif ( $op eq ':*' )
    {
        #   Use Negative Alternative Value
        $$l =~ s/$subst/exists $arg->{$name} ? '' : $str/e;
    }
    else
    {
        #   There is an error in these statements
        die "Internal error when expanding variables";
    }
    return 'redo';
}

sub _process_line
{
    my ( $self, $store, $level, $fn, ) = @_;
    my $l   = $self->l;
    my $arg = $self->arg;

    #   EOL-comments
    return if $$l =~ m/^\s*#(?!use|include|depends)/;

    return if !$self->_line_Continuation_Support($store);

    # Variable Interpolation
    if ( my $ret = $self->_line_perform_Substitutions )
    {
        return $ret;
    }

    #   EOL-comments again
    return if $$l =~ m/^\s*#(?!use|include|depends)/;

    #   Implicit Variables
    $$l =~ s|__LINE__|$self->line_idx|eg;
    $arg->{'IPP_SRC_REALNAME'} //= '';
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

    if ( my $ret = $self->_line_do_includes( $level, ) )
    {
        return $ret;
    }

    return;
}

sub _line_do_includes
{
    my ( $self, $level, ) = @_;
    my $l   = $self->l;
    my $arg = $self->arg;

    #   ``#include'', ``#use'' and ``#depends'' directives
    if ( my ( $cmd, $incfile, $args ) =
        ( $$l =~ m/^#(use|include|depends)\s+(\S+)(.*)$/ ) )
    {
        #   set arguments
        my %argO = %$arg;
        TheWML::Backends::IPP::Args->new->setargs( $arg, $args );

        #   do possible argument mapping
        $incfile = $self->_main->_map->mapfile($incfile);

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
        ${ $self->out } .=
            $self->_main->_process_file( $cmd, _delim($type),
            $incfile, "", $level + 1, 0, $arg );
        if ( not $self->_main->opt_N and not $arg->{'IPP_NOSYNCLINES'} )
        {
            ${ $self->out } .=
"<__file__ @{[$self->realname]} /><__line__ @{[$self->line_idx]} />"
                . "<protect pass=2><:# self @{[$self->line_idx]} \"@{[$self->realname]}\":></protect>\n";
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
        ${ $self->out } .= $$l;
    }
    return;
}

1;
