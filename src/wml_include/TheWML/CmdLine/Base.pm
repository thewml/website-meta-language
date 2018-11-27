package TheWML::CmdLine::Base;

use strict;
use warnings;

use TheWML::CmdLine::IO ();

sub _gen_opt
{
    my ( $self, $opt ) = @_;

    return sub { my ( undef, $v ) = @_; $self->$opt($v); },;
}

sub error
{
    my ( $self, $str ) = @_;
    die "** " . $self->_name() . ":Error: $str\n";
}

sub verbose
{
    my ( $self, $str ) = @_;
    if ( $self->opt_v )
    {
        print STDERR "** " . $self->_name . ":Verbose: $str\n";
    }
}

sub _out
{
    my ( $self, $opt_o, $strs ) = @_;
    TheWML::CmdLine::IO->out( $opt_o, $strs, );

    return;
}

sub _input
{
    my $self = shift;

    return TheWML::CmdLine::IO->input( $self->argv,
        sub { return $self->usage(@_); },
    );
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
