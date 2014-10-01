#!/usr/bin/perl

use strict;
use warnings;
my $sq = chr(39); my $at = chr(64); my $bs = chr(92);
s/$sq$at$sq/$sq$bs$at$sq/g;
s/ +$//ms;
