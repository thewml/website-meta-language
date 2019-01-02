#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;

{
    open my $in, "man ./wml_frontend/wmk.1 | cat |"
        or die "Cannot open $! !";
    local $/;
    my $text = <>;

    # TEST
    unlike( $text, qr/\@WML_VERSION/, "WML_VERSION was expanded" );
}
__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2009 Shlomi Fish

=cut
