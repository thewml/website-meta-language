
require "TEST.pl";
&TEST::init;

print "1..2\n";

#
#   TEST 1-2: throughput
#

$pass = 6;

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
{:[[s/ä/&auml;/]][[s/ü/&uuml;/]][[tr/[a-z]/[A-Z]/]]
Foo Bar Baz Quux with Umlauts ä and ü
:}
EOT_IN

FOO BAR BAZ QUUX WITH UMLAUTS &AUML; AND &UUML;

EOT_OUT

&TEST::cleanup;

