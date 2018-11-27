package TheWML::Backends;

use strict;
use warnings;

use IO::All qw/ io /;

sub out
{
    my ( $self, $opt_o, $output_aref ) = @_;

    ( $opt_o eq '-' ? io('-') : io->file($opt_o) )->print(@$output_aref);

    return;
}

sub input
{
    my ( $self, $argv, $usage ) = @_;

    my @local_argv = @$argv;
    my $foo_buffer;

    if ( ( @local_argv == 1 and $local_argv[0] eq '-' ) or !@local_argv )
    {
        $foo_buffer = io('-')->all;
    }
    elsif ( @local_argv == 1 )
    {
        $foo_buffer = io->file( $local_argv[0] )->all;
    }
    else
    {
        $usage->();
    }

    return $foo_buffer;
}

1;

__END__

=head1 COPYRIGHT & LICENSE

Copyright 2018 by Shlomi Fish

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
