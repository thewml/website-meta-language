package WML_Backends;

use strict;
use warnings;

use IO::All qw/ io /;

sub out
{
    my ( $self, $opt_o, $err_subref, $output_aref ) = @_;

    ( $opt_o eq '-' ? io('-') : io->file($opt_o) )->print(@$output_aref);

    return;
}

sub input
{
    my ( $self, $argv, $err_subref, $usage ) = @_;

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

