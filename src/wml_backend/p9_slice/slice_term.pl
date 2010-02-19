# @(#)yaccpar 1.8 (Berkeley) 01/20/91 (JAKE-P5BP-0.6 04/26/98)
package SliceTermParser;
;##
;##  slice_term.y -- YACC parser for slice terms
;##  Copyright (c) 1997-2002 Ralf S. Engelschall. 
;##  Copyright (c) 1999-2002 Denis Barbier.
;##

package SliceTermParser;
$SLICE=257;
$YYERRCODE=256;
@yylhs = (                                               -1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,
);
@yylen = (                                                2,
    1,    2,    2,    2,    3,    3,    3,    3,    3,    3,
    3,    3,    3,
);
@yydefred = (                                             0,
    0,    0,    0,    0,    0,    2,    3,    4,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   13,    0,    0,
    0,    0,    0,    0,    9,   10,
);
@yydgoto = (                                              5,
);
@yysindex = (                                           -33,
  -60,  -33,  -33,  -33,   -7,    0,    0,    0,  -12,  -33,
  -33,  -33,  -33,  -33,  -33,  -33,  -33,    0,  -31,  -31,
   24,   24,  -36,  -36,    0,    0,
);
@yyrindex = (                                             0,
    2,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,   15,   17,
    9,   14,    3,    8,    0,    0,
);
@yygindex = (                                           135,
);
$YYTABLESIZE=224;
@yytable = (                                              2,
   17,    1,    5,    6,    0,   17,    4,    6,   11,    0,
    0,   13,    0,   12,    7,    0,    8,    0,    0,    0,
    0,    0,    0,    0,   17,    0,    0,    0,   18,   17,
   13,    0,   11,    0,    0,   13,    0,   11,    1,    0,
    0,    0,    1,    5,    1,    5,    1,    5,    6,   11,
    6,   11,    6,   11,   12,    7,   12,    8,   12,    7,
   17,    8,   15,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   16,    0,    0,    0,    0,   16,   10,
    0,   15,    0,    0,   10,   12,   15,    0,   14,    0,
    0,    0,    3,    1,    5,    1,    5,   16,    0,    6,
   11,    6,   16,    0,   12,   12,    7,   14,    8,   12,
    0,    1,   14,    0,    0,    0,    0,   15,    1,    5,
    0,    1,    5,    0,    6,   11,    0,    6,    0,    0,
   12,    0,    0,   16,    0,    0,    7,    8,    9,    0,
    0,    0,    0,   14,   19,   20,   21,   22,   23,   24,
   25,   26,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    1,
);
@yycheck = (                                             33,
   37,    0,    0,   64,   -1,   37,   40,    0,    0,   -1,
   -1,   43,   -1,    0,    0,   -1,    0,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   37,   -1,   -1,   -1,   41,   37,
   43,   -1,   45,   -1,   -1,   43,   -1,   45,   37,   -1,
   -1,   -1,   41,   41,   43,   43,   45,   45,   41,   41,
   43,   43,   45,   45,   41,   41,   43,   41,   45,   45,
   37,   45,   94,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  110,   -1,   -1,   -1,   -1,  110,   92,
   -1,   94,   -1,   -1,   92,  117,   94,   -1,  120,   -1,
   -1,   -1,  126,   92,   92,   94,   94,  110,   -1,   92,
   92,   94,  110,   -1,  117,   92,   92,  120,   92,  117,
   -1,  110,  120,   -1,   -1,   -1,   -1,   94,  117,  117,
   -1,  120,  120,   -1,  117,  117,   -1,  120,   -1,   -1,
  117,   -1,   -1,  110,   -1,   -1,    2,    3,    4,   -1,
   -1,   -1,   -1,  120,   10,   11,   12,   13,   14,   15,
   16,   17,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  257,
);
$YYFINAL=5;
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
$YYMAXTOKEN=257;
#if YYDEBUG
@yyname = (
"end-of-file",'','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','',
"'!'",'','','',"'%'",'','',"'('","')'",'',"'+'",'',"'-'",'','','','','','','','','','','','','','','',
'','','',"'\@'",'','','','','','','','','','','','','','','','','','','','','','','','','','','',"'\\\\'",'',
"'^'",'','','','','','','','','','','','','','','',"'n'",'','','','','','',"'u'",'','',"'x'",'','','','','',
"'~'",'','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','',
'','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','',
'','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','',
'','','','','','','','','','','','','',"SLICE",
);
@yyrule = (
"\$accept : expr",
"expr : SLICE",
"expr : SLICE '\@'",
"expr : '!' expr",
"expr : '~' expr",
"expr : expr 'x' expr",
"expr : expr '^' expr",
"expr : expr '\\\\' expr",
"expr : expr '-' expr",
"expr : expr 'n' expr",
"expr : expr '%' expr",
"expr : expr 'u' expr",
"expr : expr '+' expr",
"expr : '(' expr ')'",
);
#endif
sub yyclearin {
  my  $p;
  ($p) = @_;
  $p->{yychar} = -1;
}
sub yyerrok {
  my  $p;
  ($p) = @_;
  $p->{yyerrflag} = 0;
}
sub new {
  my $p = bless {}, $_[0];
  $p->{yylex} = $_[1];
  $p->{yyerror} = $_[2];
  $p->{yydebug} = $_[3];
  return $p;
}
sub YYERROR {
  my  $p;
  ($p) = @_;
  ++$p->{yynerrs};
  $p->yy_err_recover;
}
sub yy_err_recover {
  my  $p;
  ($p) = @_;
  if ($p->{yyerrflag} < 3)
  {
    $p->{yyerrflag} = 3;
    while (1)
    {
      if (($p->{yyn} = $yysindex[$p->{yyss}->[$p->{yyssp}]]) && 
          ($p->{yyn} += $YYERRCODE) >= 0 && 
          $p->{yyn} <= $#yycheck &&
          $yycheck[$p->{yyn}] == $YYERRCODE)
      {
        warn("yydebug: state " . 
                     $p->{yyss}->[$p->{yyssp}] . 
                     ", error recovery shifting to state" . 
                     $yytable[$p->{yyn}] . "\n") 
                       if $p->{yydebug};
        $p->{yyss}->[++$p->{yyssp}] = 
          $p->{yystate} = $yytable[$p->{yyn}];
        $p->{yyvs}->[++$p->{yyvsp}] = $p->{yylval};
        next yyloop;
      }
      else
      {
        warn("yydebug: error recovery discarding state ".
              $p->{yyss}->[$p->{yyssp}]. "\n") 
                if $p->{yydebug};
        return(undef) if $p->{yyssp} <= 0;
        --$p->{yyssp};
        --$p->{yyvsp};
      }
    }
  }
  else
  {
    return (undef) if $p->{yychar} == 0;
    if ($p->{yydebug})
    {
      $p->{yys} = '';
      if ($p->{yychar} <= $YYMAXTOKEN) { $p->{yys} = 
        $yyname[$p->{yychar}]; }
      if (!$p->{yys}) { $p->{yys} = 'illegal-symbol'; }
      warn("yydebug: state " . $p->{yystate} . 
                   ", error recovery discards " . 
                   "token " . $p->{yychar} . "(" . 
                   $p->{yys} . ")\n");
    }
    $p->{yychar} = -1;
    next yyloop;
  }
0;
} # yy_err_recover

