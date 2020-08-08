
use strict;
use warnings;

use WmlTest ();
WmlTest::init();

use Test::More tests => 4;

#
#   TEST 1-2: throughput
#

{
    my $in = <<'EOT_IN';
foo bar baz quux
öäüÖÄÜß
!"§$%&/()=?`'*+
EOT_IN

    my $out = <<'EOT_OUT';
foo bar baz quux
öäüÖÄÜß
!"§$%&/()=?`'*+
EOT_OUT

    # TEST*2
    WmlTest::all_passes( $in, $out, '-Dbar -Dvoid=\"\" -Dvoid2=' );
}

{
    my $in = <<'EOT_IN';
<protect pass=2-9>\
$(bar)
$(void)
$(void2)
$(ROOT)\
</protect>
EOT_IN

    my $out = <<'EOT_OUT';
1


.
EOT_OUT

    # TEST*2
    WmlTest::all_passes( $in, $out, '-Dbar -Dvoid=\"\" -Dvoid2= -DROOT=.', );
}

WmlTest::cleanup();

