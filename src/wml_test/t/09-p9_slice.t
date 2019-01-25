
use strict;
use warnings;

use WmlTest;
WmlTest::init();

use Test::More tests => 2;

#
#   TEST 1-2: throughput
#

my $pass = 9;

my $tmpfile1 = WmlTest::tmpfile(<<'EOT_IN');
%!slice -o(ALL-LANG_*)+LANG_EN:b
[LANG_EN:en:]
[LANG_DE:de:]
[LANG_EN:[LANG_EN:en:][LANG_DE:de:]:]
[LANG_DE:[LANG_EN:en:][LANG_DE:de:]:]
EOT_IN

my $tmpfile2 = WmlTest::tmpfile(<<'EOT_IN');
en

ende
en
EOT_IN

# TEST
ok( !system("$ENV{WML} -p$pass $tmpfile1 >a"), "wml" );

# TEST
ok( !system("cmp $tmpfile2 b"), "cmp" );

WmlTest::add_files("b");
WmlTest::add_files("a");

WmlTest::cleanup();

