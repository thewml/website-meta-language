
use strict;
use warnings;

use WmlTest ();
WmlTest::init();

use Test::More tests => 2;

#
#   TEST 1-2: throughput
#

my $pass = 8;

# TEST*2
WmlTest::generic( $pass, <<'EOT_IN', <<'EOT_OUT', '' );


foo

<pre>

bar
</pre>
EOT_IN
foo
<pre>

bar
</pre>
EOT_OUT

WmlTest::cleanup();

