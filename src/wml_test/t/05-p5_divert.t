
use strict;
use warnings;

use WmlTest ();
WmlTest::init();

use Test::More tests => 2;

#
#   TEST 1-2: throughput
#

my $pass = 5;

# TEST*2
WmlTest::generic( $pass, <<'EOT_IN', <<'EOT_OUT', '' );
FOO
<<BAR>>
QUUX
..BAR>>
BAZ
<<..
EOT_IN
FOO

BAZ

QUUX

EOT_OUT

WmlTest::cleanup();

