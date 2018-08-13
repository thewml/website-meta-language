package WML_Backends;

use strict;
use warnings;

use IO::Handle;

sub out
{
    my ( $self, $opt_o, $err_subref, $output_aref ) = @_;
    #
    #   create output file
    #
    my $out_fh;
    if ( $opt_o eq '-' )
    {
        $out_fh = IO::Handle->new;
        $out_fh->fdopen( fileno(STDOUT), "w" )
            || $err_subref->("cannot write into STDOUT: $!");
    }
    else
    {
        open $out_fh, '>', $opt_o
            or $err_subref->("cannot write into $opt_o: $!");
    }
    $out_fh->print(@$output_aref)
        || $err_subref->("cannot write into $opt_o: $!");
    $out_fh->close() || $err_subref->("cannot close $opt_o: $!");

    return;
}

sub input
{
    my ( $self, $argv, $err_subref, $usage ) = @_;

    my @local_argv = @$argv;
    my $foo_buffer;

    if ( ( @local_argv == 1 and $local_argv[0] eq '-' ) or !@local_argv )
    {
        my $in = IO::Handle->new;
        $in->fdopen( fileno(STDIN), 'r' )
            || $err_subref->("cannot load STDIN: $!");
        local $/;
        $foo_buffer = <$in>;
        $in->close() || $err_subref->("cannot close STDIN: $!");
    }
    elsif ( @local_argv == 1 )
    {
        open my $in, '<', $local_argv[0]
            or $err_subref->("cannot load $local_argv[0]: $!");
        local $/;
        $foo_buffer = <$in>;
        $in->close() || $err_subref->("cannot close $local_argv[0]: $!");
    }
    else
    {
        $usage->();
    }

    return $foo_buffer;

}

1;

