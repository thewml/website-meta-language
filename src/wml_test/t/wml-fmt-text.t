use strict;
use warnings;

use WmlTest ();
WmlTest::init();

use Test::More;
use Path::Tiny qw/ tempfile /;
use File::Which qw/ which /;

unless ( which 'txt2html' )
{
    plan skip_all => "Skipping because txt2html is not installed";
}

plan tests => 4;

my $text_in = <<'EOT_IN';
FOO
===

1. bar
2. quux
   a. baz
   b. foo
EOT_IN

#
#   TEST 1-2: inline
#

my $out = <<'EOT_OUT';
<h1><a name="section_1">FOO</a></h1>
<ol>
  <li>bar
  </li><li>quux
  <ol>
    <li>baz
    </li><li>foo
  </li></ol>
</li></ol>
EOT_OUT

{
    my $in = <<"EOT_IN";
#use wml::fmt::text

<text notypo>
$text_in
</text>
EOT_IN

    # TEST*2
    WmlTest::all_passes( $in, $out, '-Dbar -Dvoid=\"\" -Dvoid2=' );
}

#
#   TEST 3-4: external via function call
#

{
    my $tmpfile = tempfile();
    $tmpfile->spew($text_in);
    my $in = <<"EOT_IN";
#use wml::fmt::text

<: print &wml_fmt_text({ FILE => '$tmpfile', OPTIONS => '--xhtml'}); :>
EOT_IN

    # TEST*2
    WmlTest::all_passes( $in, $out, '-Dbar -Dvoid=\"\" -Dvoid2=' );
}

WmlTest::cleanup();
