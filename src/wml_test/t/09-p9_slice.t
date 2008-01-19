
require "TEST.pl";
&TEST::init;

print "1..2\n";

#
#   TEST 1-2: throughput
#

$pass = 9;

$tmpfile1 = &TEST::tmpfile(<<'EOT_IN');
%!slice -o(ALL-LANG_*)+LANG_EN:b
[LANG_EN:en:]
[LANG_DE:de:]
[LANG_EN:[LANG_EN:en:][LANG_DE:de:]:]
[LANG_DE:[LANG_EN:en:][LANG_DE:de:]:]
EOT_IN

$tmpfile2 = &TEST::tmpfile(<<'EOT_IN');
en

ende
en
EOT_IN

$rc = &TEST::system("$ENV{WML} -p$pass $tmpfile1 >a");
print ($rc == 0 ? "ok\n" : "not ok\n");
$rc = &TEST::system("cmp $tmpfile2 b");
print ($rc == 0 ? "ok\n" : "not ok\n");

push(@TEST::TMPFILES, "b");
push(@TEST::TMPFILES, "a");
&TEST::cleanup;

