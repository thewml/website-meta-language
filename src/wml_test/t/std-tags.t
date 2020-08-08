use strict;
use warnings;

use WmlTest ();
WmlTest::init();

use Test::More tests => 2;

my $pass = "1-9";

# TEST*2
WmlTest::generic( $pass, <<'EOT_IN', <<'EOT_OUT', '' );
#use wml::std::tags
<define-tag nesting endtag=required whitespace=delete>
<perl>
    <perl:assign:sq $body>%body</perl:assign:sq>
    <perl:print: $body />
</perl>
</define-tag>
<nesting><nesting>ABC</nesting></nesting>
<nesting>D<nesting>E</nesting>F</nesting>
EOT_IN
ABC
DEF
EOT_OUT

WmlTest::cleanup();
