
require "TEST.pl";
&TEST::init;

print "1..2\n";

&TEST::tmpfile_with_name('a.wml', <<'EOT_IN');
#use wml::std::lang
<lang:new id=en short>
<lang:new id=de short>
<lang:star:slice:>
<a href="<lang:star: $(WML_SRC_BASENAME).*.html>">Link</a>
EOT_IN

$tmpfile1 = &TEST::tmpfile(<<'EOT_IN');
<a href="a.en.html">Link</a>
EOT_IN

$tmpfile2 = &TEST::tmpfile(<<'EOT_IN');
<a href="a.de.html">Link</a>
EOT_IN

$rc = &TEST::system("$ENV{WML} a.wml 2>&1");
print ($rc == 0 ? "ok\n" : "not ok\n");
$rc = &TEST::system("cmp a.en.html $tmpfile1 && cmp a.de.html $tmpfile2");
print ($rc == 0 ? "ok\n" : "not ok\n");

push(@TEST::TMPFILES, "a.en.html");
push(@TEST::TMPFILES, "a.de.html");
&TEST::cleanup;

