use strict;
use warnings;

use Test::More tests => 4;

use WmlTest ();

WmlTest::init();

#
#   TEST 1-4: throughput
#
my $temp_fn_1 = WmlTest::tmpfile_with_name( 'a', <<'EOT');
a1
#include 'b'
a2
EOT

WmlTest::tmpfile_with_name( 'b', <<'EOT');
b1
#include 'c'
b2
EOT

WmlTest::tmpfile_with_name( 'c', <<'EOT');
c
EOT

my $temp_fn_2 = WmlTest::tmpfile(<<'EOT');
a1
b1
c
b2
a2
EOT

my $temp_fn_3 = WmlTest::tmpfile();

# TEST
ok( !system("$ENV{WML} -p1 $temp_fn_1 >$temp_fn_3"), "System 1" );

# TEST
ok( !system("cmp $temp_fn_2 $temp_fn_3"), "System 2" );

$temp_fn_1 = WmlTest::tmpfile(<<'EOT');
#include "b" bar=1
foo=$(foo) \
baz=$(baz)
# comment
EOT

WmlTest::tmpfile_with_name( 'b', <<'EOT');
bar=$(bar)
__END__
skipped
EOT

$temp_fn_2 = WmlTest::tmpfile(<<'EOT');
bar=1
foo=1 baz=
EOT

# TEST
ok( !system("$ENV{WML} -p1 -Dfoo=1 $temp_fn_1 >$temp_fn_3"), "WML System 3", );

# TEST
ok( !system("cmp $temp_fn_2 $temp_fn_3"), "CMP System 4", );

WmlTest::cleanup();
