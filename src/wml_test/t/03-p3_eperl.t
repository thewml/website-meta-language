use strict;
use warnings;

use Test::More tests => 2;
use File::Temp qw/ tempdir tempfile /;

my $dir = tempdir( CLEANUP => 1 );

my $tmpfile3 = "$dir/out.txt";
my $rc       = system("$ENV{WML_P3} > $tmpfile3");

Test::More::ok( !$rc, "generic system wml" );

sub _slurp
{
    my $filename = shift;

    open my $in, '<', $filename
        or die "Cannot open '$filename' for slurping - $!";

    local $/;
    my $contents = <$in>;

    close($in);

    return $contents;
}

Test::More::is( _slurp($tmpfile3), "foo\n", "generic cmp" );
