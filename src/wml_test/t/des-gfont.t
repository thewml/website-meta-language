
require "TEST.pl";
&TEST::init;

$exists = 0;
foreach (split(/:/, $ENV{'PATH'})) {
    if (-x "$_/gfont") {
        $exists = 1;
        last;
    }
}
if (not $exists) {
  $| = 1;
  print STDERR "(skipped) ";
  print "1..1\nok\n";
  exit (0);
}

print "1..6\n";

$pass = "1-9";

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '');
#use wml::des::gfont
<gfont notag>foo</gfont>
void
EOT_IN
void
EOT_OUT

push(@TEST::TMPFILES, qw(tmp.00.gfont000.gif tmp.00.gfont000.gif.cmd));

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '-Dbar -Dvoid=\"\" -Dvoid2=');
#use wml::des::gfont
<gfont file="tmp.gif">foo</gfont>
EOT_IN
<img src="tmp.gif" alt="foo" width="24" height="22" border="0">
EOT_OUT

push(@TEST::TMPFILES, qw(tmp.gif tmp.gif.cmd));

&TEST::generic($pass, <<'EOT_IN', <<'EOT_OUT', '-Dbar -Dvoid=\"\" -Dvoid2=');
#use wml::des::gfont
<gfont base="tmp">foo</gfont>
EOT_IN
<img src="tmp.gfont000.gif" alt="foo" width="24" height="22" border="0">
EOT_OUT

push(@TEST::TMPFILES, qw(tmp.gfont000.gif tmp.gfont000.gif.cmd));

&TEST::cleanup;

