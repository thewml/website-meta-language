
require "TEST.pl";
&TEST::init;

print "1..2\n";

#
#   TEST 1-2: throughput
#

$pass = 5;

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
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

&TEST::cleanup;

