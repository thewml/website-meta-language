use strict;
use warnings;

use Test::More tests => 4;

use WmlTest ();
WmlTest::init();

# TEST*2
WmlTest::all_passes( <<'EOT_IN', <<'EOT_OUT', '' );
#use wml::des::navbar
<navbar:define name=test>
<navbar:button id=foo url="foo.html" txt="foo">
<navbar:button id=bar url="bar.html" txt="bar">
</navbar:define>
<navbar:render name=test>
EOT_IN
<a href="foo.html" onmouseover="self.status = 'foo.html'; return true" onmouseout="self.status = ''; return true" onfocus="self.status = 'foo.html'; return true" onblur="self.status = ''; return true">foo</a><a href="bar.html" onmouseover="self.status = 'bar.html'; return true" onmouseout="self.status = ''; return true" onfocus="self.status = 'bar.html'; return true" onblur="self.status = ''; return true">bar</a>
EOT_OUT

# TEST*2
WmlTest::all_passes( <<'EOT_IN', <<'EOT_OUT', '' );
#use wml::des::navbar
<navbar:define name=test>
<navbar:button id=foo url="foo.html" txt="foo">
<navbar:button id=bar url="bar.html" txt="bar">
</navbar:define>
<navbar:render name=test select=foo>
EOT_IN
foo<a href="bar.html" onmouseover="self.status = 'bar.html'; return true" onmouseout="self.status = ''; return true" onfocus="self.status = 'bar.html'; return true" onblur="self.status = ''; return true">bar</a>
EOT_OUT

WmlTest::cleanup();
