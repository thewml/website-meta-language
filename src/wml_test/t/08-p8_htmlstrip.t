
require "TEST.pl";
&TEST::init;

print "1..2\n";

#
#   TEST 1-2: throughput
#

$pass = 8;

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');


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

&TEST::cleanup;

