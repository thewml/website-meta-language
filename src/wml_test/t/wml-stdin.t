#!/usr/bin/env perl

use strict;
use warnings;
use Test::More tests => 4;
use File::Temp qw/ tempdir tempfile /;

my $dir = tempdir( CLEANUP => 1 );

sub tmpfile
{
    my ( $fh, $filename ) = tempfile( DIR => $dir );

    print {$fh} @_;

    return $filename;
}

# See https://github.com/thewml/website-meta-language/issues/24
# Thanks to @xtaran
# " wml throws several "uninitialized value" warnings on empty input " (from stdin)

{
    my $empty_fn = tmpfile;
    my $output   = `$ENV{WML} < $empty_fn 2>&1`;
    my $rc       = $?;

    # TEST
    like( $output, qr#\A[\s\n\r]{0,10}\z#ms,
        "empty output and no warnings when accepting empty input from stdin" );

    # TEST
    is( $rc, 0, "success exit" );
}

{
    my $foo_fn = tmpfile("foo\n");
    my $output = `$ENV{WML} < $foo_fn 2>&1`;
    my $rc     = $?;

    # TEST
    like( $output, qr#\Afoo[\s\n\r]{0,10}\z#ms,
        "correct output and no warnings when accepting small input from stdin"
    );

    # TEST
    is( $rc, 0, "success exit" );
}

__END__

=head1 COPYRIGHT & LICENSE

Copyright 2019 by Shlomi Fish

This program is distributed under the MIT / Expat License:
L<http://www.opensource.org/licenses/mit-license.php>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut
