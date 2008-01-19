
use Term::Cap;

require "TEST.pl";
&TEST::init;

eval "\$term = Tgetent Term::Cap { TERM => undef, OSPEED => 9600 }";
if ($@) {
    $bold = '';
    $norm = '';
}
else {
    $bold = $term->Tputs('md', 1, undef);
    $norm = $term->Tputs('me', 1, undef);
}

print "1..6\n";

#
#   TEST 1-2: throughput
#

$wmk = $ENV{WML};
$wmk =~ s/l /k /;
$wmk =~ s/l$/k/;

&TEST::tmpfile_with_name('a.html', "x");
&TEST::tmpfile_with_name('a', <<'EOT_IN');
foo
EOT_IN
&TEST::tmpfile_with_name('a.wml', <<"EOT_IN");
#include 'a'
bar
EOT_IN
$tmpfile1 = &TEST::tmpfile(<<'EOT_IN');
foo
bar
EOT_IN

$rc = `$wmk a.wml 2>&1`;
print ($rc eq "$ENV{WML} -n -q -W \"1,-N\" -o a.html a.wml\n" ? "ok\n" : "not ok\n");
$rc = &TEST::system("cmp $tmpfile1 a.html");
print ($rc == 0 ? "ok\n" : "not ok\n");

$rc = `$wmk a.wml 2>&1`;
print ($rc eq "$ENV{WML} -n -q -W \"1,-N\" -o a.html a.wml  (${bold}skipped${norm})\n" ? "ok\n" : "not ok\n");

open(OUT, ">>a");
print OUT " ";
close(OUT);
$rc = `$wmk a.wml 2>&1`;
print ($rc eq "$ENV{WML} -n -q -W \"1,-N\" -o a.html a.wml\n" ? "ok\n" : "not ok\n");

$tmpfile1 = &TEST::tmpfile(<<'EOT_IN');
Hello
EOT_IN
$tmpfile2 = &TEST::tmpfile(<<'EOT_IN');
Willkommen
EOT_IN
&TEST::tmpfile_with_name('a.wml', <<'EOT_IN');
#!wml -o (ALL-LANG_*)+LANG_EN:%BASE.en.html \
      -o (ALL-LANG_*)+LANG_DE:%BASE.de.html
#use wml::std::lang
<lang:new id=en short>
<lang:new id=de short>
<en>Hello</en><de: Willkommen>
EOT_IN

$rc = `$wmk a.wml 2>&1`;
print ($rc eq "$ENV{WML} -n -q -W \"1,-N\" -o '(ALL-LANG_*)+LANG_EN:a.en.html' -o '(ALL-LANG_*)+LANG_DE:a.de.html' a.wml\n" ? "ok\n" : "not ok\n");
$rc = &TEST::system("cmp $tmpfile1 a.en.html") &&
      &TEST::system("cmp $tmpfile2 a.de.html");
print ($rc == 0 ? "ok\n" : "not ok\n");

push(@TEST::TMPFILES, "a.en.html");
push(@TEST::TMPFILES, "a.de.html");
&TEST::cleanup;

