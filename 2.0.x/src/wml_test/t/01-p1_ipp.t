
require "TEST.pl";
&TEST::init;

print "1..4\n";

#
#   TEST 1-4: throughput
#
$tmpfile1 = &TEST::tmpfile_with_name('a', <<'EOT');
a1
#include 'b'
a2
EOT
&TEST::tmpfile_with_name('b', <<'EOT');
b1
#include 'c'
b2
EOT
&TEST::tmpfile_with_name('c', <<'EOT');
c
EOT
$tmpfile2 = &TEST::tmpfile(<<'EOT');
a1
b1
c
b2
a2
EOT
$tmpfile3 = &TEST::tmpfile;
$rc = &TEST::system("$ENV{WML} -p1 $tmpfile1 >$tmpfile3");
print ($rc == 0 ? "ok\n" : "not ok\n");
$rc = &TEST::system("cmp $tmpfile2 $tmpfile3");
print ($rc == 0 ? "ok\n" : "not ok\n");

$tmpfile1 = &TEST::tmpfile(<<'EOT');
#include "b" bar=1
foo=$(foo) \
baz=$(baz)
# comment
EOT
&TEST::tmpfile_with_name('b',<<'EOT');
bar=$(bar)
__END__
skipped
EOT
$tmpfile2 = &TEST::tmpfile(<<'EOT');
bar=1
foo=1 baz=
EOT
$rc = &TEST::system("$ENV{WML} -p1 -Dfoo=1 $tmpfile1 >$tmpfile3");
print ($rc == 0 ? "ok\n" : "not ok\n");
$rc = &TEST::system("cmp $tmpfile2 $tmpfile3");
print ($rc == 0 ? "ok\n" : "not ok\n");

&TEST::cleanup;

