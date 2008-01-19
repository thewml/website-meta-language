
require "TEST.pl";
&TEST::init;

print "1..2\n";

#
#   TEST 1-2: throughput
#

$pass = 3;

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
<:
    print "foo";
:>
EOT_IN
foo
EOT_OUT

&TEST::cleanup;

