#! /usr/bin/env perl
#
# Embeds a text file as C code.
#
# Version 0.0.1
# Copyright (C) 2020 Shlomi Fish < https://www.shlomifish.org/ >
#
# Licensed under the terms of the MIT license.
#
# MITLed rewrite of asc2c by Ralf S. Engelschall.

use strict;
use warnings;
use 5.014;
use autodie;

use Path::Tiny qw/ path /;

my $in    = path(shift);
my $out_c = shift;
my $o     = path( $out_c . '.c' );
my $name  = shift;
die "extraneous @ARGV" if @ARGV;
my $l = -s $in;
$o->spew_raw(
    "/* Automatically generated by $0 */\n",
    "const char * const ${name} = \n",
    (
        map {
            chomp;
            s/\\/\\\\/g;
            s/"/\\\"/g;
            qq#"$_\\n"\n#;
        } $in->lines_raw()
    ),
    ";\n",
);

__END__

=head1 COPYRIGHT & LICENSE

Copyright 2020 by Shlomi Fish

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
