%{
##
##  slice_term.y -- YACC parser for slice terms
##  Copyright (c) 1997-2002 Ralf S. Engelschall. 
##  Copyright (c) 1999-2002 Denis Barbier.
##

package SliceTermParser;
%}

%token SLICE

%left '\\' '-' 
%left 'u' '+'
%left 'x' '^' 
%left 'n' '%'

%right '!' '~' 

%%
expr:   SLICE           { $$ = newvar($1); push(@OUT, "my ".$$." = \$CFG->{SLICE}->{SET}->{OBJ}->{'".$1."'}->Clone;"); }

    |   SLICE '@'       { $$ = newvar($1); push(@OUT, "my ".$$." = \$CFG->{SLICE}->{SET}->{OBJ}->{'NOV_".$1."'}->Clone;"); }

    |   '!' expr        { $$ = $2; push(@OUT, $2."->Complement(".$2.");"); }
    |   '~' expr        { $$ = $2; push(@OUT, $2."->Complement(".$2.");"); }

    |   expr 'x' expr   { $$ = $1; push(@OUT, $1."->ExclusiveOr(".$1.",".$3.");"); }
    |   expr '^' expr   { $$ = $1; push(@OUT, $1."->ExclusiveOr(".$1.",".$3.");"); }

    |   expr '\\' expr  { $$ = $1; push(@OUT, $1."->Difference(".$1.",".$3.");"); }
    |   expr '-' expr   { $$ = $1; push(@OUT, $1."->Difference(".$1.",".$3.");"); }

    |   expr 'n' expr   { $$ = $1; push(@OUT, $1."->Intersection(".$1.",".$3.");"); }
    |   expr '%' expr   { $$ = $1; push(@OUT, $1."->Intersection(".$1.",".$3.");"); }

    |   expr 'u' expr   { $$ = $1; push(@OUT, $1."->Union(".$1.",".$3.");"); }
    |   expr '+' expr   { $$ = $1; push(@OUT, $1."->Union(".$1.",".$3.");"); }

    |   '(' expr ')'    { $$ = $2; }
    ;
%%

#   create new set variable
$tmpcnt = 0;
sub newvar {
    my ($name) = @_;
    my ($tmp);

    if ($main::CFG->{SLICE}->{SET}->{OBJ}->{"$name"} eq '') {
        main::printwarning("no such slice '$name'\n") if $undef;
        #    The $undef string is caught by caller, it is used
        #    to trap warnings depending on the -y command line flag.
        die $undef."\n" if $undef > 1;
        $main::CFG->{SLICE}->{SET}->{OBJ}->{"$name"} =
                $main::CFG->{SLICE}->{SET}->{OBJ}->{DEF0}->Clone;
    }
    $tmp = sprintf("\$T%03d", $tmpcnt++);
    return $tmp;
}

#   the lexical scanner
sub yylex {
    local (*s) = @_;
    my ($c, $val);

    #   ignore whitespaces
    $s =~ s|^\s+||;

    #   recognize end of string
    return 0 if ($s eq '');

    #   found a token
    if ($s =~ s|^([_A-Z0-9*{}]+)||) {
        $val = $1;

        #   if its a wildcarded slice name we have
        #   to construct the slice union on-the-fly
        if ($val =~ m|\*|) {
            my $pat = $val;
            $pat =~ s|\*|\.\*|g;

            #   treat special *{...} sequence
            $excl = '';
            while ($pat =~ s|^(.*?)\.\*\{([_A-Z0-9]+)\}(.*)$|$1\.\*$3|) {
                my $temp = $1 . $2 . $3;
                $temp =~ s|\.\*\{[_A-Z0-9]+\}|\.\*|g;
                $excl .= "return 1 if m/^$temp\$/;";
            }
            $sub_excl = eval "sub { \$_ = shift; $excl; return 0}";

            my $slice;
            my @slices = ();
            foreach $slice (keys(%{$main::CFG->{SLICE}->{SET}->{ASC}})) {
                if ($slice =~ m|^$pat$|) {
                    push(@slices, $slice) unless &$sub_excl($slice);
                }
            }
            if ($#slices == 0) {
                $val = $slices[0];
            }
            elsif ($#slices > 0) {
                $s = join('u', @slices).')'.$s;
                return ord('(');
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
    $c = substr($s, 0, 1);
    $s = substr($s, 1);
    return ord($c);
}

#   and error function
sub yyerror {
    my ($msg, $s) = @_;
    die "$msg at $s.\n";
}

#
#  The top-level function which gets called by the user
#
#  ($cmds, $var) = SliceTerm::Parse($term, $status);
#

package SliceTerm;

sub Parse {
    local($str, $status) = @_;
    my($p, $var, $cmds);

    @SliceTermParser::OUT = ();
    $SliceTermParser::undef = $status->{u};
    $SliceTermParser::wildcard = $status->{w};
    $p = SliceTermParser->new(\&SliceTermParser::yylex, \&SliceTermParser::yyerror, 0);
    eval {$var = $p->yyparse(*str);};
    if ($@ =~ s/^(\d)$//) {
        main::error("Execution stopped\n") if $1 > 2;
        return ();
    }
    $cmds = join("\n", @SliceTermParser::OUT) . "\n";

    return ($cmds, $var);
}

package main;

1;
##EOF##
