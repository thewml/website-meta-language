
use strict;
use warnings;

use WmlTest ();
WmlTest::init();

use Test::More tests => 2;

WmlTest::tmpfile_with_name( 'a.wml', <<'EOT_IN');
#use wml::std::lang
<lang:new id=en short>
<lang:new id=de short>
<lang:star:slice:>
<a href="<lang:star: $(WML_SRC_BASENAME).*.html>">Link</a>
EOT_IN

my $tmpfile1 = WmlTest::tmpfile(<<'EOT_IN');
<a href="a.en.html">Link</a>
EOT_IN

my $tmpfile2 = WmlTest::tmpfile(<<'EOT_IN');
<a href="a.de.html">Link</a>
EOT_IN

# TEST
ok( !system("$ENV{WML} a.wml 2>&1"), "wml" );

# TEST
ok( !system("cmp a.en.html $tmpfile1 && cmp a.de.html $tmpfile2"), "cmp" );

WmlTest::add_files("a.en.html");
WmlTest::add_files("a.de.html");
WmlTest::cleanup();

