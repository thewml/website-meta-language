
use strict;
use warnings;

use WmlTest ();
WmlTest::init();

use Test::More tests => 4;

#
#   TEST 1-2: throughput
#

my $pass = 4;

# TEST*2
WmlTest::generic( $pass, <<'EOT_IN', <<'EOT_OUT', '' );
m4_define(`foo',`bar')m4_dnl
foo
EOT_IN
bar
EOT_OUT

# TEST*2
WmlTest::generic( "1,4", <<'EOT_IN', <<'EOT_OUT', '' );
m4_quotes`'m4_dnl
m4_define(`foo',`bar')m4_dnl
foo
m4_noquotes`'
EOT_IN
bar
`'
EOT_OUT

WmlTest::cleanup();

