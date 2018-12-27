use strict;
use warnings;

use Test::More tests => 1;

{
    my $out = `wml-params-conf -h`;

    # TEST
    like( $out, qr/use lib do/, "help contains text" );
}
