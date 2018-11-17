#!/usr/bin/env perl

use strict;
use warnings;
use autodie;

chdir(qq($ENV{HOME}/website-meta-language));
mkdir('build');
chdir('build');
system( $^X, '../src/wml_test/cyg_test.pl' ) and die $!;
