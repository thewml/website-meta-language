
require "TEST.pl";
&TEST::init;

print "1..4\n";

#
#   TEST 1-2: throughput
#

$pass = "1-9";

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
foo bar baz quux
öäüÖÄÜß
!"§$%&/()=?`'*+
EOT_IN
foo bar baz quux
öäüÖÄÜß
!"§$%&/()=?`'*+
EOT_OUT

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '-Dbar -Dvoid=\"\" -Dvoid2=');
<protect pass=2-9>\
$(bar)
$(void)
$(void2)
$(ROOT)\
</protect>
EOT_IN
1


.
EOT_OUT

&TEST::cleanup;

