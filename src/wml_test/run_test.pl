#!/usr/bin/env perl

use strict;
use warnings;

use File::Path qw / rmtree /;
use Path::Tiny qw/ path /;

sub do_system
{
    my ($args) = @_;

    my $cmd = $args->{cmd};
    print "Running [@$cmd]\n";
    if ( system(@$cmd) )
    {
        die "Running [@$cmd] failed!";
    }
}
my $IS_WIN = ( $^O eq "MSWin32" );
my $SEP = $IS_WIN ? "\\" : '/';

# %ENV path separator
my $P    = $IS_WIN ? ';'     : ':';
my $MAKE = $IS_WIN ? 'gmake' : 'make';

my $cmake_gen;
if ($IS_WIN)
{
    $cmake_gen = 'MSYS Makefiles';
}
my $tatzer_dir =  path($0)->parent->parent->absolute ;
my $script_dir = path(__FILE__)->parent->absolute;
my $myprefix   = Path::Tiny->cwd->child(qw(tests installation));

my $build_dir = "FOO";

if ( !-e $myprefix )
{
    rmtree( [$build_dir] );
    if (
        do_system(
            {
                cmd => [
"cd . && mkdir $build_dir && cd $build_dir && cmake "
                        . ( defined($cmake_gen) ? qq#-G "$cmake_gen"# : "" )
                        . " -DCMAKE_INSTALL_PREFIX=$myprefix $tatzer_dir && $MAKE && $MAKE install"
                ]
            }
        )
        )
    {
        rmtree( [$myprefix] );
        die "cmake Failed";
    }
}

my $P5L = $ENV{PERL5LIB} ? $ENV{PERL5LIB} . $P : $P;
$ENV{PERL5LIB}              = "$myprefix/lib/perl5$P$P5L$script_dir";
$ENV{QUAD_PRES_NO_HOME_LIB} = 1;
$ENV{PATH}                  = "$myprefix/bin$P$ENV{PATH}";
$ENV{WML}                   = "$myprefix/bin/wml -q -W1-N";
( $ENV{WML_P3} ) = glob("$myprefix/*/wml/exec/wml_p3_eperl*");
$ENV{LANG} = $ENV{LC_ALL} = 'C';

chdir($script_dir);

do_system( { cmd => [ 'bash', '-c', q{echo "$PATH"}, ] } );
if ( $ENV{INTERACTIVE} )
{
    system("bash");
}
else
{
    do_system(
        {
            cmd => [
                'prove', ( $ENV{WML_TEST_QUIET} ? () : ('-v') ),
                glob("t${SEP}*.t"),
            ],
        }
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
