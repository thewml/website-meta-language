
require "TEST.pl";
&TEST::init;

print "1..4\n";

#
#   TEST 1-2: throughput
#

$pass = 7;

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
<body bgcolor=a0f43c>
<indent num=2>
<img src=foo.gif>
</indent>
EOT_IN
<body bgcolor="#a0f43c">
        
        <img src="foo.gif" alt="">

EOT_OUT

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
<a href=http://some.where.com/query?var=val&var2=val2>
<a href=http://some.where.com/query?var=val&var2=val2/>
EOT_IN
<a href="http://some.where.com/query?var=val&var2=val2">
<a href="http://some.where.com/query?var=val&var2=val2" />
EOT_OUT

&TEST::cleanup;

