use strict;
use warnings;

use WmlTest ();
WmlTest::init();

use Test::More tests => 2;

#   Test if PNG support was found
my $png_support = 0;

=begin removed

my $conf;
{
  open (my $IN, '<', '../wml_common/gd/Makefile');
  local $/ = undef;
  $conf = <$IN>;
  close ($IN);
  $png_support = ($conf =~ m/-lpng/);
}

=end removed

=cut

if ($png_support)
{
    #   PNG support available
    # TEST*2
    WmlTest::all_passes( <<'EOT_IN', <<'EOT_OUT', '' );
#use wml::des::space
<space format=png width=6 height=4>
<space format=gif width=6>
<space height=4>
<space>
EOT_IN
<img src="imgdot-1x1-transp-ffffff.png" alt="" width="6" height="4" align="bottom" border="0">
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="6" height="1" align="bottom" border="0">
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="4" align="bottom" border="0">
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="1" align="bottom" border="0">
EOT_OUT
}
else
{
    #   PNG support unavailable
    WmlTest::all_passes( <<'EOT_IN', <<'EOT_OUT', '' );
#use wml::des::space
<space format=gif width=6>
<space height=4>
<space>
EOT_IN
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="6" height="1" align="bottom" border="0">
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="4" align="bottom" border="0">
<img src="imgdot-1x1-transp-ffffff.gif" alt="" width="1" height="1" align="bottom" border="0">
EOT_OUT
}

#   If the variable @TEST::TMPFILES appears only once, a warning
#   is reported
WmlTest::add_files(
    "imgdot-1x1-transp-ffffff.gif",
    "imgdot-1x1-transp-ffffff.png",
);

WmlTest::cleanup();
