#!/usr/bin/env perl

use strict;
use warnings;
use autodie;

$ENV{PERL_MM_USE_DEFAULT} = 1;
chdir(qq($ENV{HOME}/website-meta-language));
mkdir('build');
chdir('build');
system( $^X, '../src/wml_test/run_test.pl' ) and die $!;
