use strict;
use warnings;

use Test::More;

use WmlTest ();
WmlTest::init();

my $exists = 0;
foreach ( split( /:/, $ENV{'PATH'} ) )
{
    if ( -x "$_/gfont" )
    {
        $exists = 1;
        last;
    }
}
if ( not $exists )
{
    plan skip_all => "gfont not found";
}
else
{
    plan tests => 6;
}

# TEST*2
WmlTest::all_passes( <<'EOT_IN', <<'EOT_OUT', '' );
#use wml::des::gfont
<gfont notag>foo</gfont>
void
EOT_IN
void
EOT_OUT

WmlTest::add_files(qw(tmp.00.gfont000.gif tmp.00.gfont000.gif.cmd));

# TEST*2
WmlTest::all_passes( <<'EOT_IN', <<'EOT_OUT', '-Dbar -Dvoid=\"\" -Dvoid2=' );
#use wml::des::gfont
<gfont file="tmp.gif">foo</gfont>
EOT_IN
<img src="tmp.gif" alt="foo" width="24" height="22" border="0">
EOT_OUT

WmlTest::add_files(qw(tmp.gif tmp.gif.cmd));

# TEST*2
WmlTest::all_passes( <<'EOT_IN', <<'EOT_OUT', '-Dbar -Dvoid=\"\" -Dvoid2=' );
#use wml::des::gfont
<gfont base="tmp">foo</gfont>
EOT_IN
<img src="tmp.gfont000.gif" alt="foo" width="24" height="22" border="0">
EOT_OUT

WmlTest::add_files(qw(tmp.gfont000.gif tmp.gfont000.gif.cmd));

WmlTest::cleanup();
