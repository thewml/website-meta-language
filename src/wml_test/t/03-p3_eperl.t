
use strict;
use warnings;

use WmlTest ();
WmlTest::init();

use Test::More tests => 2;

#
#   TEST 1-2: throughput
#

my $pass = 3;

# TEST*2
WmlTest::generic( $pass, <<'EOT_IN', <<'EOT_OUT', '' );
<:
    print "foo";
:>
EOT_IN
foo
EOT_OUT

WmlTest::cleanup();

