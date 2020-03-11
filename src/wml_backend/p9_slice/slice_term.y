%{
##
##  slice_term.y -- YACC parser for slice terms
##  Copyright (c) 1997-2002 Ralf S. Engelschall.
##  Copyright (c) 1999-2002 Denis Barbier.
##

package SliceTermParser;

{
    use strict;
    use warnings;
    my $YYTABLESIZE;
    my $YYMAXTOKEN;
    my $YYERRCODE;
    my (@yylhs, @yylen, @yyname, $YYFINAL, @yygindex, @yycheck, @yytable, @yyrule);
    my (@yydgoto, @yyrindex, @yysindex, @yydefred);
    my $SLICE ;
%}

%token SLICE

%left '\\' '-'
%left 'u' '+'
%left 'x' '^'
%left 'n' '%'

%right '!' '~'

%%
expr:   SLICE           { $$ = newvar($p, $s->[0], $1); my $t = $$; my $src = $1; $p->_out(sub { my ($self, $CFG) = @_; $self->_set_var($t , $CFG->{SLICE}->{SET}->{OBJ}->{$src}->Clone); return;}); }
    |  SLICE '@'          { $$ = newvar($p, $s->[0], $1); my $t = $$; my $src = $1; $p->_out(sub { my ($self, $CFG) = @_; $self->_set_var($t , $CFG->{SLICE}->{SET}->{OBJ}->{'NOV_'.$src}->Clone); return;}); }
    |   '!' expr        { $$ = $2; $p->_push_complement($2); }
    |   '~' expr        { $$ = $2; $p->_push_complement($2); }
    |   expr 'x' expr   { $$ = $1; $p->_push_mutate($1, 'ExclusiveOr', $3); }
    |   expr '^' expr   { $$ = $1; $p->_push_mutate($1, 'ExclusiveOr', $3); }
    |   expr '\\' expr  { $$ = $1; $p->_push_mutate($1, 'Difference', $3); }
    |   expr '-' expr   { $$ = $1; $p->_push_mutate($1, 'Difference', $3); }
    |   expr 'n' expr   { $$ = $1; $p->_push_mutate($1, 'Intersection', $3); }
    |   expr '%' expr   { $$ = $1; $p->_push_mutate($1, 'Intersection', $3); }
    |   expr 'u' expr   { $$ = $1; $p->_push_mutate($1, 'Union', $3); }
    |   expr '+' expr   { $$ = $1; $p->_push_mutate($1, 'Union', $3); }
    |   '(' expr ')'    { $$ = $2; }
    ;
%%

#   create new set variable
my $tmpcnt = 0;
sub newvar {
local $SIG{__WARN__}=sub{print STDERR @_;exit(-1);};
# use warnings FATAL => 'all';
# die "oooookkkk";
    my ($p, $CFG, $name) = @_;
    my ($tmp);

    if (( $CFG->{SLICE}->{SET}->{OBJ}->{"$name"} // '') eq '') {
        main::printwarning("no such slice '$name'\n") if $p->{_undef};
        #    The $p->{_undef} string is caught by caller, it is used
        #    to trap warnings depending on the -y command line flag.
        die $p->{_undef}."\n" if $p->{_undef} > 1;
        $CFG->{SLICE}->{SET}->{OBJ}->{"$name"} =
                $CFG->{SLICE}->{SET}->{OBJ}->{DEF0}->Clone;
    }
    $tmp = sprintf("\$T%03d", $tmpcnt++);
    return $tmp;
}

#   the lexical scanner
sub yylex {
    my ($ctx) = @_;
    my ($c, $val);

    my ($CFG, $s, $wildcard) = @$ctx;
    #   ignore whitespaces
    $$s =~ s|^\s+||;

    #   recognize end of string
    return (0, 0) if ($$s eq '');

    #   found a token
    if ($$s =~ s|^([_A-Z0-9*{}]+)||) {
        $val = $1;

        #   if its a wildcarded slice name we have
        #   to construct the slice union on-the-fly
        if ($val =~ m|\*|) {
            my $pat = $val;
            $pat =~ s|\*|\.\*|g;

            #   treat special *{...} sequence
            my $excl = '';
            while ($pat =~ s|^(.*?)\.\*\{([_A-Z0-9]+)\}(.*)$|$1\.\*$3|) {
                my $temp = $1 . $2 . $3;
                $temp =~ s|\.\*\{[_A-Z0-9]+\}|\.\*|g;
                $excl .= "return 1 if m/^$temp\$/;";
            }
            my $sub_excl = eval "sub { \$_ = shift; $excl; return 0}";

            my $slice;
            my @slices = ();
            foreach $slice (keys(%{$CFG->{SLICE}->{SET}->{ASC}})) {
                if ($slice =~ m|^$pat$|) {
                    push(@slices, $slice) unless &$sub_excl($slice);
                }
            }
            if (@slices == 1) {
                $val = $slices[0];
            }
            elsif (@slices > 1) {
                $$s = join('u', @slices).')'.$$s;
                return( ord('('), 0);
            }
            else {
                main::printwarning("no existing slice matches `$val'\n") if $wildcard;
                #    The $wildcard string is caught by caller, it is used
                #    to trap warnings depending on the -y command line flag.
                die $wildcard."\n" if $wildcard > 1;
            }
        }
        return ($SLICE, $val);
    }

    #   else give back one plain character
    $c = substr($$s, 0, 1);
    $$s = substr($$s, 1);
    return (ord($c), 0);
}

#   and error function
sub yyerror {
    my ($msg, $s) = @_;
    die "$msg at $$s.\n";
}

sub _out {
    my ($p, $item) = @_;
    push(@{$p->{_OUT}}, $item);
    return;
}
sub _push_complement {
    my ($p, $v) = @_;
    $p->_out(sub{my ($self)=@_;return $self->_complement_var($v);});
    return;
}
sub _push_mutate {
    my ($p, @args) = @_;
    $p->_out(sub{my ($self)=@_;return $self->_mutate_var(@args);});
    return;
}

#
#  The top-level function which gets called by the user
#
#  ($cmds, $var) = SliceTerm::Parse($term, $status);
#

package SliceTerm;

sub Parse {
    my ($CFG, $str, $status) = @_;

    my $p = SliceTermParser->new(\&SliceTermParser::yylex, \&SliceTermParser::yyerror, 0);
    $p->{_OUT} = [];
    $p->{_undef} = $status->{u};
    # $p->yyclearin;
    my $var = eval {$p->yyparse([$CFG, \$str, $status->{w}]);};
    if ($@ =~ s/^(\d)$//) {
        main::error("Execution stopped\n") if $1 > 2;
        return ();
    }
    return ($p->{_OUT}, $var);
}

}

package main;

1;
##EOF##
