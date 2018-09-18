
use strict;
use warnings;

use Test::More tests => 2;
use File::Temp qw/ tempdir tempfile /;
use IO::All qw/ io /;

my $pass = 3;

my $dir = tempdir( CLEANUP => 1 );

sub tmpfile
{
    my ( $fh, $filename ) = tempfile( DIR => $dir );

    print {$fh} @_;

    return $filename;
}

sub generic
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ( $pass, $in, $out, $opt ) = @_;

    # local($tmpfile1, $tmpfile2, $tmpfile3, $rc);
    my $tmpfile1 = tmpfile($in);
    my $tmpfile2 = tmpfile($out);
    my $tmpfile3 = tmpfile();
    my $rc       = system("$ENV{WML_P3} $tmpfile1 >$tmpfile3");

    Test::More::ok( !$rc, "generic system wml" );

    # $rc = system("cmp $tmpfile2 $tmpfile3");

    Test::More::is(
        io()->file($tmpfile3)->all(),
        io()->file($tmpfile2)->all(),
        "generic cmp"
    );
}
generic( $pass, <<'EOT_IN', <<'EOT_OUT', '' );
<:
    print "foo";
:>
EOT_IN
foo
EOT_OUT
