package WML_Backends;

use strict;
use warnings;

use IO::Handle;

sub out
{
    my ($self, $opt_o, $err_subref, $output_aref) = @_;
    #
    #   create output file
    #
    my $out_fh;
    if ($opt_o eq '-') {
        $out_fh = IO::Handle->new;
        $out_fh->fdopen(fileno(STDOUT), "w") || $err_subref->("cannot write into STDOUT: $!");
    }
    else {
        open $out_fh, '>', $opt_o
            or $err_subref->("cannot write into $opt_o: $!");
    }
    $out_fh->print(@$output_aref)
    || $err_subref->("cannot write into $opt_o: $!");
    $out_fh->close() || $err_subref->("cannot close $opt_o: $!");

    return;
}

1;

