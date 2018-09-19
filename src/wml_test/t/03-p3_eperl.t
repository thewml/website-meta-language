use strict;
use warnings;

use Test::More tests => 2;
use File::Temp qw/ tempdir tempfile /;

my $dir = tempdir( CLEANUP => 1 );

sub tmpfile
{
    my ( $fh, $filename ) = tempfile( DIR => $dir );

    print {$fh} @_;

    return $filename;
}
my $out = <<'EOT_OUT';
foo
EOT_OUT

# local($tmpfile1, $tmpfile2, $tmpfile3, $rc);
my $tmpfile2 = tmpfile($out);
my $tmpfile3 = tmpfile();
my $rc       = system("$ENV{WML_P3} > $tmpfile3");

Test::More::ok( !$rc, "generic system wml" );

# $rc = system("cmp $tmpfile2 $tmpfile3");
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

Test::More::is( _slurp($tmpfile3), _slurp($tmpfile2), "generic cmp" );
