
require "TEST.pl";
&TEST::init;

print "1..4\n";

#
#   TEST 1-2: throughput
#

$pass = 7;

my $WS = ' ';
my $output_text = <<"EOT_OUT";
<body bgcolor="#a0f43c">
$WS$WS$WS$WS$WS$WS$WS$WS
        <img src="foo.gif" alt="">

EOT_OUT
&TEST::generic($pass, <<'EOT_IN', $output_text, '');
<body bgcolor=a0f43c>
<indent num=2>
<img src=foo.gif>
</indent>
EOT_IN

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
<a href=http://some.where.com/query?var=val&var2=val2>
<a href=http://some.where.com/query?var=val&var2=val2/>
EOT_IN
<a href="http://some.where.com/query?var=val&var2=val2">
<a href="http://some.where.com/query?var=val&var2=val2" />
EOT_OUT

&TEST::cleanup;

