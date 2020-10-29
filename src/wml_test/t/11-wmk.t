use strict;
use warnings;

use Term::ANSIColor qw/ colored /;
use Path::Tiny qw/ cwd path /;

use WmlTest ();
WmlTest::init();

use Test::More tests => 8;

#
#   TEST 1-2: throughput
#

my ($rc);

my $wmk = $ENV{WML};
$wmk =~ s/l /k /;
$wmk =~ s/l$/k/;
my $abs_wmk = path($wmk)->absolute;

WmlTest::tmpfile_with_name( 'a.html', "x" );
WmlTest::tmpfile_with_name( 'a',      <<'EOT_IN');
foo
EOT_IN
WmlTest::tmpfile_with_name( 'a.wml', <<"EOT_IN");
#include 'a'
bar
EOT_IN

my $tmpfile1 = WmlTest::tmpfile(<<'EOT_IN');
foo
bar
EOT_IN

# TEST
is(
    ( scalar `$wmk a.wml 2>&1` ),
    "$ENV{WML} -n -q -W \"1,-N\" -o a.html a.wml\n",
    "wmk output 1"
);

# TEST
ok( !system("cmp $tmpfile1 a.html"), "cmp 1" );

# TEST
is(
    ( scalar `$wmk a.wml 2>&1` ),
"$ENV{WML} -n -q -W \"1,-N\" -o a.html a.wml  (@{[colored('skipped', 'bold')]})\n"
);

{
    open( my $o, '>>', 'a' );
    print {$o} " ";
    close($o);
}

# TEST
is(
    ( scalar `$wmk a.wml 2>&1` ),
    "$ENV{WML} -n -q -W \"1,-N\" -o a.html a.wml\n",
    "wmk 2"
);

$tmpfile1 = WmlTest::tmpfile(<<'EOT_IN');
Hello
EOT_IN
my $tmpfile2 = WmlTest::tmpfile(<<'EOT_IN');
Willkommen
EOT_IN
WmlTest::tmpfile_with_name( 'a.wml', <<'EOT_IN');
#!wml -o (ALL-LANG_*)+LANG_EN:%BASE.en.html \
      -o (ALL-LANG_*)+LANG_DE:%BASE.de.html
#use wml::std::lang
<lang:new id=en short>
<lang:new id=de short>
<en>Hello</en><de: Willkommen>
EOT_IN

# TEST
is(
    ( scalar `$wmk a.wml 2>&1` ),
"$ENV{WML} -n -q -W \"1,-N\" -o '(ALL-LANG_*)+LANG_EN:a.en.html' -o '(ALL-LANG_*)+LANG_DE:a.de.html' a.wml\n",
    "WMK 4"
);

# TEST
ok( !system("cmp $tmpfile1 a.en.html"), "en cmp" );

# TEST
ok( !system("cmp $tmpfile2 a.de.html"), "de cmp" );

WmlTest::add_files("a.en.html");
WmlTest::add_files("a.de.html");

{
    my $tempdir_ret = WmlTest::new_tempdir();
    my $old         = $tempdir_ret->{orig};
    my $new         = $tempdir_ret->{new};
    $new->child(".wmkrc")->spew_utf8(<<'EOF');
-o (ALL-LANG_*)+LANG_EN:%BASE.en.html -o (ALL-LANG_*)+LANG_DE:%BASE.de.html
-A *.wml
-A *.md
EOF
    $new->child(".wmlrc")
        ->spew_utf8(
"-o (ALL-LANG_*)+LANG_EN:%BASE.en.html -o (ALL-LANG_*)+LANG_DE:%BASE.de.html\n"
        );

    $new->child("foo.wml")->spew_utf8("<de>deutsch</de><en>english</en>\n");

    # diag($abs_wmk);

    # TEST
    like(
        ( scalar `$abs_wmk 2>&1` ),
        qr#\Q$ENV{WML}\E.*?-o\s*\(ALL-LANG#ms,
        "wmkrc -o",
    );
    chdir($old);
}

WmlTest::cleanup();