sub yyparse {
  my  $p;
  my $s;
  ($p, $s) = @_;
  if ($p->{yys} = $ENV{'YYDEBUG'})
  {
    $p->{yydebug} = int($1) if $p->{yys} =~ /^(\d)/;
  }

  $p->{yynerrs} = 0;
  $p->{yyerrflag} = 0;
  $p->{yychar} = (-1);

  $p->{yyssp} = 0;
  $p->{yyvsp} = 0;
  $p->{yyss}->[$p->{yyssp}] = $p->{yystate} = 0;

yyloop: while(1)
  {
    yyreduce: {
      last yyreduce if ($p->{yyn} = $yydefred[$p->{yystate}]);
      if ($p->{yychar} < 0)
      {
        if ((($p->{yychar}, $p->{yylval}) = 
            &{$p->{yylex}}($s)) < 0) { $p->{yychar} = 0; }
        if ($p->{yydebug})
        {
          $p->{yys} = '';
          if ($p->{yychar} <= $#yyname) 
             { $p->{yys} = $yyname[$p->{yychar}]; }
          if (!$p->{yys}) { $p->{yys} = 'illegal-symbol'; };
          warn("yydebug: state " . $p->{yystate} . 
                       ", reading " . $p->{yychar} . " (" . 
                       $p->{yys} . ")\n");
        }
      }
      if (($p->{yyn} = $yysindex[$p->{yystate}]) && 
          ($p->{yyn} += $p->{yychar}) >= 0 && 
          $p->{yyn} <= $#yycheck &&
          $yycheck[$p->{yyn}] == $p->{yychar})
      {
        warn("yydebug: state " . $p->{yystate} . 
                     ", shifting to state " .
              $yytable[$p->{yyn}] . "\n") if $p->{yydebug};
        $p->{yyss}->[++$p->{yyssp}] = $p->{yystate} = 
          $yytable[$p->{yyn}];
        $p->{yyvs}->[++$p->{yyvsp}] = $p->{yylval};
        $p->{yychar} = (-1);
        --$p->{yyerrflag} if $p->{yyerrflag} > 0;
        next yyloop;
      }
      if (($p->{yyn} = $yyrindex[$p->{yystate}]) && 
          ($p->{yyn} += $p->{'yychar'}) >= 0 &&
          $p->{yyn} <= $#yycheck &&
          $yycheck[$p->{yyn}] == $p->{yychar})
      {
        $p->{yyn} = $yytable[$p->{yyn}];
        last yyreduce;
      }
      if (! $p->{yyerrflag}) {
        &{$p->{yyerror}}('syntax error', $s);
        ++$p->{yynerrs};
      }
      return(undef) if $p->yy_err_recover;
    } # yyreduce
    warn("yydebug: state " . $p->{yystate} . 
                 ", reducing by rule " . 
                 $p->{yyn} . " (" . $yyrule[$p->{yyn}] . 
                 ")\n") if $p->{yydebug};
    $p->{yym} = $yylen[$p->{yyn}];
    $p->{yyval} = $p->{yyvs}->[$p->{yyvsp}+1-$p->{yym}];
if ($p->{yyn} == 1) {
{ $p->{yyval} = newvar($p->{yyvs}->[$p->{yyvsp}-0]); push(@OUT, "my ".$p->{yyval}." = \$CFG->{SLICE}->{SET}->{OBJ}->{'".$p->{yyvs}->[$p->{yyvsp}-0]."'}->Clone;"); }
}
if ($p->{yyn} == 2) {
{ $p->{yyval} = newvar($p->{yyvs}->[$p->{yyvsp}-1]); push(@OUT, "my ".$p->{yyval}." = \$CFG->{SLICE}->{SET}->{OBJ}->{'NOV_".$p->{yyvs}->[$p->{yyvsp}-1]."'}->Clone;"); }
}
if ($p->{yyn} == 3) {
{ $p->{yyval} = $p->{yyvs}->[$p->{yyvsp}-0]; push(@OUT, $p->{yyvs}->[$p->{yyvsp}-0]."->Complement(".$p->{yyvs}->[$p->{yyvsp}-0].");"); }
}
if ($p->{yyn} == 4) {
{ $p->{yyval} = $p->{yyvs}->[$p->{yyvsp}-0]; push(@OUT, $p->{yyvs}->[$p->{yyvsp}-0]."->Complement(".$p->{yyvs}->[$p->{yyvsp}-0].");"); }
}
if ($p->{yyn} == 5) {
{ $p->{yyval} = $p->{yyvs}->[$p->{yyvsp}-2]; push(@OUT, $p->{yyvs}->[$p->{yyvsp}-2]."->ExclusiveOr(".$p->{yyvs}->[$p->{yyvsp}-2].",".$p->{yyvs}->[$p->{yyvsp}-0].");"); }
}
if ($p->{yyn} == 6) {
{ $p->{yyval} = $p->{yyvs}->[$p->{yyvsp}-2]; push(@OUT, $p->{yyvs}->[$p->{yyvsp}-2]."->ExclusiveOr(".$p->{yyvs}->[$p->{yyvsp}-2].",".$p->{yyvs}->[$p->{yyvsp}-0].");"); }
}
if ($p->{yyn} == 7) {
{ $p->{yyval} = $p->{yyvs}->[$p->{yyvsp}-2]; push(@OUT, $p->{yyvs}->[$p->{yyvsp}-2]."->Difference(".$p->{yyvs}->[$p->{yyvsp}-2].",".$p->{yyvs}->[$p->{yyvsp}-0].");"); }
}
if ($p->{yyn} == 8) {
{ $p->{yyval} = $p->{yyvs}->[$p->{yyvsp}-2]; push(@OUT, $p->{yyvs}->[$p->{yyvsp}-2]."->Difference(".$p->{yyvs}->[$p->{yyvsp}-2].",".$p->{yyvs}->[$p->{yyvsp}-0].");"); }
}
if ($p->{yyn} == 9) {
{ $p->{yyval} = $p->{yyvs}->[$p->{yyvsp}-2]; push(@OUT, $p->{yyvs}->[$p->{yyvsp}-2]."->Intersection(".$p->{yyvs}->[$p->{yyvsp}-2].",".$p->{yyvs}->[$p->{yyvsp}-0].");"); }
}
if ($p->{yyn} == 10) {
{ $p->{yyval} = $p->{yyvs}->[$p->{yyvsp}-2]; push(@OUT, $p->{yyvs}->[$p->{yyvsp}-2]."->Intersection(".$p->{yyvs}->[$p->{yyvsp}-2].",".$p->{yyvs}->[$p->{yyvsp}-0].");"); }
}
if ($p->{yyn} == 11) {
{ $p->{yyval} = $p->{yyvs}->[$p->{yyvsp}-2]; push(@OUT, $p->{yyvs}->[$p->{yyvsp}-2]."->Union(".$p->{yyvs}->[$p->{yyvsp}-2].",".$p->{yyvs}->[$p->{yyvsp}-0].");"); }
}
if ($p->{yyn} == 12) {
{ $p->{yyval} = $p->{yyvs}->[$p->{yyvsp}-2]; push(@OUT, $p->{yyvs}->[$p->{yyvsp}-2]."->Union(".$p->{yyvs}->[$p->{yyvsp}-2].",".$p->{yyvs}->[$p->{yyvsp}-0].");"); }
}
if ($p->{yyn} == 13) {
{ $p->{yyval} = $p->{yyvs}->[$p->{yyvsp}-1]; }
}
    $p->{yyssp} -= $p->{yym};
    $p->{yystate} = $p->{yyss}->[$p->{yyssp}];
    $p->{yyvsp} -= $p->{yym};
    $p->{yym} = $yylhs[$p->{yyn}];
    if ($p->{yystate} == 0 && $p->{yym} == 0)
    {
      warn("yydebug: after reduction, shifting from state 0 ",
            "to state $YYFINAL\n") if $p->{yydebug};
      $p->{yystate} = $YYFINAL;
      $p->{yyss}->[++$p->{yyssp}] = $YYFINAL;
      $p->{yyvs}->[++$p->{yyvsp}] = $p->{yyval};
      if ($p->{yychar} < 0)
      {
        if ((($p->{yychar}, $p->{yylval}) = 
            &{$p->{yylex}}($s)) < 0) { $p->{yychar} = 0; }
        if ($p->{yydebug})
        {
          $p->{yys} = '';
          if ($p->{yychar} <= $#yyname) 
            { $p->{yys} = $yyname[$p->{yychar}]; }
          if (!$p->{yys}) { $p->{yys} = 'illegal-symbol'; }
          warn("yydebug: state $YYFINAL, reading " . 
               $p->{yychar} . " (" . $p->{yys} . ")\n");
        }
      }
      return ($p->{yyvs}->[1]) if $p->{yychar} == 0;
      next yyloop;
    }
    if (($p->{yyn} = $yygindex[$p->{yym}]) && 
        ($p->{yyn} += $p->{yystate}) >= 0 && 
        $p->{yyn} <= $#yycheck && 
        $yycheck[$p->{yyn}] == $p->{yystate})
    {
        $p->{yystate} = $yytable[$p->{yyn}];
    } else {
        $p->{yystate} = $yydgoto[$p->{yym}];
    }
    warn("yydebug: after reduction, shifting from state " . 
        $p->{yyss}->[$p->{yyssp}] . " to state " . 
        $p->{yystate} . "\n") if $p->{yydebug};
    $p->{yyss}[++$p->{yyssp}] = $p->{yystate};
    $p->{yyvs}[++$p->{yyvsp}] = $p->{yyval};
  } # yyloop
} # yyparse

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
1;
