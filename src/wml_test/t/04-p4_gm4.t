
require "TEST.pl";
&TEST::init;

print "1..4\n";

#
#   TEST 1-2: throughput
#

$pass = 4;

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
m4_define(`foo',`bar')m4_dnl
foo
EOT_IN
bar
EOT_OUT

&TEST::generic("1,4", <<'EOT_IN', <<'EOT_OUT', '');
m4_quotes`'m4_dnl
m4_define(`foo',`bar')m4_dnl
foo
m4_noquotes`'
EOT_IN
bar
`'
EOT_OUT

&TEST::cleanup;

