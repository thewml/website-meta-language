#!/usr/bin/env perl

use strict;
use warnings;
use autodie;

use Getopt::Long qw/ GetOptions /;
use Path::Tiny qw/ cwd path /;

sub do_system
{
    my $cmd = shift;
    print "Running [@$cmd]\n";
    if ( system(@$cmd) )
    {
        die "Running [@$cmd] failed!";
    }
}
my $is_interactive;

GetOptions( 'interactive!' => \$is_interactive, );

my $IS_WIN = ( $^O eq "MSWin32" );
my $SEP    = $IS_WIN ? "\\" : '/';

# %ENV path separator
my $P    = $IS_WIN ? ';'     : ':';
my $MAKE = $IS_WIN ? 'gmake' : 'make';

my $cmake_gen;
if ($IS_WIN)
{
    $cmake_gen = 'MSYS Makefiles';
}
my $src_dir    = path($0)->parent(2)->absolute;
my $script_dir = path(__FILE__)->parent->absolute;
my $myprefix   = cwd()->child(qw/ tests installation /);

my $build_dir = path("./FOO")->absolute;

if ( !-e $myprefix )
{
    $build_dir->remove_tree;
    if (
        do_system(
            [
                      "mkdir $build_dir && cd $build_dir && cmake "
                    . ( defined($cmake_gen) ? qq#-G "$cmake_gen"# : "" )
                    . " -D CMAKE_INSTALL_PREFIX=$myprefix $src_dir && $MAKE VERBOSE=1 && $MAKE package_source && $MAKE install"
            ]
        )
        )
    {
        $myprefix->remove_tree;
        die "cmake Failed";
    }
}

my $P5L = $ENV{PERL5LIB} ? $ENV{PERL5LIB} . $P : $P;
$ENV{PERL5LIB}              = "$myprefix/lib/perl5$P$P5L$script_dir";
$ENV{QUAD_PRES_NO_HOME_LIB} = 1;
$ENV{PATH}                  = "$myprefix/bin$P$ENV{PATH}";
$ENV{WML}                   = "$myprefix/bin/wml -q -W1,-N";
$ENV{LANG}                  = $ENV{LC_ALL} = 'C';

chdir($script_dir);

do_system( [ 'which',      'perlcritic' ] );
do_system( [ 'bash',       '-c', q{echo "$PATH"}, ] );
do_system( [ 'perlcritic', '--version', ] );
if ($is_interactive)
{
    system("bash");
}
else
{
    do_system(
        [
            'prove',
            ( $ENV{WML_TEST_QUIET} ? () : ('-v') ),
            glob( 't' . $SEP . '*.t' ),
        ],
    );
}

=head1 COPYRIGHT & LICENSE

Copyright 2014 by Shlomi Fish

This program is distributed under the MIT (X11) License:
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
